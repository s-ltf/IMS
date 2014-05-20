#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <pthread.h>
#include <unistd.h>
#ifdef OCR
#include "OCR.h"
#endif
#include "Client.h"
//new
#include <cv.h>
#include <highgui.h>
#include "time.h"

//Headers compiled as C
extern "C"{
#include "bcm_host.h"
#include "interface/vcos/vcos.h"

#include "interface/mmal/mmal.h"
#include "interface/mmal/mmal_logging.h"
#include "interface/mmal/mmal_buffer.h"
#include "interface/mmal/util/mmal_util.h"
#include "interface/mmal/util/mmal_util_params.h"
#include "interface/mmal/util/mmal_default_components.h"
#include "interface/mmal/util/mmal_connection.h"

#include "RaspiCamControl.h"
#include "RaspiPreview.h"
#include "RaspiCLI.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
}
#include <semaphore.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>

#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/video/tracking.hpp"

#include <sys/time.h>

using namespace cv;
using namespace std;

//determine if in debug mode
#define DEBUG 0

Mat gray,frame;

vector<Mat> images;
vector<int> labels;

//----Initial HSV filter values---
#if (DEBUG == 1)
//testing card
int H_MIN = 93;
int H_MAX = 143;
int S_MIN = 107;
int S_MAX = 256;
int V_MIN =71;
int V_MAX = 171;
#endif

#if (DEBUG == 0)
//door
int H_MIN = 41;
int H_MAX = 85;
int S_MIN = 123;
int S_MAX = 256;
int V_MIN = 56;
int V_MAX = 184;
#endif

//------CONSTANT VALUES----------
//Window names
const string originalImage = "Original Image";
const string hsvImage = "HSV Image";
const string thresholdImage = "Threshold Image";
const string morphImage = "Morphological Operation";
const string trackBars = "TrackBars";

//Image containers for 4  different image types
Mat cameraFeed;
Mat HSV;
Mat thresh;
Mat contourImage;

//external variable
size_t mat_size;

//POSIX thread identifiers
#ifdef OCR
pthread_t ocrThread;
#endif
pthread_t clientThread;
pthread_t keyThread;

//mutex
pthread_mutex_t keyLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t poiLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t netLock = PTHREAD_MUTEX_INITIALIZER;

//stack to store incoming MMU data
stack<string> mmu_data;

#if (DEBUG == 1)
#define AREA_MIN 5000
#define AREA_MAX 6000
#define PERIMETER_MIN 500
#define PERIMETER_MAX 700
#define ASPECT_RATIO_MIN 1.30
#define ASPECT_RATIO_MAX 1.40
#endif

#if (DEBUG == 0)
#define AREA_MIN 50000
#define AREA_MAX
#define PERIMETER_MIN 1200
#define PERIMETER_MAX 900
#define ASPECT_RATIO_MIN 1.10
#define ASPECT_RATIO_MAX 1.30
#endif

// save file
#ifdef OCR
#define SAVE_FILE "image_data"
#endif

//image name
#define IMAGE_NAME "door_image"

//host information
#define HOST "192.168.10.2" //"172.17.147.251" 
#define PORT 5005

/// Camera number to use - we only have one camera, indexed from 0.
#define CAMERA_NUMBER 0

// Standard port setting for the camera component
#define MMAL_CAMERA_PREVIEW_PORT 0
#define MMAL_CAMERA_VIDEO_PORT 1
#define MMAL_CAMERA_CAPTURE_PORT 2

// Video format information
#define VIDEO_FRAME_RATE_NUM 30
#define VIDEO_FRAME_RATE_DEN 1

/// Video render needs at least 2 buffers.
#define VIDEO_OUTPUT_BUFFERS_NUM 3

// Max bitrate we allow for recording
const int MAX_BITRATE = 30000000; // 30Mbits/s


// variable to convert I420 frame to IplImage
int nCount=0;
IplImage *py, *pu, *pv, *pu_big, *pv_big, *image,* dstImage;

//Function Decleratoins
int mmal_status_to_int(MMAL_STATUS_T status);
void DrawContour(vector< vector<Point> > contour, vector<Vec4i> hierarchy, Size size);
void ContourArea(vector< vector<Point> > contour);
string ImageData(string image);
void *ImageData(void* arg);
void *Socket(void* arg);
void SaveImage();
void ContourFilter(vector< vector<Point> > contour, vector<Vec4i> hierarchy, Size size);


//------------------STRUCTURE DEFINITIONS-----------------------------
/*
 Structure containing all state information for the current run
*/
typedef struct
{
	int timeout;                        /// Time taken before frame is grabbed and app then shuts down. Units are milliseconds
	int width;                          /// Requested width of image
	int height;                         /// requested height of image
	int bitrate;                        /// Requested bitrate
	int framerate;                      /// Requested frame rate (fps)
	int graymode;			/// capture in gray only (2x faster)
	int immutableInput;      /// Flag to specify whether encoder works in place or creates a new buffer. Result is preview can display either
	/// the camera output or the encoder output (with compression artifacts)
	RASPIPREVIEW_PARAMETERS preview_parameters;   /// Preview setup parameters
	RASPICAM_CAMERA_PARAMETERS camera_parameters; /// Camera setup parameters

	MMAL_COMPONENT_T *camera_component;    /// Pointer to the camera component
	MMAL_COMPONENT_T *encoder_component;   /// Pointer to the encoder component
	MMAL_CONNECTION_T *preview_connection; /// Pointer to the connection from camera to preview
	MMAL_CONNECTION_T *encoder_connection; /// Pointer to the connection from camera to encoder

	MMAL_POOL_T *video_pool; /// Pointer to the pool of buffers used by encoder output port
}RASPIVID_STATE;

/*
 * Struct used to pass information in encoder port userdata to callback
 */
typedef struct
{
	FILE *file_handle;                   /// File handle to write buffer data to.
	VCOS_SEMAPHORE_T complete_semaphore; /// semaphore which is posted when we reach end of frame (indicates end of capture or fault)
	RASPIVID_STATE *pstate;            /// pointer to our state in case required in callback
}PORT_USERDATA;

//Struct to pass POI data as a group.
typedef struct{
	int flag;
	const char* location;
	#ifdef OCR
	const char* data;
	#endif
}POI;

//struct to pass data to communication center
typedef struct{
    int flag;			//signal to send data
    const char* uplinkData;	//data to send
    //const char* downlinkData;	//storing incoming data in buffer

}CLIENT_USERDATA;

//struct to pass OCR data
#ifdef OCR
typedef struct{
    int flag;
//    const char* imageName;
  //  const char* outputFile;
}OCR_USERDATA;
#endif

//struct for keyPress thread
typedef struct{
    int flag;
}KEY_USERDATA;
//----------------------------------------------------------------------------


//Global struct instances
POI poiData;
#ifdef OCR
OCR_USERDATA ocrData;
#endif
KEY_USERDATA keyData;
CLIENT_USERDATA netData;


/*This function is called whenver trackbar
 *position is changed
 */
void OnChange( int, void* ){/*do nothing*/}

string IntegerToString(int num)
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

/*
 * Create trackbar window to change
 * min & max HSV values.
 */
void CreateTrackbars()
{
	//trackbar window
	namedWindow(trackBars,0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf( TrackbarName, "H_MIN", H_MIN);
	sprintf( TrackbarName, "H_MAX", H_MAX);
	sprintf( TrackbarName, "S_MIN", S_MIN);
	sprintf( TrackbarName, "S_MAX", S_MAX);
	sprintf( TrackbarName, "V_MIN", V_MIN);
	sprintf( TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them in the window
	//OnChange function is called everytime the slider changes position
	createTrackbar( "H_MIN", trackBars, &H_MIN, 256, OnChange );
	createTrackbar( "H_MAX", trackBars, &H_MAX, 256, OnChange );
	createTrackbar( "S_MIN", trackBars, &S_MIN, 256, OnChange );
	createTrackbar( "S_MAX", trackBars, &S_MAX, 256, OnChange );
	createTrackbar( "V_MIN", trackBars, &V_MIN, 256, OnChange );
	createTrackbar( "V_MAX", trackBars, &V_MAX, 256, OnChange );
}

/*
 * Apply morphological operations to remove noise from binary image
 */
void MorphologyOperations(Mat &thresh)
{
	Mat kernelElement = getStructuringElement(MORPH_RECT,Size(5,5));
	morphologyEx(thresh,thresh,MORPH_OPEN,kernelElement);
	morphologyEx(thresh,thresh,MORPH_CLOSE,kernelElement);
}

void Contour(Mat &threshold, Mat &cameraFeed, bool drawContour, bool getArea)
{
	vector< vector<Point> > contour;
	vector<Vec4i> hierarchy;
	findContours(threshold, contour, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	if(drawContour)
		{DrawContour(contour, hierarchy, threshold.size());}
	if(getArea)
	{
		ContourFilter(contour, hierarchy, threshold.size());
	}
}

/*
 *Function responsible for taking appropriate action if a door appears in FOV
 */
void ContourFilter(vector< vector<Point> > contour, vector<Vec4i> hierarchy, Size size)
{
	string latest_location;	//latest location obtained from MMU
	float aspRatio;
	double area, perimeter;
	//convert contours to get approximate poly
	vector<vector<Point> > contours_poly(contour.size());
	vector<Rect> boundRect(contour.size());
	vector<Moments> mu(contour.size());
	for (int i = 0; i < contour.size(); i++)
	{
		//approximate poly
		approxPolyDP(Mat(contour[i]), contours_poly[i], 6, true);

		//find the bounding rectangle
		boundRect[i] = boundingRect(Mat(contours_poly[i]));

		//calculate aspect ratio
		aspRatio = (float)boundRect[i].width/(float)boundRect[i].height;
		//printf("Aspect ratio: %.2f\n", aspRatio);

		//calculate moments and store
		mu[i] = moments(contour[i], false);
		area = mu[i].m00;
		perimeter = arcLength(contour[i], true);
		//printf("Area: %.2f\n", area);
		//printf("Perimeter: %.2f\n", area);

		//check conditions
		if(area >= AREA_MIN)// && mu[i].m00 <= AREA_MAX)
			if(perimeter >= PERIMETER_MIN)// && arcLength(contour[i], true) <= PERIMETER_MAX)
 				if(aspRatio >= ASPECT_RATIO_MIN)// && aspRatio <= ASPECT_RATIO_MAX)
				{
					printf("Found object\n");
					printf("Area: %.2f\nPerimeter: %.2f\nAsp ratio: %.2f\n\n",area, perimeter, aspRatio);
					//grab latest location
					//latest_location = mmu_data.top();
					SaveImage();
					//group location & poi data
					//pthread_mutex_lock(&poiLock);
					//poiData.location = latest_location.c_str();
					//pthread_mutex_unlock(&poiLock);
					//send data
					pthread_mutex_lock(&netLock);
					netData.flag = 1;
					pthread_mutex_unlock(&netLock);
					//maybe sleep reuqired to ensure door not captured twice?
					//sleep(4);
				}
	}
}

/*
 *Function executed by network thread
 */
void *Socket(void* arg)
{
	//get data from stack
	Client cObj(HOST, PORT);
	string command;
	while (true)
	{
		if(netData.flag == 1)
		{
			cObj.Send("Door");
			//string data = mmu_data.top();
			//make CONST
			//command= "./ Client.py " + data;
			//send console command
			//system(command.c_str());
			pthread_mutex_lock(&netLock);
			netData.flag = 0;
			pthread_mutex_unlock(&netLock);
		}
		else if (netData.flag == 0)
		{
			//this logic would only work in python program itself
		}
	}
	/*-----Client.cpp----
	//initialize client & connect to host
	Client cObj(HOST, PORT);
	while(true)
	{
		//send data when flag is set
		if(netData.flag == 1)
		{
			//cObj.Send(poiData.location);
			cObj.Send("12, 13}");
			//after sending data, start listening for incoming data (bool to indicate data sent?)
			pthread_mutex_lock(&netLock);
			netData.flag = 0;
			pthread_mutex_unlock(&netLock);
		}
		else		//else constantly store received data
		{
			//push data onto stack
			mmu_data.push(cObj.Receive());
			cout << "Stack top value: " << mmu_data.top() << endl;
		}
	}
	*/
}

/* Function: ImageData
 * Argument: arg - Name of image to look for in directory
 * Function executed by OCR thread
 */
#ifdef OCR
void *ImageData(void* arg)
{
	while(true)
	{
		//run script when flag is set
		//pthread_mutex_lock(&ocrLock);
//		printf("Flag value %d\n", ocrData.flag);
		if(ocrData.flag == 1)
		{
			printf("Running OCR things.\n");
/*
			//create OCR class instance
			OCR ocr(SAVE_FILE);
			//runs the OCR operation on specified image
			ocr.Run(IMAGE_NAME);
			//set flag back to default
			pthread_mutex_lock(&ocrLock);
			ocrData.flag = 0;
			pthread_mutex_unlock(&ocrLock);

			pthread_mutex_lock(&poiLock);
			//store output in POI struct
			poiData.data = (ocr.Result()).c_str();
			pthread_mutex_unlock(&poiLock);
*/
			pthread_mutex_lock(&ocrLock);
			ocrData.flag = 0;
			pthread_mutex_unlock(&ocrLock);
			pthread_mutex_lock(&poiLock);
			poiData.data = "Sample Data";
			pthread_mutex_unlock(&poiLock);
		}
		else
		{
//			printf("Not executing OCR things.\n");
		}
	}
}
#endif

/*
 * Function: SaveImage()
 * This function saves an image from the Contour
 * matrix in a JPEG format
 */
void SaveImage()
{
	//run ethernet client on seperate thread (doesn't interefere with main program)
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
	compression_params.push_back(100);
	try
	{
		imwrite("door_image.jpeg", cameraFeed, compression_params);
	}
	catch(runtime_error& ex)
	{
		fprintf(stderr, "Exception converting image to JPEG format: %s\n", ex.what());
	}
//	fprintf(stdout, "Saved Image\n");
}

/*
 * Responsible for drawing contour of the binary image and creating a bounding rect
 */
void DrawContour(vector< vector<Point> > contour, vector<Vec4i> hierarchy, Size size)
{
	vector<Rect> boundRect(contour.size());
	vector<vector<Point> > contour_poly(contour.size());

	contourImage = Mat::zeros(size, CV_8UC3);
	//vector< vector<Point> > contourOut(contour.size());
	for(int i = 0; i < contour.size(); i++)
	{
		approxPolyDP(Mat(contour[i]),contour_poly[i],3,true);
		boundRect[i] = boundingRect(Mat(contour_poly[i]));
		drawContours(contourImage, contour, i, Scalar(255,255,255), 1, 8, hierarchy, 0, Point());
		rectangle(contourImage, boundRect[i].tl(), boundRect[i].br(),Scalar(255,255,0), 2, 8, 0);
	}
	namedWindow("Contour", CV_WINDOW_AUTOSIZE);
	imshow("Contour", contourImage);
	waitKey(5);
}

static void default_status(RASPIVID_STATE  *state)
{
	if(!state)
	{
		vcos_assert(0);
		return;
	}
	//Default everything to zero
	memset(state, 0, sizeof(RASPIVID_STATE));

	//set anything non-zero
	state->timeout = 10000;
	state->width=320;
	state->height=240;
	//state->width = 640;
	//state->height = 480;
	state->bitrate=17000000;
	state->framerate=VIDEO_FRAME_RATE_NUM;
	state->immutableInput = 1;
	state->graymode = 0;

	//Setup preview window defaults
	raspipreview_set_defaults(&state->preview_parameters);

	//Setup the camera parameters to default
	raspicamcontrol_set_defaults(&state->camera_parameters);
}
/*
 * buffer header callback function for video
 *
 * @param port Pointer to port from which callback originated
 * @param buffer mmal buffer header pointer
 */
static void video_buffer_callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
	MMAL_BUFFER_HEADER_T *new_buffer;
	PORT_USERDATA *pData = (PORT_USERDATA *)port->userdata;

	if (pData)
	{

		if (buffer->length)
		{

			mmal_buffer_header_mem_lock(buffer);

			//
			// *** PR : OPEN CV Stuff here !
			//
			int w=pData->pstate->width;	// get image size
			int h=pData->pstate->height;
			int h4=h/4;

			memcpy(py->imageData,buffer->data,w*h);	// read Y

			if (pData->pstate->graymode==0)
			{
				memcpy(pu->imageData,buffer->data+w*h,w*h4); // read U
				memcpy(pv->imageData,buffer->data+w*h+w*h4,w*h4); // read v

				cvResize(pu, pu_big, CV_INTER_NN);
				cvResize(pv, pv_big, CV_INTER_NN);  //CV_INTER_LINEAR looks better but it's slower
				cvMerge(py, pu_big, pv_big, NULL, image);

				cvCvtColor(image,dstImage,CV_YCrCb2RGB);	// convert in RGB color space (slow)
				//conver RGB array image to Mat
				cameraFeed=cvarrToMat(dstImage);

			}
			else
			{
				// for face reco, we just keep gray channel, py
				gray=cvarrToMat(py);
				//cvShowImage("camcvWin", py); // display only gray channel
			}
			//-------------CUSTOM CODE-----------------------
			//ALWAYS CREATE NAMED-WINDOW BEFORE
			//namedWindow(originalImage);
			//namedWindow("Thresh");

			//convert to HSV color space
			cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);

			//filter HSV image between values and store filtered image to threshold matrix
			inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),thresh);

			//perform morphological operations on thresholded image to eliminate noise
			//and emphasize the filtered object(s)
			MorphologyOperations(thresh);

			//Display camera feed and filtered binary image
			//imshow(originalImage,cameraFeed);
			//imshow("Thresh", thresh);

			//Find the contours and draw them
			Contour(thresh, cameraFeed, false, true);

			//Wait for 30ms to allow processing (the image will not show without this)
			waitKey(30);
			nCount++;		// count frames displayed
			//---------------------------------------------------
			mmal_buffer_header_mem_unlock(buffer);
		}
		else vcos_log_error("buffer null");

	}
	else
	{
		vcos_log_error("Received a encoder buffer callback with no state");
	}

	// release buffer back to the pool
	mmal_buffer_header_release(buffer);

	// and send one back to the port (if still open)
	if (port->is_enabled)
	{
		MMAL_STATUS_T status;

		new_buffer = mmal_queue_get(pData->pstate->video_pool->queue);

		if (new_buffer)
			status = mmal_port_send_buffer(port, new_buffer);

		if (!new_buffer || status != MMAL_SUCCESS)
			vcos_log_error("Unable to return a buffer to the encoder port");
	}

}


/**
* Create the camera component, set up its ports
*
* @param state Pointer to state control struct
*
* @return 0 if failed, pointer to component if successful
*
*/
static MMAL_COMPONENT_T *create_camera_component(RASPIVID_STATE *state)
{
	MMAL_COMPONENT_T *camera = 0;
	MMAL_ES_FORMAT_T *format;
	MMAL_PORT_T *preview_port = NULL, *video_port = NULL, *still_port = NULL;
	MMAL_STATUS_T status;

	/* Create the component */
	status = mmal_component_create(MMAL_COMPONENT_DEFAULT_CAMERA, &camera);

	if (status != MMAL_SUCCESS)
	{
		vcos_log_error("Failed to create camera component");
		goto error;
	}

	if (!camera->output_num)
	{
		vcos_log_error("Camera doesn't have output ports");
		goto error;
	}

	video_port = camera->output[MMAL_CAMERA_VIDEO_PORT];
	still_port = camera->output[MMAL_CAMERA_CAPTURE_PORT];

	//  set up the camera configuration
	{
		MMAL_PARAMETER_CAMERA_CONFIG_T cam_config =
		{
			{ MMAL_PARAMETER_CAMERA_CONFIG, sizeof(cam_config) },
			cam_config.max_stills_w = state->width,
			cam_config.max_stills_h = state->height,
			cam_config.stills_yuv422 = 0,
			cam_config.one_shot_stills = 0,
			cam_config.max_preview_video_w = state->width,
			cam_config.max_preview_video_h = state->height,
			cam_config.num_preview_video_frames = 3,
			cam_config.stills_capture_circular_buffer_height = 0,
			cam_config.fast_preview_resume = 0,
			cam_config.use_stc_timestamp = MMAL_PARAM_TIMESTAMP_MODE_RESET_STC
		};
		mmal_port_parameter_set(camera->control, &cam_config.hdr);
	}
	// Set the encode format on the video  port

	format = video_port->format;
	format->encoding_variant = MMAL_ENCODING_I420;
	format->encoding = MMAL_ENCODING_I420;
	format->es->video.width = state->width;
	format->es->video.height = state->height;
	format->es->video.crop.x = 0;
	format->es->video.crop.y = 0;
	format->es->video.crop.width = state->width;
	format->es->video.crop.height = state->height;
	format->es->video.frame_rate.num = state->framerate;
	format->es->video.frame_rate.den = VIDEO_FRAME_RATE_DEN;

	status = mmal_port_format_commit(video_port);
	if (status)
	{
		vcos_log_error("camera video format couldn't be set");
		goto error;
	}

	// PR : plug the callback to the video port
	status = mmal_port_enable(video_port, video_buffer_callback);
	if (status)
	{
		vcos_log_error("camera video callback2 error");
		goto error;
	}

	// Ensure there are enough buffers to avoid dropping frames
	if (video_port->buffer_num < VIDEO_OUTPUT_BUFFERS_NUM)
		video_port->buffer_num = VIDEO_OUTPUT_BUFFERS_NUM;


	// Set the encode format on the still  port
	format = still_port->format;
	format->encoding = MMAL_ENCODING_OPAQUE;
	format->encoding_variant = MMAL_ENCODING_I420;
	format->es->video.width = state->width;
	format->es->video.height = state->height;
	format->es->video.crop.x = 0;
	format->es->video.crop.y = 0;
	format->es->video.crop.width = state->width;
	format->es->video.crop.height = state->height;
	format->es->video.frame_rate.num = 1;
	format->es->video.frame_rate.den = 1;

	status = mmal_port_format_commit(still_port);
	if (status)
	{
		vcos_log_error("camera still format couldn't be set");
		goto error;
	}


	//PR : create pool of message on video port
	MMAL_POOL_T *pool;
	video_port->buffer_size = video_port->buffer_size_recommended;
	video_port->buffer_num = video_port->buffer_num_recommended;
	pool = mmal_port_pool_create(video_port, video_port->buffer_num, video_port->buffer_size);
	if (!pool)
	{
		vcos_log_error("Failed to create buffer header pool for video output port");
	}
	state->video_pool = pool;

	/* Ensure there are enough buffers to avoid dropping frames */
	if (still_port->buffer_num < VIDEO_OUTPUT_BUFFERS_NUM)
		still_port->buffer_num = VIDEO_OUTPUT_BUFFERS_NUM;

	/* Enable component */
	status = mmal_component_enable(camera);

	if (status)
	{
		vcos_log_error("camera component couldn't be enabled");
		goto error;
	}

	raspicamcontrol_set_all_parameters(camera, &state->camera_parameters);

	state->camera_component = camera;

	return camera;

error:

	if (camera)
		mmal_component_destroy(camera);

	return 0;
}

/**
* Destroy the camera component
*
* @param state Pointer to state control struct
*
*/
static void destroy_camera_component(RASPIVID_STATE *state)
{
	if (state->camera_component)
	{
		mmal_component_destroy(state->camera_component);
		state->camera_component = NULL;
	}
}


/**
* Destroy the encoder component
*
* @param state Pointer to state control struct
*
*/
static void destroy_encoder_component(RASPIVID_STATE *state)
{
	// Get rid of any port buffers first
	if (state->video_pool)
	{
		mmal_port_pool_destroy(state->encoder_component->output[0], state->video_pool);
	}

	if (state->encoder_component)
	{
		mmal_component_destroy(state->encoder_component);
		state->encoder_component = NULL;
	}
}

/**
* Connect two specific ports together
*
* @param output_port Pointer the output port
* @param input_port Pointer the input port
* @param Pointer to a mmal connection pointer, reassigned if function successful
* @return Returns a MMAL_STATUS_T giving result of operation
*
*/
static MMAL_STATUS_T connect_ports(MMAL_PORT_T *output_port, MMAL_PORT_T *input_port, MMAL_CONNECTION_T **connection)
{
	MMAL_STATUS_T status;

	status =  mmal_connection_create(connection, output_port, input_port, MMAL_CONNECTION_FLAG_TUNNELLING | MMAL_CONNECTION_FLAG_ALLOCATION_ON_INPUT);

	if (status == MMAL_SUCCESS)
	{
		status =  mmal_connection_enable(*connection);
		if (status != MMAL_SUCCESS)
			mmal_connection_destroy(*connection);
	}

	return status;
}

/**
* Checks if specified port is valid and enabled, then disables it
*
* @param port  Pointer the port
*
*/
static void check_disable_port(MMAL_PORT_T *port)
{
	if (port && port->is_enabled)
		mmal_port_disable(port);
}

/**
* Handler for sigint signals
*
* @param signal_number ID of incoming signal.
*
*/
static void signal_handler(int signal_number)
{
	// Going to abort on all signals
	vcos_log_error("Aborting program\n");

	// TODO : Need to close any open stuff...how?
	exit(255);
}

//This function will determine if the ESC key has been pressed
void *ESCPressed(void* arg)
{
	printf("Key thread started.\n");
	int c;
	while(1)
	{
		c = getchar();
		if(c == 27)
		{
			//raise end program flag
			pthread_mutex_lock(&keyLock);
			keyData.flag = 1;
			pthread_mutex_unlock(&keyLock);
		}
	}
}

/**
* Entry Point of the program
*/
int main(int argc, const char **argv)
{
	pthread_mutex_lock(&keyLock);
	keyData.flag = 0;
	pthread_mutex_unlock(&keyLock);
	system("/bin/stty -icanon");	//change behaviour of command line
	int rc;
	//Create a thread to determine key press
	rc = pthread_create(&keyThread, NULL, ESCPressed, NULL);
	if(rc != 0)
	{
		printf("Error creating key thread.\n");
		exit(EXIT_FAILURE);
	}
	//create client networking thread
	rc = pthread_create(&clientThread, NULL, Socket, NULL);
	if(rc != 0)
	{
		printf("error creating client thread\n");
		exit(EXIT_FAILURE);
	}
	//create OCR operation thread
	#ifdef OCR
	rc = pthread_create(&ocrThread, NULL, ImageData, NULL);
	if (rc != 0)
	{
		printf("Error creating client thread");
		exit(EXIT_FAILURE);
	}
	#endif
	//create slider bars for HSV filtering
	//CreateTrackbars();
	//-----------RASPIVID CODE------------
	// Our main data storage vessel..
	RASPIVID_STATE state;

	MMAL_STATUS_T status;// = -1;
	MMAL_PORT_T *camera_video_port = NULL;
	MMAL_PORT_T *camera_still_port = NULL;
	MMAL_PORT_T *preview_input_port = NULL;
	MMAL_PORT_T *encoder_input_port = NULL;
	MMAL_PORT_T *encoder_output_port = NULL;

	time_t timer_begin,timer_end;
	double secondsElapsed;

	bcm_host_init();
	signal(SIGINT, signal_handler);

	// read default status
	default_status(&state);

	// init windows and  OpenCV Stuff
	int w=state.width;
	int h=state.height;

	dstImage = cvCreateImage(cvSize(w,h), IPL_DEPTH_8U, 3);
	py = cvCreateImage(cvSize(w,h), IPL_DEPTH_8U, 1);		// Y component of YUV I420 frame
	pu = cvCreateImage(cvSize(w/2,h/2), IPL_DEPTH_8U, 1);	// U component of YUV I420 frame
	pv = cvCreateImage(cvSize(w/2,h/2), IPL_DEPTH_8U, 1);	// V component of YUV I420 frame
	pu_big = cvCreateImage(cvSize(w,h), IPL_DEPTH_8U, 1);
	pv_big = cvCreateImage(cvSize(w,h), IPL_DEPTH_8U, 1);
	image = cvCreateImage(cvSize(w,h), IPL_DEPTH_8U, 3);	// final picture to display

	// create camera
	if (!create_camera_component(&state))
	{
		vcos_log_error("%s: Failed to create camera component", __func__);
	}
	else if ((status=raspipreview_create(&state.preview_parameters))!=MMAL_SUCCESS)
	{
		vcos_log_error("%s: Failed to create preview component", __func__);
		destroy_camera_component(&state);
	}
	else
	{
		//Run this code if Camera Component created successfully
		PORT_USERDATA callback_data;

		camera_video_port   = state.camera_component->output[MMAL_CAMERA_VIDEO_PORT];
		camera_still_port   = state.camera_component->output[MMAL_CAMERA_CAPTURE_PORT];

		VCOS_STATUS_T vcos_status;

		callback_data.pstate = &state;

		vcos_status = vcos_semaphore_create(&callback_data.complete_semaphore, "RaspiStill-sem", 0);
		vcos_assert(vcos_status == VCOS_SUCCESS);

		// assign data to use for callback
		camera_video_port->userdata = (struct MMAL_PORT_USERDATA_T *)&callback_data;

		// init timer
		time(&timer_begin);


		// start capture
		if (mmal_port_parameter_set_boolean(camera_video_port, MMAL_PARAMETER_CAPTURE, 1) != MMAL_SUCCESS)
		{
			return 0;
		}

		// Send all the buffers to the video port

		int num = mmal_queue_length(state.video_pool->queue);
		int q;
		for (q=0;q<num;q++)
		{
			MMAL_BUFFER_HEADER_T *buffer = mmal_queue_get(state.video_pool->queue);

			if (!buffer)
				vcos_log_error("Unable to get a required buffer %d from pool queue", q);

			if (mmal_port_send_buffer(camera_video_port, buffer)!= MMAL_SUCCESS)
				vcos_log_error("Unable to send a buffer to encoder output port (%d)", q);
		}
		while(keyData.flag != 1)
		{//wait until flag is raised
		}
		//mmal_status_to_int(status);


		// Disable all our ports that are not handled by connections
		check_disable_port(camera_still_port);
		if (state.camera_component)
			mmal_component_disable(state.camera_component);

		//destroy_encoder_component(&state);
		raspipreview_destroy(&state.preview_parameters);
		destroy_camera_component(&state);

	}
	if (status != 0)
		raspicamcontrol_check_configuration(128);

	time(&timer_end);  /* get current time; same as: timer = time(NULL)  */
	cvReleaseImage(&dstImage);
	cvReleaseImage(&pu);
	cvReleaseImage(&pv);
	cvReleaseImage(&py);
	cvReleaseImage(&pu_big);
	cvReleaseImage(&pv_big);

	secondsElapsed = difftime(timer_end,timer_begin);

	printf ("%.f seconds for %d frames : FPS = %f\n", secondsElapsed,nCount,(float)((float)(nCount)/secondsElapsed));
	//since some threads are never terminated, we do not explicitly call pthread_exit in main function
	system("/bin/stty icanon");		//enable canonical input
	return 0;
}

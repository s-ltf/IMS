
#include "CamCV.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
extern "C"
{
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
}

//OPENCV stuff
#include <cv.h>
#include <highgui.h>

//Function Declerations
void Detect(Mat cameraInput);

CamCV cam;

int sizeofmat(Mat *mat)
{
	return mat->rows * mat->step;
}

int main(int argc, const char **argv)
{
	int result = cam.Initialize();
	/*
	SHARED MEMORY STUFF
	int shmid;
	key_t key = 5678;
	uchar *vdisp;
	Mat *s = Mat(240,320,CV_8U);
	const size_t vdispsize = sizeofmat(s);
	//Locate the segment
	if((shmid = shmget(key,vdispsize,0666))<0)
	{
		perror("shmget");
		exit(1);
	{
	if((vdisp = (uchar *)shmat(shmid,NULL,0))==(uchar *)-1)
	{
		perror("shmat");
		exit(1);
	}
	s->data.ptr = vdisp;
	*/
	return result;
}
void DataToSend(string location, string data)
{
	
}

	/*2 states:
	 * 1- monitor/adjust speed, yaw, displacement, dist_to_walls
	 *  - Mapping is complete under conditions:
	 *  	a) x dispalcement is 2 meters or less away from the start point	
	 *    	b) y dispalcement is 5 meters or less away from the start point
	 *  	c) The estimates above were chosen according to the minimum width of the hallway in ITB 2nd floor
	 *		d) It is assumed that x is always the width of hallway being mapped (at starting point)
	 *		e) It is assumed that +y is always the direction where the carrier initially moves (at starting point)
	 *		TODO: d) and e) may be incorrect. See how MMU is implementing.
	 *
	 *	- Mapping Algorithm is divided to two parts: Primary (main hallway) and Secondary (sub-hallways) Mapping
	 *  
	 *  -Four possible scenarios:
	 *  	1- Car starts at a U-shaped hallway
	 *  		|--------
	 *  		|	*->
	 *  		|--------
	 *  		Best case scenario.. Car will eventually go back to starting point.
	 *  
	 *  	2- car starts in middle of hallway
	 *  		--------
	 *  			*->
	 *  		--------
	 *  		Use boolean is_wall_behind_starting_point to continue mapping behind starting point once reached.
	 *  		it is set to 0 once triggered so we dont map again where we started
	 *  		UPDATE: line above is modified.. sub_hallway_t struct is used to keep track of sub-hallways. This
	 *  				scenario is considered a subhallway.. see init_carrier_position()
	 *  	
	 *  	3- car starts at an island (island is on right side of the car)
	 *			
	 *				|---------------------------|			|---------------|
	 *				|			*->				|			|				|
	 *				|    	|-----------|		|			|	|-------|	|
	 *				|	 	|			|		|			|	|		|	|
	 *			----|		|-----------|		|-----------|	|-------|	|------
	 *			(B)						   (A)
	 *			-------------------------------------------------------------------
 	 *			According to algorithm, the car continues moving straight to point A.
	 *			Once wall is detected ahead, it turns right and follows the wall on the right. According
	 *			to algorithm, we take note of the missing wall on the left of the car (as a sub-hallway)
	 *			at point (A) before turning. The car continues to point B then back to starting point.
	 *			Then we travel to missed "sub-hallway" (at left of A) and finish mapping.
	 *  
	 *  	 4- car starts at an island (island is on left side of the car)
	 *				|-----------------------|
	 *				|	      <-*			|
	 *				|    |-----------|		|
	 *				|	 |			 |		|
	 *			----|	 |xxxxxxxxxxx|		-------------
	 *				  (A)
	 *			-----------------------------------------
	 *  		Mapping is done normally but x walls will be missed. When point (A) reached, we take
	 *  		note of missing wall on left which is discarded when the car comes back from the left hall.
	 *  		But according to scenario 2, at the starting point, the back of the car is 
	 *  		marked as a sub-hallway, so the island will end up in the correct side of the car and will be mapped.
	 *  		
	 *  
	 *  //  $TODO case 4 above
	 *  
	 *  - If starting point reached (i.e. hallway sweeped):
	 *  	a) check if hallway exists behind startpoint that was not mapped
	 *  	b) check if sub-hallways were mapped. If not go back and map any islands
	 *  	c) Then mapping complete
	 *  - If starting point is at an island with the right side of the carrier pointing towards the island
	 *  - 
	 * 2- check surrounding walls/sub-hallways and plan accordingly
	 * 
	 *	- Walls are detected 1 meter ahead
	 *	- If car about to turn right and wall is not detected 1 meter 
	 *		ahead, then mark the hall ahead and to the left (if any) as sub-hallway
	 */

#include <stdint.h>
#include <stdio.h>

#include "planner.h"
#include "shmemory.h"
#include <math.h>
#include <stdlib.h>

const uint8_t min_dist_right_wall = 1;		//minimum distance which POI operates //$TODO: get value from POI
const uint8_t max_dist_right_wall = 20;		//maximum distance which POI operates //$TODO: get value from POI

#define M_PI 3.14159265358979323846264338327

#define MIN_DIST_POI 2

#define hallway_width (current_dist_to_wall.dist_right + current_dist_to_wall.dist_left)
#define OFFSET_DIST 3	//Error in sensors accuracy is mitigated by 3meters
						//ex: if sensor module reads current displacement as 5, the Planner sees it as 5 +/- OFFSET_DIST
						//$TODO: check accuracy of the distance sensors
						//$TODO: should there be separate vars for displacement and distance sensors
#define OFFSET_DISP 3
#define OFFSET_YAW 45

#define MAX_COUNT       5
#define NUM_STATES      8
#define maximum(a,b)     ((a) > (b) ? (a) : (b))

int8_t check_current_subhallways (int8_t angle, bool isTurningAfter);
int8_t add_subhallway(int8_t angle, bool isTurningAfter);


///*** Global Variables

int front=0, left=0, right=0, yaw=0, dist = 0;

int prev_wall_left = 0, prev_wall_right = 0, prev_wall_front = 0, prev_yaw = 0, prev_dist = 0;
int prev_is_wall_left = 0, prev_is_wall_right = 0, prev_is_wall_front = 0;

int count = 0; // read 5 times from wall sensors and average them

int prevDist=0; //used for tracking right and left wall sensor
int tog = 0;

int delay=0;
int count_front = 0, count_left = 0, count_right = 0;

int current_state = 0;
int state[NUM_STATES+1]= {[0 ... NUM_STATES] = 0};     //state[0] is the initial 'move forward' state (see Detect_wall_algo)

int turning_flag = -1;	//=0 just finished turning, =1 currently turning, =-1 nothing
int left_flag=0, right_flag=0, front_flag=0, yaw_flag=0, dist_flag=0;

typedef enum {
    FORWARD = 0,
    TURN_LEFT,
    TURN_RIGHT,
	SLOW,
	DRIFT_LEFT,
	DRIFT_RIGHT,
} e_command;

e_command curr_cmd = -1;
e_command prev_cmd = -1;
int is_car_slow = 0;

///##########///
///Variables for Front Threshold
const int FRONT_THRESH = 300;
int front_thresh = 300;

//#define dynamic_threshold
	//Enables the front sensor to detect a wall at different distances depending on the left and right sensor readings

//Track the left and right sensors to calculate the front thresh 
int track_data [MAX_COUNT]= { [0 ... MAX_COUNT-1] = 0};	//each element holds the sum of left and right sensor readings to be averaged
int track_arr_length = MAX_COUNT;
int track_count;		//increment when readings are stable
int track_average = 0;	//average of the sensor readings

///##########///
///Variables for drifting algo
int track_drift [MAX_COUNT]= { [0 ... MAX_COUNT-1] = 0};
int drift_average = 0;
	//"track_arr_length" and "track_count" declared above are used with this track_drift array
float desired_drift_offset = 2;	//how far off the centre of the hallway do we want to the car to be.
														//(=2 -> center, =3 -> one third the width of the hallway with reference to the left wall)
e_command drift_cmd = -1;

int desired_distance = 0;			//the distance to be covered after turning the car for drifting in desired direction (always = cos(yaw))
int init_dist = 0;

///##########///
int temp_sc_front_dist = 0;

int avg_left = 0, avg_right = 0, avg_front = 0;

int track_left [MAX_COUNT]= { [0 ... MAX_COUNT-1] = 0}; 
int track_right [MAX_COUNT]= { [0 ... MAX_COUNT-1] = 0};
int left_average = 0, right_average = 0;
/**************************************************************************************************
/**************************************************************************************************
/**************************************************************************************************
######################################## INIT FUNCTIONS ########################################
*/

void init_planner (){

	current_state = INIT;
	current_sub_state = NONE;
	
	is_wall_left  = 0;
	is_wall_right = 0;
	is_wall_front = 0;
	is_wall_behind_starting_point  = 0;
	
	reference_yaw = 0;
	timerFlag = 0;
	sub_hallways_count = 0;
	
	starting_point_reached = 0;
	
	first_sub_hallway = NULL;
	last_sub_hallway = NULL;
	//mistaken_sub_hallway = NULL;	//delete variable
	
	is_car_turning	= 0;

	reset_velocity();
	reset_dist_to_wall();
}

void main_planner (){
	//TODO: This function must be called in an infinite while loop. Remove the while(true) when integrated with rest of software
	
		getSensors();
        
        ///If all 3 sonar sensors are ready to be read from memory (flag byte for each sensor is 1)
        //If currently turning do not do anything
        //BEWARE between reading all flags as 1 and setting turning_flag to 1 and sending turning command, memory may update again! therefore disabling the turning_flag
        if (left_flag == 1 && right_flag == 1 && front_flag == 1){
            left_flag = right_flag = front_flag = 0;    //Reset flags to 0 so we start updating our globals again
            update_current_dist_to_wall();
			
			int index = check_environment();
			
			if ((state[index] == (MAX_COUNT/2)) && index >= 4 && index <= 7)
			{
				printf ("Entered state change segment, state: %d\n", index);
				temp_sc_front_dist = front/2;
				if(front<500)
					front_thresh = front;
				printf ("temp_front_dist: %d\n", temp_sc_front_dist);
				prev_cmd = curr_cmd;
				curr_cmd = SLOW;
				process_cmd(0);
			}
			if ((state[index] == (MAX_COUNT/2)) && index == 2)
			{
				printf ("Entered pre state 2\n");
				//temp_sc_front_dist = front;
				//if(front<500)
				//front_thresh = max(front-50,100);
				((front-50)<100)? front_thresh = 100 : ((front_thresh = front - 50),state[index] = 0);
				printf ("front thresh: %d\n", front_thresh);
				prev_cmd = curr_cmd;
				curr_cmd = SLOW;
				process_cmd(0);
			}
			//check if incremented stated has reached Possible_State_Change_Count
			//change front threshold to 100
			//go to new function
			//new function:
			//if that new state requires turning then get distance to front and devide by two and store it in global
			//
			
            if (state[index] >= MAX_COUNT){
				printf ("STATE:%d\n", index);
				track_sensors(index);
                Detect_Wall_Turn_Algo(index);
                process_cmd(index);
				check_drift(index);
			}
			return_from_drift(index);
		}
}//main_planner

void reset_velocity(){
	//set global vars: displacement, yaw, speed to zero
	current_velocity.disp_x = 0;
	current_velocity.disp_y = 0;
	current_velocity.yaw    = 0;
	current_velocity.speed  = 0;
}
void reset_dist_to_wall(){
	//set global vars: distance to left and right walls to
	current_dist_to_wall.dist_left = 0;
	current_dist_to_wall.dist_right = 0;
	current_dist_to_wall.dist_front = 0;
}


/**************************************************************************************************
/**************************************************************************************************
/**************************************************************************************************
######################################## Planner Algorithms ########################################
*/
int check_environment(){
     	int i = -1;
        
        //If reached this point, then car just finished turning, now move forward
        if (turning_flag == 1){
            count_right = 0;
			turning_flag = 0;
            
			while (!front_flag)
            {
                getSensors();
            }

            if (front < 300)
			{
				printf ("\t\t00-Finished Turning, Wall ahead.. \n");
				sleep(1);
				turning_flag = -1;
			}
			else
			{
				//Move Forward
				printf ("\n*************************************\n*************************************\n");
				printf ("\t\t00-Finished Turning, stop then move forward\n");
				sleep(1);
				prev_cmd = curr_cmd;
				curr_cmd = FORWARD;
				process_cmd(0);
			}
        }
        
        //After turning, if wall is seen on the right then proceed with normal algorithm and disable turning flag
        //Otherwise move forward and DO NOT DO ANYTHING ELSE until a wall on the right is detected
        if (turning_flag == 0){
			printf ("Turning flag: 0\n");
			
			if (is_wall_right == 1) count_right ++;
			
            //Check if wall on right detected MAX_COUNT times to make sure we moved out of the corner we just turned
			if (count_right >= MAX_COUNT){
				count_right = 0;
                turning_flag = -1;
                i=0;
                while (i <= NUM_STATES){
                    state[i] = 0;
                    i++;
                }
                i = -1;
            }
            else
                return 0;
        }
        
        ///Check if walls exists and increment the corresponding state
        
        if (is_wall_right == 1 && is_wall_front == 0 && is_wall_left  == 1){            //case 0 - move Forward
            state[0]++;
            i = 0;
		}
		else if (is_wall_right == 1 && is_wall_front == 0 && is_wall_left  == 0){    //case 1 - move Forward, mark sub-hallway left
            state[1]++;
            i = 1;
		}
		else if (is_wall_right == 1 && is_wall_front == 1 && is_wall_left  == 1){    //case 2 - Turn Left, track yaw
            state[2]++;
            i = 2;
		}
        else if (is_wall_right == 1 && is_wall_front == 1 && is_wall_left  == 0){    //case 3 - Turn Left
            state[3]++;
            i = 3;
		}
		else if (is_wall_right == 0 && is_wall_front == 1 && is_wall_left  == 0){    //case 4 - Turn Right, mark sub-hallway left, track yaw
            state[4]++;
            i = 4;
		}
		else if (is_wall_right == 0 && is_wall_front == 1 && is_wall_left  == 1){    //case 5 - Turn Right, track yaw
            state[5]++;
            i = 5;
		}
		else if (is_wall_right == 0 && is_wall_front == 0 && is_wall_left  == 0){    //case 6 - Turn Right, mark subhallway left and ahead, track yaw
            state[6]++;
            i = 6;
		}
		else if (is_wall_right == 0 && is_wall_front == 0 && is_wall_left  == 1){   //case 7 - Turn Right, mark subhallway ahead, track yaw
            state[7]++;
            i = 7;
		}

        ///If a state has been incremented
        if (i >= 0){
            ///If needed, ceil the incremented state to MAX_COUNT
            if (state[i] > MAX_COUNT)
                state[i] = MAX_COUNT;
        
            ///Decrement the other states by 1
            int k;
            for(k = 0; k < NUM_STATES; k++){
                if (k != i){
                     state[k]--;
                     state[k] = maximum(state[k],0);
                }
            }
        }
        else{
            printf ("Error - Check_Environment() called but no state updated");
            return 0;
        }
     
    return i;   //Return the state that was incremented
}


void getSensors (){
	int flag = 0;
	int  sensor = 0;

	//Read sensors from memory according to flag
	//(flag=1 means the same sensor value was read from memory and has not been updated since)
	flag = read_sensors_from_memory (&sensor, "left");
	if (flag == 1){			left_flag = 1;			prev_wall_left = left;				left = sensor;}
	else if (flag != 0) printf ("Error Planner - invalid left_flag: %s\n", flag);
	
	flag = read_sensors_from_memory (&sensor, "right");
	if (flag == 1){			right_flag = 1;		prev_wall_right = right;			right = sensor;}
	else if (flag != 0) printf ("Error Planner - invalid right_flag: %s\n", flag);

	flag = read_sensors_from_memory (&sensor, "front");
	if (flag == 1){			front_flag = 1;		prev_wall_front = front;			front = sensor;}
	else if (flag != 0) printf ("Error Planner - invalid front_flag: %s\n", flag);
	
    /*****************************CHECK IF SAME IF STATEMENT APPLIES TO YAW AND DIST**************/
	flag = read_sensors_from_memory (&sensor, "yaw");
	if (flag == 1){			yaw_flag = 1;		prev_yaw = yaw;						yaw = sensor;}
	else if (flag == 0) 	yaw_flag = 0;
	else if (flag != 0) printf ("Error Planner - invalid yaw_flag: %s\n", flag);
	
	flag = read_sensors_from_memory (&sensor, "dist");
	if (flag == 1){			dist_flag = 1;		prev_dist = dist;						dist = sensor;}
	else if (flag == 0)	dist_flag = 0;
	else if (flag != 0) printf ("Error Planner - invalid dist_flag: %s\n", flag);
	
//	printf ("\t\tleft:  %d\tright: %d\tfront: %d\tyaw: %d\tdist: %d\n", left, right, front, yaw, dist, current_state);
}


void Detect_Wall_Turn_Algo (int state_num){

		/**Mapping algorithm (see Table-P3.0)**/
        
        //if turning OR finished turning but did not reach the beginning of the hallway then do nothing
        if (turning_flag == 0 || turning_flag == 1){
            printf ("Error - Reached Detect_Wall_Turn_Algo but should not be here.Debug:\n %dturning_flag: %d", turning_flag);
            return;
        }
        
        printf ("\n*************************************\n*************************************\n");
        
        prev_cmd = curr_cmd;
        
		if (state_num == 0 ){             //case 0 - move Forward     (right=1, front=0, left=1)
				printf ("\t\t%d-move forward\n", state_num);
                curr_cmd = FORWARD;
				current_state = state_num;
		}	
		else if (state_num == 1 ){         //case 1 - move Forward, mark sub-hallway left      (right=1, front=0, left=0)
				printf ("\t\t%d-move forward (mark sub-hallway left)\n", state_num);
				curr_cmd = FORWARD;
				current_state = state_num;
		}
		else if (state_num == 2 ){         //case 2 - Turn Left, track yaw      (right=1, front=1, left=1)
				printf ("\t\t%d-turn left\n", state_num);
				curr_cmd = TURN_LEFT;
				current_state = state_num;
				turning_flag = 1;
		}
        else if (state_num == 3 ){	    //case 3 - Turn Left, track yaw      (right=1, front=1, left=1)
				printf ("\t\t%d-turn left\n", state_num);
				curr_cmd = TURN_LEFT;
				current_state = state_num;
				turning_flag = 1;
		}
        else if (state_num == 4 ){         //case 4 - Turn Right, mark sub-hallway left, track yaw      (right=0, front=1, left=0)
				printf ("\t\t%d-turn right (mark sub left)\n", state_num);
				curr_cmd = TURN_RIGHT;
				current_state = state_num;
				turning_flag = 1;
		}
        else if (state_num == 5 ){         //case 5 - Turn Right, track yaw      (right=0, front=1, left=1)
				printf ("\t\t%d-turn right\n", state_num);
				curr_cmd = TURN_RIGHT;
				current_state = state_num;
				turning_flag = 1;
		}
        else if (state_num == 6 ){         //case 6 - Turn Right, mark subhallway left and ahead, track yaw      (right=0, front=0, left=0)
				printf ("\t\t%d-turn right (mark sub left and ahead)\n", state_num);
				curr_cmd = TURN_RIGHT;
				current_state = state_num;
				turning_flag = 1;
		}
        else if (state_num == 7 ){         //case 7 - Turn Right, mark subhallway ahead, track yaw      (right=1, front=0, left=1)
				printf ("\t\t%d-turn right (mark sub ahead)\n", state_num);
				curr_cmd = TURN_RIGHT;				
				current_state = state_num;
				turning_flag = 1;
		}
}

int process_cmd(int index){
    
	printf ("curr_cmd:%d\n", curr_cmd);
	printf ("prev_cmd:%d\n", prev_cmd);
	
    //If the command sent to car last time is not the same as the current command then send it
    if ((curr_cmd != prev_cmd)||(curr_cmd!= FORWARD)){
        if (curr_cmd == FORWARD){
            printf ("*SEND FORWARD COMMAND\n");
			send_cmd (10, 0);
		}
        else if (curr_cmd == TURN_LEFT){
			printf ("*SEND TURN LEFT COMMAND\n");
			
			left_flag = right_flag = front_flag = 0;    //Reset flags to 0
			
			send_cmd(0,0);
			sleep(1);
			send_cmd (0, 270);
			sleep(3);
			}
        else if (curr_cmd == TURN_RIGHT){
            printf ("*SEND TURN RIGHT COMMAND\n");
			
			if ((index==4) || (index==5)){
					//printf ("front: %d, temp_sc_front: %d\n", front, temp_sc_front_dist);
					int i = 0;
					
					while (front >= temp_sc_front_dist){
						getSensors();
					}
					printf ("front: %d, temp_sc_front: %d\n", front, temp_sc_front_dist);
					//printf ("new wall before turning right - is_left: %d, is_right: %d, is_front: %d", left,right,front);
					//printf ("avg_front: %d, temp_sc_front/2: %d\n", avg_front, temp_sc_front_dist/2);
			} 
			
			left_flag = right_flag = front_flag = 0;    //Reset flags to 0
			
			send_cmd(0,0);
			sleep(1);
			send_cmd (0, 90);
			sleep(3);
			}
		else if (curr_cmd == SLOW){
            printf ("SEND SLOW DOWN COMMAND\n");
			send_cmd (01, 0);
			}
		
        else
            return 0;
        return 1;   //succes: command sent
    }
    else
        return 0;
}

void update_current_velocity (){

	//Update global variable
	current_velocity.disp_x = 0;
	current_velocity.disp_y = 0;
	
	current_velocity.yaw  = yaw;
	current_velocity.speed  = 1;
}


void update_current_dist_to_wall (){
	
	int i=0;
	for (i = 0; i < 8 ; i++)
		printf ("state[%d]: %d\t", i, state[i]);
	printf ("\n");
	printf ("\x1B[1;32;40m \t\tleft:  %d\tright: %d\tfront: %d\tyaw: %d\tdist: %d\n\033[0m", left, right, front, yaw, dist, current_state);
  
	/*  //Remember previous
    prev_is_wall_left = is_wall_left;
    prev_is_wall_right = is_wall_right;
    prev_is_wall_front = is_wall_front;
    */
    //Update distance to walls
    if (left >= 0 && left < 300)
        is_wall_left = 1;
    else 
        is_wall_left = 0;
            
    if (right >= 0 && right < 300)
        is_wall_right = 1;
    else
        is_wall_right = 0;
	
#ifdef dynamic_threshold
	printf ("\x1B[1;34;40m \nfront_thresh: %d  \033[0m", front_thresh);
	if ( front >= 0 && front < front_thresh)
		is_wall_front = 1;
#else

	if ( front >= 0 && front < FRONT_THRESH)
        is_wall_front = 1;
#endif    
	else 
        is_wall_front = 0;
}


int send_cmd (int speed, int angle){

	//Note: +ve angle is counter-clockwise
	//	speed must be +- sign plus 2 digits 
	//	angle must be 3 digits in total
	//	last char must be 'n'
	//msg format sent to serial: +ssaaan	(s: speed, a: angle)
	
	char cspeed [5];
	char cangle [5];	
	char msg[12];
	char *p = &msg[0];
	int pos;
	
	if (speed < 0) cspeed[0] = '-';
	else msg[0] = '+';
	pos=1;
	
	///Speed
	if (speed == 0)
		sprintf(cspeed, "+00");
	else if (speed < 0)
		sprintf(cspeed, "-10");
	else if (speed == 10)
		sprintf(cspeed, "+10");
	else if (speed == 1)
		sprintf(cspeed, "+01");
		
	///Angle
	sprintf(cangle,"%03d", angle);
	
	///Speed & Angle
	sprintf(msg, "%s%sn", cspeed, cangle); //Original message (ex: +10275n)
	printf ("move_car: %s\n", msg);
	
	makeMsg (&msg, strlen(msg));

	return write_pln_cmd (msg, 1);
	//'+10245n'
} //move_car()


int check_carrier_orientation (int *tog, int *prevDist){

	//prevDist is the previous right - left
	
	int curr_dist = 0;

	if (*tog == 1){		//check car drift with previous calculation

		curr_dist = right - left;
		
		//if difference between prev and current dist to right wall is greater thatn +-0 and +-5cm
		if (curr_dist > 0 && curr_dist > 5){
			send_cmd(0,355);
			printf ("car drifting right\n");
		}
		else if (curr_dist < 0 && curr_dist < -5){
			send_cmd(0, 5);
			printf ("car drifting left\n");
		}

		*prevDist = current_dist_to_wall.dist_right;
	}
	else if (*tog == 0)
		*prevDist = right - left;

	*tog = (*tog==0)? 1:0;

	return (current_dist_to_wall.dist_right);// - current_dist_to_wall.dist_left);
}


void track_sensors (int index){

	//Run the function only if we are moving straight with walls on left and right(i.e. index = 0)
	if (index != 0){
		#ifdef dynamic_threshold
		#undef dynamic_threshold
		#endif
		track_count = 0;	//the count must increment when index = 0, MAX_COUNT number of times in a row
		return;
	}

#ifndef dynamic_threshold
#define dynamic_threshold
#endif
	
	//If we are in a state where there is no right wall
	if (index >= 4 && index <= 7){
		
	}
	
	//If gathered MAX_COUNT readings then average them out
	if (track_count > MAX_COUNT){
			
			//Front threshold algorithm
			track_count = 0;
			printf ("\x1B[1;34;40m averaging...  \033[0m");
				
			//Find avg of data gathered
			int i, sum_thresh = 0, sum_drift = 0, sum_left = 0, sum_right = 0;
			for (i = 0 ; i < track_arr_length; i++){
					sum_thresh =sum_thresh + track_data[i];
					//printf ("\x1B[1;34;40m \ntrack_data: %d  \033[0m", track_data[i]);
					
					sum_drift =sum_drift + track_drift[i];
					//printf ("\x1B[1;34;40m \t\ttrack_drift: %d  \033[0m\n", track_data[i]);
					
					sum_left =sum_left + track_left[i];
					printf ("\x1B[1;34;40m \t\ttrack_left: %d  \033[0m\n", track_left[i]);
					
					sum_right =sum_right + track_right[i];
					printf ("\x1B[1;34;40m \t\ttrack_right: %d  \033[0m\n", track_right[i]);
			}
			track_average = sum_thresh / track_arr_length;
			front_thresh = track_average;
			printf ("\x1B[1;34;40m \narr_length: %d, front_thresh: %d \n \033[0m", track_arr_length, front_thresh);
			
			drift_average = sum_drift / track_arr_length;
			left_average = sum_left/ track_arr_length;
			right_average = sum_right / track_arr_length;
			printf ("\x1B[1;34;40m \nright_average: %d, left_average: %d \n \033[0m", right_average, left_average);
	}
	else{
			//if count is still within the track array length and there is a wall on our left and right then avg them out
			if (is_wall_left == 1 && is_wall_right == 1){
					track_data[track_count] = (left + right);
					track_drift [track_count] = (right - left );
					track_left [track_count]  = (left);
					track_right [track_count]  = (right);
					track_count ++;
			}
	}
}

void check_drift (int index){		
		
		//If ready to make a drift decision (if we averaged max_count number of data)
		if (turning_flag == -1 && index == 0 && track_count > MAX_COUNT){
		printf ("\x1B[1;34;31mdrift_average:%d, drift_cmd: %d, turning flag: %d, index:%d, track_count:%d, MAX_COUNT:%d\n\033[0m", drift_average, drift_cmd, turning_flag, index, track_count, MAX_COUNT);
			if (drift_cmd != DRIFT_LEFT && drift_cmd != DRIFT_RIGHT){
				if (drift_average > 40){																				//If right sensor reading greater than left, send command to drift left
						printf ("\x1B[1;34;31m##Car drifting left.\n Sending command to drift right!!\n\033[0m");
						
						init_dist = dist;
						left_average = (left_average == 0)? left : left_average;		//when car first starts left_average = 0
						desired_distance = dist + (/*(track_average / 2.0) - left_average*/drift_average/2)/sin(15.0*M_PI/180);
						printf("\x1B[1;34;31mdesired_distance: %d\ncurrent distance:%d\ntrack_average/2=%d\ndrift_avg:%d\nleft_average:%d\n\033[0m", desired_distance, dist, track_average/2, drift_average, left_average);
						
						int err = send_cmd (10, 15);
						if (err != -1)	drift_cmd = DRIFT_RIGHT;	else	printf ("\x1B[1;34;31mError in Drift - could not drift right\n\033[0m");
						printf("\x1B[1;34;31mdrift%d\n\033[0m", drift_cmd);
				}
				else if (drift_average < -40){																		//if drifting to the left, send command to drift right
						printf ("\x1B[1;34;31m##Car drifting right.\n Sending command to drift left!!\n\033[0m");

						init_dist = dist;
						right_average = (right_average == 0)? right : right_average;		//when car first starts right_average = 0
						desired_distance = dist + (/*(track_average / 2.0) - right_average*/-drift_average/2)/sin(15.0*M_PI/180);
						printf("\x1B[1;34;31mdesired_distance: %d\ncurrent distance:%d\ntrack_average/2=%d\ndrift_avg:%d\nright_average:%d\n\033[0m", desired_distance, dist, track_average/2, drift_average, right_average);
						
						int err = send_cmd (10, 345);
						if (err != -1)	drift_cmd = DRIFT_LEFT;		else	printf ("\x1B[1;34;31mError in Drift - could not drift left\n\033[0m");
						printf("\x1B[1;34;31mdrift%d\n\033[0m", drift_cmd);
				}
			}				
		}
}

void return_from_drift(index){
	
	if (turning_flag == -1 && index == 0){
		if (drift_cmd == DRIFT_LEFT){
				//Check if we reached the desired distance and turn back.
				if (dist >= desired_distance){
						drift_cmd = -1;
						printf ("\x1B[1;34;31mDone drifting, drift back to the right.\n\tdist:%d, init_dist: %d\n\033[0m", dist, init_dist);
						int err = send_cmd (10, 15);
						if (err == -1)	printf ("\x1B[1;34;31mError returning from left drift\n\033[0m");
				}
				else
					return;
		}
		else if (drift_cmd == DRIFT_RIGHT){
				//Check if we reached the desired distance and turn back.
				if (dist >= desired_distance){
						drift_cmd = -1;
						printf ("\x1B[1;34;31mDone drifting, drift back to the left.\n\tdist:%d, init_dist: %d\n\033[0m", dist, init_dist);
						int err = send_cmd (10, 345);
						if (err == -1)	printf ("\x1B[1;34;31mError returning from right drift\n\033[0m");
				}
				else
					return;
		}
	}
}

/* Turning Algorithm
	+ if post-turning (turning_flag == 0)and at specific state, have conditional statements that check for 
		expected wall ranges to be seen by sensors after moving forward for a while
	+ once that expected wall ranges are reached/detected, change turning_flag to -1 to completely disable it
	+ turning_flag = 1 means currently turning, =0 means post-turning, =-1 means disabled

$TODO:
	- change range for front sensor to half or one times the sum of left and right sensors (assuming both sensors see walls, otherwise have a default min range ex 150m)
	- 
	
	*/


/*
//If desired offset is at 1/3rd the distance from the left wall
				if (desired_drift_offset == 3){
						if ((drift_average - (track_average / 3.0)) > ((track_average / 3.0) + 10) && drift_cmd != DRIFT_LEFT){						//If drifting to the right, send command to drift left
								printf ("##Car drifting right.\n Sending command to drift left!!\n");
								int err = send_cmd (10, 358);
								if (err != 1){		drift_cmd = DRIFT_LEFT;		printf ("Error in Drift - could not drift left\n");}
						}
						else if ((drift_average - (track_average / 3.0)) > ((track_average / 3.0) - 10) && drift_cmd != DRIFT_RIGHT){			//if drifting to the left, send command to drift right
								printf ("##Car drifting left.\n Sending command to drift right!!\n");
								int err = send_cmd (10, 2);
								if (err != 1)	{		drift_cmd = DRIFT_RIGHT;			printf ("Error in Drift - could not drift right\n");}
						}
						else{															//otherwise we dont need to drift. Return from any current drift
								if (drift_cmd == DRIFT_RIGHT){
										int err = send_cmd (10, 358);
										if (err != 1)		printf ("error returning from right drift\n");
								}
								else if (drift_cmd == DRIFT_LEFT){
										int err = send_cmd (10, 2);
										if (err != 1)		printf ("error returning from left drift\n");
								}
								drift_cmd = -1;
						}
				}*/
				
				//Average out 5 sensor readings
					/*	if (left_flag == 1 && right_flag == 1 && front_flag == 1 && i < MAX_COUNT){
								sum_left = sum_left + left;
								sum_right = sum_right + right;
								sum_front = sum_front + front;
								i++;
						}
						else if (i >= MAX_COUNT){
								i = 0;
								avg_left = sum_left / MAX_COUNT;
								avg_right = sum_right / MAX_COUNT;
								avg_front = sum_front / MAX_COUNT;
						}*/


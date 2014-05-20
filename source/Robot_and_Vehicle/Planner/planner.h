#ifndef PLANNER_H
#define PLANNER_H

#ifndef true
#define true 1
#endif

#ifndef false
#define false 1
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef bool
#define bool char
#endif


/// ********* Function Headers *********
void init_planner ();
void main_planner ();
//void ReadSensors (int fd, int *dist_left, int *dist_right, int *dist_front, int *yaw, int *distance);


//States of the planner
typedef enum {
	INIT=0,			//Runs only at power on.. going through initialization/calibration
	MAIN_MAPPING,		//Mapping main hallway. Does not include turning
	SUB_MAPPING,	//Mapping sub-hallways which may or may not follow the right hand rle
	FOLLOWING_POI,	//Moving back, adjusting orientation, or going to certain coordinates in the map
} state_e;

//Sub-States of the planner
typedef enum {
	NONE,			//Not doing any of the actions below
	TURNING,		//if instructed carrier to turn. Planning cannot be done until carrier stops turning.
	ADJUSTING_ANGLE,//if mapping but too close to left/right wall. Angle is adjusted in this state.
} sub_state_e;

/*
 *The beginning of a sub-hallway junction is recognized as a square. Each side of the square 
 *represents 4 different directions of a hallway and will be marked as:
 *		
 *		-MAPPED: Hallway ahead is mapped and will not be returned to when mapping sub-hallways.
				 At a junction, the front and right of the car is marked as MAPPED.
 *
 *		-FLAGGED: Hallway ahead is flageed for later. Done only when car does not see a wall on its
 *				  left while moving forward OR when turning right but no wall exists ahead.
*/
typedef struct Sub_Hallway {
	int8_t sub_coord_x;					//x-displacement of the sub-hallway
	int8_t sub_coord_y;					//y-displacement of the sub-hallway
	int8_t sub_yaw;						//angle at which the sub-hallway starts (with reference to initial_yaw)
	struct Sub_Hallway *next_sub_hallway;	//pointer to next sub-hallway
	
	int8_t sub_current_yaw;	//the yaw at which sub-hallway was detected.
							//This is used to prevent redunduncies in the mapping algorithm.
	
		//The variable below is used to track the hall ahead of carrier (90deg when a sub-hallway is marked on its left and the carrier continues ahead.
		//This is only applied to case 2/5 (see Table-P3.0) where the hallway marked may be an entrance to an island.
		//This is done to prevent detecting sub-hallways that have already been mapped
		//earlier in the mapping process. This is managed in add_subhallway().
		//Ignoring this variable may cause bugs, i.e. infinite mapping of the same areas.
	bool yaw_ahead;		//0:not mapped, 1: mapped
	
	//sub_hallway_state state;
} sub_hallway_t;

/*The substate can occur while main or sub mapping.
 *Used for having a better control of the car
 *$TODO: may not need struct below
 */
typedef enum {
	MAPPED=0,
	FLAGGED,
} sub_hallway_state;


/**************************************************************************************************
 ########################################## MMU INTERFACE ##########################################
 */
 
//This variable should be updated every second when MMU calls MMU_Get_2DMap ()
typedef struct Velocity {
	int8_t disp_x;		//meters
	int8_t disp_y;		//meters
	int8_t yaw;			//degrees
	int8_t speed;		//meters per second
} Velocity_t;

typedef struct Dist_To_Wall {
	int dist_left;
	int dist_right;
	int dist_front;
	int dist_back;
} dist_to_wall_t;

/**************************************************************************************************
 ########################################## POI INTERFACE ##########################################
 */
 
typedef enum {
	CONTINUE=0,
	//CONTINUE_LEFT,
	//CONTINUE_RIGHT,
	REVERSE,
	//REVERSE_LEFT,
	//REVERSE_RIGHT
	PAUSE,
	SLOWDOWN,
	TURN,
} POI_Command;


/**************************************************************************************************
 ########################################## CARRIER INTERFACE ##########################################
 */
/*#define		SPEED_ZERO		0
#define		SPEED_LOW		16
#define		SPEED_HIGH		50		//cm/s
 */
typedef enum {
	ZERO	=0,
//	LOW		=2,	// cm/s
	MEDIUM	=5,
	HIGH	=10,
} Speed_Level;

typedef struct Packet_To_Carrier {
	int8_t 			angle_change;	 //+ve angle means clock-wise
	Speed_Level		speed;			 //Change in speed
	int8_t 			dist_left_right; //+ve means shift right and -ve # means shift left
} Packet_To_Carrier_t;

/**************************************************************************************************
 ########################################## GLOBAL VARIABLES ##########################################
 */

/**Global Variables**/
Velocity_t current_velocity;
dist_to_wall_t current_dist_to_wall;
int16_t reference_yaw;		//This yaw is set once after turning on and calibrating the system
							//the reference line from which the angle change will be measured????????????
							//angle at which the carrier is pointing relative to starting point
	
char is_car_turning;	//0->car not turning; 1->car turning;
//TODO: LET CARRIER USE THIS VARIABLE WHEN TURNING

char is_wall_left;		//0: no wall on left. 1: wall exists on left
char is_wall_right;		//0: no wall on right. 1: wall exists on right
char is_wall_front;		//0: no wall on front. 1: wall exists on front
char is_wall_behind_starting_point;		//0: no wall on back. 1: wall exists on back
//4 variables above are set true if current min distance to wall is detected (set by MIN_DIST_POI)

char timerFlag; //1:one second timer interrupt

/**sub-hallway variables**/
int8_t sub_hallways_count;	//number of non-mapped sub-hallways. 0: no sub-hallways exist
sub_hallway_t *first_sub_hallway;
sub_hallway_t *last_sub_hallway;
//sub_hallway_t *mistaken_sub_hallway; //this is a list of sub-hallways that have been already mapped but may
									 //be detected later in the mapping process as an unmapped sub-hallway.
									 //This is managed in add_subhallway().
/**---------------------**/
 
char starting_point_reached; //0:never reached; 1:reached.  starting point is reached THEN if sub_hallways_count is checked. 0: MAPPING FIN, otherwise finish sub-hallway mapping then fin

state_e car_current_state;
sub_state_e current_sub_state;

#endif

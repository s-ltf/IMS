#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>		//For shared mem
#include <sys/shm.h>	//For shared mem
#include <unistd.h>   // for usleep()
#include <getopt.h>
//#include "arduino-serial-lib.h"
#include <stdint.h>
//#include <malloc.h>

//#include <usb_serial.h>
//
// arduino-serial-lib -- simple library for reading/writing serial ports
//
// 2006-2013, Tod E. Kurt, http://todbot.com/blog/
//

//#include "arduino-serial-lib.h"

#include <stdio.h>    // Standard input/output definitions 
#include <unistd.h>   // UNIX standard function definitions 
#include <fcntl.h>    // File control definitions 
#include <errno.h>    // Error number definitions 
#include <termios.h>  // POSIX terminal control definitions 
#include <string.h>   // String function definitions 
#include <sys/ioctl.h>	

#include "shmemory.h"

#define		MEM_SIZE	200		//100 //should be in bytes

char *shared_memory_address;
const int buf_max = 256;
int seg_len = 0;		//Length in bytes of each segment in memory (per sensor) including \n character
int fd;
/*************************************************************************************/
/*************************************************************************************/

void init_sharedmem (int *shmid){

		//char *shared_memory_address;

		// The memory key must be the same as the one used in the comm.py module
		key_t key = 44298;

		// OPEN shared memory. Do not create one. comm.py must have one already created
		if ((*shmid = shmget(key, MEM_SIZE, 0/*SHM_REMAP/*IPC_EXCL IPC_CREAT | 0666*//*S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | IPC_CREAT*/)) < 0)
		{
			printf("Error getting shared memory id: %d\n", errno);
			exit(1);
		}
		// Attached shared memory
		if ((shared_memory_address = shmat(*shmid, NULL, 0)) == (char *) -1)
		{
			printf("Error attaching shared memory id, error: %d\n", errno);
			exit(1);
		}

		//In python, when reading from a part of memory that was not written to before, it is discarded.
		//So it is necessary to write anything to memory to avoid buggy memory reading.
		
		//First segment in memory contains the size of the segments in bytes (each segment is separated by a \n character
		char *buf = malloc (sizeof(char)*buf_max);	//buf is initially size 0. Do not use strlen(buf) until something is copied to it or points to something..
		memset(buf,0,buf_max);
		memcpy(buf, shared_memory_address, buf_max);
		
		//Get index of first \n character. The segment before it contains the length of each segment
		char *e = strchr(buf, '\n');
		int a = (int)(e - buf);
		
		//Rewrite the first segment without the \n char (leading zeros still exist)
		char *newBuf = malloc (sizeof(char)*a);
		memcpy(newBuf, buf, a);
		
		//Remove leading zeros
		char *p = newBuf;
		while (*p && *p == '0') p++;
		sprintf (newBuf, "%s", p);
		seg_len =atoi(newBuf);
		
		printf ("shmemory module attached to memory SUCCESSFUL!\nMemory Segment Length: %d\n", seg_len);
		
		//printf ("planner, first segment:\n%s-------\n", buf);
		//printf (" newBuf after%s\n", newBuf);
		
		free (buf);
		free(newBuf);
	/*	
		///DEBUGGING: reading and writing to memory
		char *tmp= malloc (sizeof(char)*buf_max);
		read_mem_string(tmp, 0, 0);
		
		char *b = "1000000bl\n";
		write_mem_cmd(b,1);
		
		read_mem_string(tmp, 0, 0);
		
		free(tmp);*/
	
	/*
		///DEBUGGING: sending planner command to memory
		move_car (10, 275);
		
		char *tmp= malloc (sizeof(char)*buf_max);
		read_mem_string(tmp);
		free(tmp);
		*/
} //init_sharedmem()

void makeMsg(char *msg, int length){
	//The command to be stored in memory must be constructed here because 
	// we know the exact memory segment size in this file not in planner.c

	if (length > seg_len){printf ("Error constructing command - command length too long!\n"); return "-1";}
	
	char *tmpMsg [seg_len];
	sprintf (tmpMsg, "%s", msg);

	sprintf(msg, "%s%s\n", "10", tmpMsg); 		//structure: flag bit '1', speed, angle, 'n', '\n'  (ex: 10+10275n\n)
																			//structure is fixed so will always be adding "10" at the beginning
	//printf("command in memory: %s\n", msg);
	//printf("strlen: %d\n", strlen(msg));
	/*
	char *p = newMsg;
	printf("chars: %c\n", *p	);
	printf("chars: %c\n", *(p+1));
	printf("chars: %c\n", *(p+2));
	printf("chars: %c\n", *(p+3));
	printf("chars: %c\n", *(p+4));
	printf("chars: %c\n", *(p+5));
	printf("chars: %c\n", *(p+6));
	printf("chars: %c\n", *(p+7));
	printf("chars: %c\n", *(p+8));
	printf("chars: %c\n", *(p+9));
	printf("chars: %c\n", *(p+10));*/
}

int write_pln_cmd (char * msg, int pos){

	//memory structure for sending carrier commands: "10+10275n\n" ... first byte is flag, second byte is leading zero
	//Structure for carrier commands: "+10275n"
	
	//First check if flag digit is 0. If so then comm module sent previous cmd. Proceed with new cmd.
	char *buf = malloc (sizeof(char)*buf_max);
	read_mem_string(buf, 1, 1);
	
	//printf ("isValid: %d\n", isValid(buf));
	if (isValid(buf) == 1){
		printf ("Pln cmd mem write error - previous planner command not sent yet!\n");
		return -1;
	}
	
	printf("memory: %s\n", msg);
	
	//Planner should only be writing commands to Carrier (ie only using the second segment in memory)
	if (pos != 1){ printf ("Memory Write Error - should only be writing to 2nd segment.. "); return -1;}
	if (msg[strlen(msg)-1] != '\n'){printf ("Memory Write Error - String does not end with \n"); return -1;}
	
	memcpy(shared_memory_address+(pos*seg_len), msg, strlen(msg));
	
	return 1;
}//write_pln_cmd()

int write_mem_string (char * msg, int pos){

	if (pos == 0){ printf ("Memory Write Error - should only be writing to 0th segment.. "); return -1;}
	if (strlen(msg) == 0){printf ("Memory Write Error - empty string"); return -1;}
	memcpy(shared_memory_address+(pos*seg_len), msg, strlen(msg));
}//write_mem_cmd()


void read_mem_string (char * buf, int pos, int length){

	if (length == 0)		//read a length of buf_max, otherwise read only one segment at given position
		memcpy(buf, shared_memory_address + (pos*seg_len), buf_max);
	else if (length == 1)
		memcpy(buf, shared_memory_address + (pos*seg_len), seg_len);
	else
		{printf ("Error Reading Memory - length: %d", length); return;}

	//printf ("memory: \n%s", **buf);

	return;
}//read_mem_string()


int read_sensors_from_memory (int * sensor, char * string){
	//pos: segment # in memory.. left is 2, right 3, front 4, yaw 5, dist 6

	char *buf = malloc (sizeof(char)*buf_max);
	char *seg = 0;
	int flag = 0;
	int pos = 0;
	
	if        (strcmp(string,"left")   == 0) pos = 2;
	else if (strcmp(string,"right") == 0) pos = 3;
	else if (strcmp(string,"front") == 0) pos = 4;
	else if (strcmp(string,"yaw")  == 0) pos = 5;
	else if (strcmp(string,"dist")   == 0) pos = 6;
	else{	printf ("Read Memory Error - Invalid string: %s, pos: %d\n", string, pos); return -1;}
	
	read_mem_string(buf, pos, 1); 
	
    //If flag byte for sensor being read from memory is 1, process it and write 0 to flag byte in memory
    //This ensures that we do not read any non-updated sensor value in memory
	if (isValid(buf) == 1){
		flag = 1;
		*sensor = memorytoint (buf);
		//printf ("flag: %d\n", flag);
		write_mem_string("0", pos);
	}
	
	//printf ("buf2: %s", buf);
	
	free (buf);
	return flag;
}

int isValid (char * value){
	//Checks the flag digit of the sensor reading from memory. If 1 return true (good to read), otherwise false.
	char c = '1';
	char p = *value;

	if (value[0] == '1')
		return 1;
	else
		return 0;
}

int memorytoint(char * buf){

	char *shifter = buf+1;
	char *ori = buf;
	while (*shifter != '\n'){
	
		*ori = *shifter;
		
		ori++;
		shifter++;
	}
	*ori = 0;
	//printf ("buf1: %s\n", buf);
	//printf ("length: %d\n", atoi(buf));
	
	return atoi(buf);
}

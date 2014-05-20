//shared memory example http://www.stuffaboutcode.com/2013/08/shared-memory-c-python-ipc.html
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "planner.h"
#include "shmemory.h"

/*IMPORTANT: always add \n to end of messages being written to memory
 *
 *
 *
 *
 */

/*
int main(int argc, char *argv[]){

	init_planner();
	main_planner();

	printf ("hello zift");
	return 0;
}
*/

// **** Declare Variables ****//



// **** Functions ****//

void init_main(){

	//Give read/write permissions to USB port
//	system("sudo chmod a+rw /dev/ttyACM0;");
//	system("python /home/mahmoud/Documents/capstone/comm/comm.py &");
}




int main(int argc, const char **argv)
{

		//***SIM Variables
		int front=0, left=1, right=32, yaw=-1, dist=0;	//Sonar variables

		//***Shared Memory Variables
		char *msg = {0};
		int shmid;

		//***main Variables
		int count=0;
		
		//***init functions
		init_main();
		init_planner ();
		//init_serial();		//Open and Initialize usb port
		init_sharedmem(&shmid);
		
		//write_to_serial("Starting");
sleep(1);
//write_mem_string("1000000rn\n", 1);
//serial_flush();
		while (true){

			//	ReadSensors ();	//reads sensor values and stores to shared memory

				//***Planner code
				main_planner();
				
			//	printf ("\n--LOOP--\n");
				usleep(500);
				//if (count++ > 50){
				//	printf ("\nexiting\n");
					// sleep so there is enough time to run the reader!
				//	sleep(10);

				//	close_usb_port ();

					// Detach and remove shared memory
				//	shmdt(shmid);
				//	shmctl(shmid, IPC_RMID, NULL);
				//	break;
				//}
		}
		return 0;
}



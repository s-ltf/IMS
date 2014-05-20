#ifndef MYSERIAL_H
#define MYSERIAL_H

void init_comm ();
int getWallDist (int fd, char direc);
//int requestWallReading ();
void close_usb_port ();

int write_pln_cmd (char * msg, int pos);
int write_mem_string (char * msg, int pos);
int read_sensors_from_memory (int * sensor, char* string);

#endif

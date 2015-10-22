#ifndef _APPLICATION_LAYER_H_
#define _APPLICATION_LAYER_H_

//camada de aplicação

struct applicationLayer {
	/*serial port file descriptor*/
	int fileDescriptor; 
	
	int status; /* 0 == TRANSMITTER | 1 == RECEIVER*/

	int fileSize;
}

//slide 18, 19, 20


#endif
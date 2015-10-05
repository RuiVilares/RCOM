/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1


#define FLAG 0x7e
#define A 0x03
#define C_SET 0x07
#define C_UA 0x03
#define LENGTH 5

volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[1];
	char SET[LENGTH], UA[LENGTH];

    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS4", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS0", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS4\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
  
    
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */



  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) próximo(s) caracter(es)
  */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");

	int i=0;

    while (STOP==FALSE) {       /* loop for input */
	res = read(fd,buf,1);   /* returns after 1 char have been input */
	SET[i] = buf[0];

if(i==4 && SET[i] == FLAG)
STOP = TRUE;
else
i++;
    }

UA[0] = FLAG;
UA[1] = A;
UA[2] = C_UA;
UA[3] = A^C_UA;
UA[4] = FLAG;

if(SET[3] != (SET[1]^SET[2])){

printf("Error receiving the set!");
//
}

printf("%x,%x,%x,%x,%x\n", SET[0], SET[1], SET[2], SET[3], SET[4]);

    tcflush(fd, TCOFLUSH);
res = write(fd, UA, LENGTH);






  /* 
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no guião 
  */



    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}

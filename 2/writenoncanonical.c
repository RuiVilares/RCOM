/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7e
#define A 0x03
#define C_SET 0x07
#define UA 0x03

volatile int STOP=FALSE;

int flag=1, conta=1;
int fd;

void atende()                   // atende alarme
{
	if (flag) {
		printf("alarme # %d\n", conta);
		conta++;
		unsigned char SET[5];
		    SET[0] = SET[4] = 0x0;
	printf("Antes!\n");
		write(fd,SET,5);
printf("Depois!\n");
	}
}

int main(int argc, char** argv)
{
    int c, res;
    struct termios oldtio,newtio;
    char buf[255];
    int i, sum = 0, speed = 0;
    
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

newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    tcflush(fd, TCIOFLUSH);

    printf("New termios structure set\n");

    unsigned char SET[5], RECEIVED[5];
    SET[0] = SET[4] = FLAG;
    SET[1] = A;
    SET[2] = C_SET;
    SET[3] = SET[1] ^ SET[2];

    (void) signal(SIGALRM, atende);  // instala  rotina que atende interrupcao

    while(conta < 4 && flag){
      alarm(3);                 // activa alarme de 3s
      write(fd,SET,5);
printf("Antes oficial!\n");
      read(fd,RECEIVED,5);
printf("Depois oficial!\n");
      if (RECEIVED[0] == RECEIVED[4] && RECEIVED[4] == FLAG && RECEIVED[1] == A && (RECEIVED[3] == (RECEIVED[1] ^ RECEIVED[2])) && RECEIVED[2] == UA)
      {
	flag = 0;
      }
}

     if (conta >= 4) {
	perror("Missing acknowledgment!\n");
	if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
	      perror("tcsetattr");
	      exit(-1);
	    }
	    close(fd);
	    return -1;
     }




newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");


do
{
	printf("\nEnter message (max. 254 char): ");
} while(gets(buf) == NULL);
    
    res = write(fd,buf,strlen(buf)+1);   
    printf("%d bytes written\n", res);
 

  /* 
    O ciclo FOR e as instruções seguintes devem ser alterados de modo a respeitar 
    o indicado no guião 
  */



   
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }




    close(fd);
    return 0;
}

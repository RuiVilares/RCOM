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

int state=1, conta=1;
int fd;
unsigned char SET[5];

void stateMachine(char msg){

switch(s){
	case 0:
		if (msg==FLAG)
			state++;
		break;
	case 1:
		switch(msg){
			case A:
				state++;
				break;	
			case FLAG:
				break;	
			default:
				state=0;
				break;
		}
		break;
	case 2:
		switch(msg){
			case FLAG:
				state=1;
				break;	
			case C_SET:
				state++;
				break;	
			default:
				state=0;
				break;
		}
		break;
	case 3: 
		if (msg==SET[1] ^ SET[2])
			state++;
		else
			state=0;
		break;
	case 4:
		if (msg==FLAG)
			state++;
		else
			state=0;
		break;
	case 5:
		break;
	default:
		break;
}
}

void atende()                   // atende alarme
{
    if (flag) {
        
        conta++;
        write(fd,SET,5);
        alarm(3);
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
        printf("Usage:tnserial SerialPortntex: nserial /dev/ttyS4n");
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
    
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */
    
    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }
    
    tcflush(fd, TCIOFLUSH);
    
    printf("New termios structure set\n");
    
    unsigned char *RECEIVED;
    SET[0] = FLAG; SET[4] = FLAG;
    SET[1] = A;
    SET[2] = C_SET;
    SET[3] = SET[1] ^ SET[2];
    
    (void) signal(SIGALRM, atende);  // instala  rotina que atende interrupcao
    alarm(3);                 // activa alarme de 3s
    write(fd,SET,5);
int state = 0;

    while(conta < 4 && state < 5){
        read(fd,RECEIVED,1);
        stateMachine(*RECEIVED);
    }
            alarm(0);
    
    if (conta >= 4) {
        perror("Missing acknowledgment!n");
        if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
            perror("tcsetattr");
            exit(-1);
        }
        close(fd);
        return -1;
    }
    
    
    
    
    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */
    
    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }
    
    printf("New termios structure setn");
    
    
    do
    {
        printf("nEnter message (max. 254 char): ");
    } while(gets(buf) == NULL);
    
    res = write(fd,buf,strlen(buf)+1);
    printf("%d bytes writtenn", res);
    
    
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

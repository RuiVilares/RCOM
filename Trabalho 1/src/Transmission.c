#include "Transmission.h"

int openPort(char *portName) {
	int fd = open(portName, O_RDWR | O_NOCTTY );

    if (fd < 0) {
    	perror(portName);
    	return EXIT_ERROR;
    }

    return fd;
}

void setUpPort(linkLayer *linkStruct) {
	if (tcgetattr(linkStruct->fd, &(linkStruct->savetio)) == -1) {
		perror("tcgetattr");
		exit(EXIT_ERROR);
	}

	struct termios newtio;
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = linkStruct->baudRate | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    newtio.c_lflag = NON_CANONICAL_INPUT;
    
    newtio.c_cc[VTIME] = INTER_CHARACTER_TIMER_UNUSED;
    newtio.c_cc[VMIN] = BLOCKING_READ_CHAR;

    tcflush(linkStruct->fd, TCIOFLUSH);

    if ( tcsetattr(linkStruct->fd, TCSANOW, &newtio) == -1) {
        perror("tcsetattr");
        exit(EXIT_ERROR);
    }
}

void closePort(int fd, struct termios oldtio) {
	if ( tcsetattr(fd,TCSANOW, &oldtio) == -1) {
        perror("tcsetattr");
        exit(EXIT_ERROR);
    }

	close(fd);
}

int sendInformation(int fd, char *buffer, int length) {

    int sentBytes = 0;
    int writeResult;

    writeResult = write(fd, &(buffer[sentBytes]), length-sentBytes);
    sentBytes += writeResult;

    while (sentBytes < length)
    {
        sleep(WRITE_AGAIN_INTERVAL);
        writeResult = write(fd, &(buffer[sentBytes]), length-sentBytes);
        sentBytes += writeResult;
    }

    return sentBytes;
}

char readByte(int fd, int alarmTime, char *readB) {

    resetAlarmGlobal();
    alarm(alarmTime);
    int result;

    while (!alarmGlobal && ((result = (read(fd, readB, 1))) == 0 || (result == -1))) {
        
    }

    fsync(fd);

    alarm(CANCEL_ALARM);

    if (result == 0 || result == -1) {
        fprintf(stderr, "Timed out to read character\n");
        return EXIT_ERROR;
    }

    resetNumTransmissionsGlobal();
    return SUCCESS_EXIT;
}

void sendSet(int fd) {
    char buffer[10];
    int length = LENGTH_SU;
    
    generateFieldUAandSet(buffer);
    buffer[CONTROL_SM_INDEX] = SET_CONTROL_FIELD;
    buffer[BCC_SM_INDEX] = buffer[CONTROL_SM_INDEX] ^ buffer[ADDRESS_SM_INDEX];

    sendInformation(fd, buffer, length);

    printf("Set Command sent.\n");
}

void sendUA(int fd) {
    char buffer[10];
    int length = LENGTH_SU;
    
    generateFieldUAandSet(buffer);
    buffer[CONTROL_SM_INDEX] = UA_CONTROL_FIELD;
    buffer[BCC_SM_INDEX] = buffer[CONTROL_SM_INDEX] ^ buffer[ADDRESS_SM_INDEX];

    sendInformation(fd, buffer, length);

    printf("UA message sent.\n");
}

void sendUA01(int fd) {
    char buffer[10];
    int length = LENGTH_SU;
    
    generateFieldUAandSet01(buffer);
    buffer[CONTROL_SM_INDEX] = UA_CONTROL_FIELD;
    buffer[BCC_SM_INDEX] = buffer[CONTROL_SM_INDEX] ^ buffer[ADDRESS_SM_INDEX];

    sendInformation(fd, buffer, length);

    printf("UA message sent.\n");
}

void generateFieldUAandSet(char *buffer) {
    buffer[FLAG_INITIAL_SM_INDEX] = FLAG;
    buffer[ADDRESS_SM_INDEX] = CMD_TRA_ANS_REC_ADDRESS_FIELD;
    buffer[FLAG_FINAL_SM_INDEX] = FLAG;
}

void generateFieldUAandSet01(char *buffer) {
    buffer[FLAG_INITIAL_SM_INDEX] = FLAG;
    buffer[ADDRESS_SM_INDEX] = CMD_REC_ANS_TRA_ADDRESS_FIELD;
    buffer[FLAG_FINAL_SM_INDEX] = FLAG;
}

void sendDiscTransmitter(int fd) {
    char buffer[10];
    int length = LENGTH_SU;
    
    generateFieldUAandSet(buffer);
    buffer[CONTROL_SM_INDEX] = DISC_CONTROL_FIELD;
    buffer[BCC_SM_INDEX] = buffer[CONTROL_SM_INDEX] ^ buffer[ADDRESS_SM_INDEX];

    sendInformation(fd, buffer, length);

    printf("DISC command sent.\n");
}

void sendDiscReceiver(int fd) {
    char buffer[10];
    int length = LENGTH_SU;
    
    generateFieldUAandSet01(buffer);
    buffer[CONTROL_SM_INDEX] = DISC_CONTROL_FIELD;
    buffer[BCC_SM_INDEX] = buffer[CONTROL_SM_INDEX] ^ buffer[ADDRESS_SM_INDEX];

    sendInformation(fd, buffer, length);

    printf("DISC command sent.\n");
}
#include "StateMachine.h"

bool stateMachineReadWrite(int fd, int timeout, char *buffer, int *size, char sequenceNumber) {
	bool init = true;
	bool receiving = true;
	int counter = 0;
	char readB = 0;
	char ret = 0;

	while (receiving && counter < FIELD_MAX_SIZE) {
		fsync(fd);
		ret = readByte(fd, timeout, &readB);
		fsync(fd);
		if (ret == EXIT_ERROR) {
			return false;
		}
		switch(readB) {
			case FLAG:
				buffer[counter] = readB;
				if (init) {
					init = false;
				}
				else {
					receiving = false;
				}
				counter++;
				break;
			default:
				//ignora enquanto não receber a flag para começar
				if (!init) {
					buffer[counter] = readB;
					counter++;
				}
				break;
		}
	}
	
	if (!(counter < FIELD_MAX_SIZE)) {
		return false;
	}

	*size = counter;

	return checkIntegrityAndControlRRandREJ(buffer, *size, sequenceNumber);
}

bool stateMachineRead(int *size, char *buffer, linkLayer *linkStruct) {
	bool init = true;
	bool receiving = true;
	int counter = 0;
	char readB = 0;
	char ret = 0;

	while (receiving && counter < FIELD_MAX_SIZE) {
		fsync(linkStruct->fd);
		ret = readByte(linkStruct->fd, linkStruct->timeout, &readB);
		fsync(linkStruct->fd);
		if (ret == EXIT_ERROR) {
			return false;
		}
		switch(readB) {
			case FLAG:
				linkStruct->frame[counter] = readB;
				if (init) {
					init = false;
				}
				else {
					receiving = false;
				}
				counter++;
				break;
			default:
				//ignora enquanto não receber a flag para começar
				if (!init) {
					linkStruct->frame[counter] = readB;
					counter++;
				}
				break;
		}
	}

	if (!(counter < FIELD_MAX_SIZE)) {
		return false;
	}

	linkStruct->frameSize = counter;

	return checkErrorOrSetOrDiscOrInfo(size, buffer, linkStruct);
}

bool checkErrorOrSetOrDiscOrInfo(int *size, char *buffer, linkLayer *linkStruct) {

	if (linkStruct->frameSize < LENGTH_SU) {
		return false;
	}

	if (linkStruct->frame[FLAG_INITIAL_INDEX] != FLAG) {
		return false;
	}

	if (linkStruct->frame[ADDRESS_FIELD_INDEX] != CMD_TRA_ANS_REC_ADDRESS_FIELD) {
		return false;
	}

	if (linkStruct->frame[BCC1_FIELD_INDEX] !=
		(linkStruct->frame[ADDRESS_FIELD_INDEX] ^ linkStruct->frame[CONTROL_FIELD_INDEX]))
	{
		return false;
	}

	if (linkStruct->frame[CONTROL_FIELD_INDEX] == DISC_CONTROL_FIELD) {
		return true;
	}

	if (linkStruct->frame[CONTROL_FIELD_INDEX] == SET_CONTROL_FIELD) {
		return true;
	}

	return checkDataIntegity(size, buffer, linkStruct);
}

bool checkDataIntegity(int *size, char *buffer, linkLayer *linkStruct) {

	*size = destuffData(buffer, linkStruct);

	int i = 0;

	char bcc = 0;
	for (; i < *size; i++) {
		bcc = bcc ^ buffer[i];
	}
	return (bcc == buffer[*size] &&
			FLAG == linkStruct->frame[linkStruct->frameSize-1] &&
			linkStruct->frame[CONTROL_FIELD_INDEX] == linkStruct->sequenceNumber);
}

int destuffData(char *buffer, linkLayer *linkStruct) {
	int i = DATA_START_INDEX;
	int dataI = 0;

	for (; i < linkStruct->frameSize-1; i++) {
		switch(linkStruct->frame[i])
		{
			case FIRST_ESCAPE:
				i++;
				switch(linkStruct->frame[i]) {
					case FLAG_ESCAPE:
						buffer[dataI] = FLAG;
						dataI++;
						break;
					default: //(SECOND_ESCAPE)
						buffer[dataI] = FIRST_ESCAPE;
						dataI++;
						break;
				}
				break;
			default:
				buffer[dataI] = linkStruct->frame[i];
				dataI++;
				break;
		}
	}

	dataI--;

	return dataI;
}

bool stateMachineSet(int fd, int timeout) {
	char readB = 0;

	int status = INITIAL_SM;
	char address, control;
	char ret = 0;

	while (numTransmissionsGlobalGreaterThanZero()) {
		fsync(fd);
		ret = readByte(fd, timeout, &readB);
		fsync(fd);
		if (ret == EXIT_ERROR) {
			status = INITIAL_SM;
			continue;
		}
		switch(status) {
			case INITIAL_SM:
				if (readB == FLAG) {
					status = FLAG_INITIAL_SM_INDEX;
				}
				break;
			case FLAG_INITIAL_SM_INDEX:
				switch (readB) {
					case CMD_TRA_ANS_REC_ADDRESS_FIELD:
						status = ADDRESS_SM_INDEX;
						address = readB;
						break;
					case FLAG:
						break;
					default:
						status = INITIAL_SM;
						break;
				}
				break;
			case ADDRESS_SM_INDEX:
				switch (readB) {
					case SET_CONTROL_FIELD:
						status = CONTROL_SM_INDEX;
						control = readB;
						break;
					case FLAG:
						status = FLAG_INITIAL_SM_INDEX;
						break;
					default:
						status = INITIAL_SM;
						break;
				}
				break;
			case CONTROL_SM_INDEX:
				if (readB == (address ^ control)) {
					status = BCC_SM_INDEX;
				}
				else if (readB == FLAG) {
					status = FLAG_INITIAL_SM_INDEX;
				}
				else {
					status = INITIAL_SM;
				}
				break;
			case BCC_SM_INDEX:
				if (readB == FLAG) {
					return true;
				}
				else {
					status = INITIAL_SM;
				}
				break;
			default://nunca vai chegar aqui
				break;
		}
	}

	return false;
}

bool stateMachineUA(int fd, int timeout, int option) {

	char readB = 0;
	char ret = 0;

	int status = INITIAL_SM;
	char address, control;

	while (numTransmissionsGlobalGreaterThanZero()) {
		fsync(fd);
		ret = readByte(fd, timeout, &readB);
		fsync(fd);
		if (ret == EXIT_ERROR) {
			if (numTransmissionsGlobalGreaterThanZero()) {
				if (option == TRANSMITTER) {
					sendSet(fd);
				}
				else {
					sendDiscReceiver(fd);
				}
			}
			continue;
		}
		switch(status) {
			case INITIAL_SM:
				if (readB == FLAG) {
					status = FLAG_INITIAL_SM_INDEX;
				}
				break;
			case FLAG_INITIAL_SM_INDEX:
				switch (readB) {
					case CMD_REC_ANS_TRA_ADDRESS_FIELD:
						status = ADDRESS_SM_INDEX;
						address = readB;
						break;
					case FLAG:
						break;
					default:
						status = INITIAL_SM;
						break;
				}
				break;
			case ADDRESS_SM_INDEX:
				switch (readB) {
					case UA_CONTROL_FIELD:
						status = CONTROL_SM_INDEX;
						control = readB;
						break;
					case FLAG:
						status = FLAG_INITIAL_SM_INDEX;
						break;
					default:
						status = INITIAL_SM;
						break;
				}
				break;
			case CONTROL_SM_INDEX:
				if (readB == (address ^ control)) {
					status = BCC_SM_INDEX;
				}
				else if (readB == FLAG) {
					status = FLAG_INITIAL_SM_INDEX;
				}
				else {
					status = INITIAL_SM;
				}
				break;
			case BCC_SM_INDEX:
				if (readB == FLAG) {
					return true;
				}
				else {
					status = INITIAL_SM;
				}
				break;
			default://nunca vai chegar aqui
				break;
		}
	}

	return false;
}

bool stateMachineDisc(int fd, int timeout) {

	sendDiscTransmitter(fd);

	char readB = 0;
	char ret = 0;

	int status = INITIAL_SM;
	char address, control;

	while (numTransmissionsGlobalGreaterThanZero()) {
		fsync(fd);
		ret = readByte(fd, timeout, &readB);
		fsync(fd);
		if (ret == EXIT_ERROR) {
			if (numTransmissionsGlobalGreaterThanZero()) {
				sendDiscTransmitter(fd);
			}
			continue;
		}
		switch(status) {
			case INITIAL_SM:
				if (readB == FLAG) {
					status = FLAG_INITIAL_SM_INDEX;
				}
				break;
			case FLAG_INITIAL_SM_INDEX:
				switch (readB) {
					case CMD_REC_ANS_TRA_ADDRESS_FIELD:
						status = ADDRESS_SM_INDEX;
						address = readB;
						break;
					case FLAG:
						break;
					default:
						status = INITIAL_SM;
						break;
				}
				break;
			case ADDRESS_SM_INDEX:
				switch (readB) {
					case DISC_CONTROL_FIELD:
						status = CONTROL_SM_INDEX;
						control = readB;
						break;
					case FLAG:
						status = FLAG_INITIAL_SM_INDEX;
						break;
					default:
						status = INITIAL_SM;
						break;
				}
				break;
			case CONTROL_SM_INDEX:
				if (readB == (address ^ control)) {
					status = BCC_SM_INDEX;
				}
				else if (readB == FLAG) {
					status = FLAG_INITIAL_SM_INDEX;
				}
				else {
					status = INITIAL_SM;
				}
				break;
			case BCC_SM_INDEX:
				if (readB == FLAG) {
					return true;
				}
				else {
					status = INITIAL_SM;
				}
				break;
			default://nunca vai chegar aqui
				break;
		}
	}

	return false;
}
#include "ApplicationLayer.h"

bool openReadFile(applicationLayer *applicationStruct, char *path) {
	int fd = open(path, O_RDONLY, FILE_PERMISSIONS);
	if (fd == -1) {
		return false;
	}

	applicationStruct->fdFile = fd;
	char *s = basename(path);
	strcpy(applicationStruct->fileName, s);
	
	struct stat buf;
	if (fstat(fd, &buf) == 0) {
		if (buf.st_size > FILE_MAX_SIZE) {
			close(fd);
			return false;
		}
		else {
			applicationStruct->fileSize = buf.st_size;
		}
	}

	return true;
}

bool sameFileSize(int fd, int size) {
	struct stat buf;
	if (fstat(fd, &buf) == 0) {
		return buf.st_size == size;
	}

	return false;
}

bool openWriteFile(applicationLayer *applicationStruct) {
	char path[256];
	strcpy(path, "./");
	strcat(path, applicationStruct->fileName);
	int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, FILE_PERMISSIONS);
	if (fd == -1) {
		return false;
	}

	applicationStruct->fdFile = fd;

	return true;
}

int readFile(char *buffer, int size, int fd) {
	return read(fd, buffer, size);
}

void writeFile(char *buffer, int size, int fd) {
	write(fd, buffer, size);
}

void closeFile(int fd) {
	close(fd);
}

int createInitialControlPacket(char *packet, char *fileName, int fileSize) {
	packet[PACKET_CONTROL_FIELD_INDEX] = START;
	return createBodyControlPacket(packet, fileName, fileSize);
}

int createFinalControlPacket(char *packet, char *fileName, int fileSize) {
	packet[PACKET_CONTROL_FIELD_INDEX] = END;
	return createBodyControlPacket(packet, fileName, fileSize);
}

int createBodyControlPacket(char *packet, char *fileName, int fileSize) {
	int size = PACKET_CONTROL_FIELD_INDEX + 1;

	copy(packet, &size, FILE_SIZE_INDEX);
	copy(packet, &size, 4);
	copy(packet, &size, (fileSize >> 24) & 0x0FF);
	copy(packet, &size, (fileSize >> 16) & 0x0FF);
	copy(packet, &size, (fileSize >> 8) & 0x0FF);
	copy(packet, &size, fileSize & 0x0FF);

	copy(packet, &size, NAME_INDEX);
	copy(packet, &size, strlen(fileName)+1);
	int i = 0;
	for (; i < strlen(fileName); i++) {
		copy(packet, &size, fileName[i]);
	}
	copy(packet, &size, 0);

	return size;
}

void copy(char *buffer, int *size, char data) {
	buffer[(*size)] = data;
	(*size) = (*size) + 1;
}

void createDataPacket(applicationLayer *applicationStruct) {
	if (applicationStruct->endFile) {
		return;
	}

	char buffer[MAX_SIZE+10];
	int size = readFile(buffer, applicationStruct->maxPacketSize, applicationStruct->fdFile);
	
	if (size == 0) {
		applicationStruct->endFile = true;
		return;
	}

	if (size != applicationStruct->maxPacketSize) {
		applicationStruct->endFile = true;
	}

	applicationStruct->currentPacketSize = size + PACKET_DATA_INFO_SIZE;

	//campo 1
	applicationStruct->currentPacket[PACKET_CONTROL_FIELD_INDEX] = DATA;
	//campo 2
	applicationStruct->currentPacket[SN_INDEX] = applicationStruct->sequenceNumber;
	applicationStruct->sequenceNumber = (applicationStruct->sequenceNumber + 1) % MOD_SN;

	//campo 3 e 4
	applicationStruct->currentPacket[L2_INDEX] = ((size >> 8) & 0x0FF);
	applicationStruct->currentPacket[L1_INDEX] = (size & 0x0FF);

	//campo dos dados
	int i = 0;
	int j = PACKET_DATA_START_INDEX;
	for (; i < size; i++, j++) {
		applicationStruct->currentPacket[j] = buffer[i];
	}

	applicationStruct->fileTransfer = applicationStruct->fileTransfer + size;
}

void writeFileFromDataPacket(applicationLayer *applicationStruct) {
	applicationStruct->sequenceNumber = (applicationStruct->sequenceNumber + 1) % MOD_SN;

	int size = (((applicationStruct->currentPacket[L2_INDEX] & 0x0FF) << 8) & 0xFF00) +
				(applicationStruct->currentPacket[L1_INDEX] & 0x0FF);


	applicationStruct->fileTransfer = applicationStruct->fileTransfer + size;
	printf("Completed %f%%\n", (applicationStruct->fileTransfer / ((double) applicationStruct->fileSize)) * 100);

	writeFile(&(applicationStruct->currentPacket[PACKET_DATA_START_INDEX]), size,
				applicationStruct->fdFile);
}

bool readFromControlPacket(char *packet, applicationLayer *applicationStruct) {
	int index = FIELD_1;

	if (packet[index] != FILE_SIZE_INDEX) {
		return false;
	}

	index += 2;

	applicationStruct->fileSize = (((packet[index] & 0xFF) << 24) & 0xFF000000) +
								(((packet[index+1] & 0xFF) << 16) & 0x00FF0000) +
								(((packet[index+2] & 0xFF) << 8) & 0x0000FF00) +
								((packet[index+3] & 0xFF) & 0x000000FF);
	
	index += 4;

	if (packet[index] != NAME_INDEX) {
		return false;
	}
	index++;
	char size = packet[index];
	index++;
	int i = 0;
	for (; i < size; i++) {
		applicationStruct->fileName[i] = packet[i+index];
	}
	index += i;

	return true;
}

void removeFile(char *fileName) {
	remove(fileName);
}
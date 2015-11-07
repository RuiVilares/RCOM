/*Non-Canonical Input Processing*/

#include <stdio.h>
#include <string.h>
#include <termios.h>
#include "DefaultValues.h"
#include "GlobalVariable.h"
#include "Transmission.h"
#include "LinkLayerStruct.h"
#include "LinkLayer.h"
#include "ApplicationLayerStruct.h"
#include "ApplicationLayer.h"
#include "Alarm.h"
#include "CLI.h"

struct linkLayerStruct;
struct applicationLayerStruct;

//Se tiver 2 argumentos, então 2 é o path do ficheiro a transmitir.
//Assim, este computador será o transmitter
//Se tiver 1 argumento, então este computador será o receiver.
//Caso contrário, mostrar mensagem de erro e terminar
int main(int argc, char** argv)
{
	if (argc != RECEIVER_ARGS && argc != TRANSMITTER_ARGS) {
		printf("serialPort <port_name> [<path>]\n");
		return EXIT_ERROR;
	}

	linkLayer linkStruct;
	applicationLayer applicationStruct;

	strcpy(applicationStruct.port, argv[PORT_NAME_ARG]);
	system("clear");
	if (argc == RECEIVER_ARGS) {
		applicationStruct.status = RECEIVER;
	}
	else {
		applicationStruct.status = TRANSMITTER;
		strcpy(applicationStruct.fileName, argv[PATH_ARG]);
		chooseDataPacketsSize(&applicationStruct);
	}

	makeChoices(&linkStruct);
	setUpGlobalTransmissionNum(linkStruct.numTransmissions);
	setUpAlarmSignal();

	if (!llopen(applicationStruct.port, applicationStruct.status, &linkStruct)) {
		printf("Couldn't start connection!\n");
		return EXIT_ERROR;
	}

	int retClose;
	int status = SET_UP;
	int transferStatus = TRANSFER_INITIAL;
	while (status != FINISH_PROGRAM) {
		switch (status) {
			case SET_UP:
				startValues(&applicationStruct, &linkStruct);
				if (applicationStruct.status == TRANSMITTER) {
					openReadFile(&applicationStruct, argv[PATH_ARG]);
				}
				status = TRANSFER;
				transferStatus = TRANSFER_INITIAL;
				break;

			case TRANSFER:
				switch(transferStatus) {
					case TRANSFER_INITIAL:
						if (applicationStruct.status == TRANSMITTER) {
							applicationStruct.currentPacketSize = createInitialControlPacket(applicationStruct.currentPacket, applicationStruct.fileName, applicationStruct.fileSize);
							int ret = llwrite(applicationStruct.currentPacket, applicationStruct.currentPacketSize, &linkStruct);
							if (ret == EXIT_ERROR) {
								printf("Couldn't write to the other side\n");
								closePort(linkStruct.fd, linkStruct.savetio);
								closeFile(applicationStruct.fdFile);
								return EXIT_ERROR;
							}
							transferStatus = TRANSFER_DATA;
						}
						else {
							int ret = llread(applicationStruct.currentPacket, &linkStruct);
							if (ret == DISC_EXIT) {
								status = CLOSE;
								break;
							}
							else if (ret == SET_EXIT) {
								transferStatus = TRANSFER_INITIAL;
								status = SET_UP;
								break;
							}
							else {
								applicationStruct.currentPacketSize = ret;
								readFromControlPacket(applicationStruct.currentPacket, &applicationStruct);
								if (!openWriteFile(&applicationStruct)) {
									printf("File already exists\n");
									closePort(linkStruct.fd, linkStruct.savetio);
									closeFile(applicationStruct.fdFile);
									return EXIT_ERROR;
								}
								transferStatus = TRANSFER_DATA;
							}
						}
						break;
					case TRANSFER_DATA:
						if (applicationStruct.status == TRANSMITTER) {
							createDataPacket(&applicationStruct);
							int ret = llwrite(applicationStruct.currentPacket, applicationStruct.currentPacketSize, &linkStruct);
							if (ret == EXIT_ERROR) {
								status = TRANSFER_END;
								break;
							}
							else {
								infoFieldGlobal++;
								printf("Completed %f%%\n", (applicationStruct.fileTransfer / ((double) applicationStruct.fileSize)) * 100);
								if (applicationStruct.endFile) {
									transferStatus = TRANSFER_END;
								}
							}
						}
						else {
							int ret = llread(applicationStruct.currentPacket, &linkStruct);
							if (ret == DISC_EXIT) {
								status = CLOSE;
								break;
							}
							else if (ret == SET_EXIT) {
								transferStatus = TRANSFER_INITIAL;
								status = SET_UP;
								removeFile(applicationStruct.fileName);
								break;
							}
							else if (ret != SAME_MESSAGE) {
								applicationStruct.currentPacketSize = ret;
								if (applicationStruct.currentPacket[PACKET_CONTROL_FIELD_INDEX] == END) {
									transferStatus = TRANSFER_END;
								}
								else {
									if ((unsigned char) applicationStruct.currentPacket[SN_INDEX] != applicationStruct.sequenceNumber) {
										printf("Packet lost in the transfer. Receiver will now hault to prevent further errors.\n");
										return EXIT_ERROR;
									}
									infoFieldGlobal++;
									writeFileFromDataPacket(&applicationStruct);
								}
							}
						}
						break;
					case TRANSFER_END:
						if (applicationStruct.status == TRANSMITTER) {
							applicationStruct.currentPacketSize = createFinalControlPacket(applicationStruct.currentPacket, applicationStruct.fileName, applicationStruct.fileSize);
							int ret = llwrite(applicationStruct.currentPacket, applicationStruct.currentPacketSize, &linkStruct);
							if (ret == EXIT_ERROR) {
								printf("Couldn't write to the other side\n");
								closePort(linkStruct.fd, linkStruct.savetio);
								closeFile(applicationStruct.fdFile);
								return EXIT_ERROR;
							}
						}
						status = CLOSE;
						break;
					default:
						break;
				}
				break;

			case CLOSE:
				retClose = llclose(&linkStruct, applicationStruct.status);
				if (retClose == EXIT_ERROR) {
					printf("Error closing the connection\n");
				}
				if (applicationStruct.status == RECEIVER) {
					if (!sameFileSize(applicationStruct.fdFile, applicationStruct.fileSize)) {
						printf("File hasn't the size specified in the control packets. Probably some packet(s) lost in the transfer.\n");
					}
					else {
						printf("File has the same size of the specified in the control packets\n");
					}
				}
				closeFile(applicationStruct.fdFile);
				status = FINISH_PROGRAM;
				break;

			default:
				printf("Status in the Application Layer state machine wrong state\n");
				return EXIT_ERROR;
		}
	}

	if (applicationStruct.status == RECEIVER) {
		printf("Information Fields received: %d\n", infoFieldGlobal);
		printf("Number of time out: %d\n", timeOutNumberGlobal);
		printf("Number of REJ sent: %d\n", rejNumGlobal);
	}
	else {
		printf("Information Fields sent: %d\n", infoFieldGlobal);
		printf("Information Fields resent: %d\n", infoFieldReSentGlobal);
		printf("Number of time out: %d\n", timeOutNumberGlobal);
		printf("Number of REJ received: %d\n", rejNumGlobal);
	}

	return SUCCESS_EXIT;
}

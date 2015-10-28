#ifndef APPLICAITON_LAYER
#define APPLICAITON_LAYER

#include "DefaultValues.h"
#include "ApplicationLayerStruct.h"
#include "GlobalVariable.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <fcntl.h>
#include <libgen.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

struct applicationLayerStruct;

//abre o ficheiro em modo de leitura
bool openReadFile(applicationLayer *applicationStruct, char *path);

//abre o ficheiro em modo de escrito
bool openWriteFile(applicationLayer *applicationStruct);

//lê size bytes do fd para o buffer
int readFile(char *buffer, int size, int fd);

//escreve size bytes do fd para o buffer
void writeFile(char *buffer, int size, int fd);

//fecha o descritor do ficheiro
void closeFile(int fd);

//cria pacote de controlo inicial
int createInitialControlPacket(char *packet, char *fileName, int fileSize);

//cria pacote de controlo final
int createFinalControlPacket(char *packet, char *fileName, int fileSize);

//cria o corpo do pacote de controlo
int createBodyControlPacket(char *packet, char *fileName, int fileSize);

//copia o caracter data para buffer e incrementa size
void copy(char *buffer, int *size, char data);

//cria um pacote de dados
void createDataPacket(applicationLayer *applicationStruct);

//do pacote de dados escreve no ficheiro
void writeFileFromDataPacket(applicationLayer *applicationStruct);

//lê do pacote de controlo e preenche a struct
bool readFromControlPacket(char *packet, applicationLayer *applicationStruct);

//remove o ficheiro indicado pelo nome filename
void removeFile(char *fileName);

//verfica se o tamanho de fd tem o mesmo tamanho de size
bool sameFileSize(int fd, int size);

#endif
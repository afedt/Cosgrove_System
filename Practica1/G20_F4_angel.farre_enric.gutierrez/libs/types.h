/*Angel Farre - angel.farre & Enric Gutierrez - enric.gutierrez*/

#define _TYPES_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <dirent.h>
#include <time.h> 
#include <signal.h>
#include <string.h>
#include <mqueue.h>


typedef struct {
  char* name;
  int checkTime;
  char* IP;
  int port;
} Telescope_info;

typedef struct {
  char* serverIP;
  int mcGruderPort;
  int mcTavishPort;
  int time;
} Lionel_config;

typedef struct {
  char type;
  char* header;
  unsigned short int length;
  char* data;
} Data;

typedef struct {
  char* extension;
  size_t fileLength;
  char* creationDate;
} File_Info;

typedef struct {
  int numConst;
  float avgDensity;
  float maxMagnitude;
  float minMagnitude;
} Last_Astronomical_Data;

typedef struct {
  size_t size;
} mem;

typedef struct{
  long idmis;
  int n1,n2;
}Missatge1;

typedef struct{
  long idmis;
  int resultat;
}Missatge2;
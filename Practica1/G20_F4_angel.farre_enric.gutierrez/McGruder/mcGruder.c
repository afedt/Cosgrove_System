/*Angel Farre - angel.farre & Enric Gutierrez - enric.gutierrez*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <errno.h>

#include "../libs/utils.h"
#include "logic.h"


int main(int argc, char **argv){
	if (argc != 2){
		write(1, ERR_PARAM, strlen(ERR_PARAM));
		exit(0);
	}
	readConfigFile(argv[1]);
	return 0;
}
/*Angel Farre - angel.farre & Enric Gutierrez - enric.gutierrez*/

#include "../libs/utils.h"
#include "../libs/semaphore_v2.h"

char* kalkunAst;
char* kalkunImg;
int kalkunSizeAst = 0;
int kalkunSizeImg = 0;
semaphore semImg;
semaphore semTxt;
int s_fd;


int mailBox;
Missatge1 M1;
Missatge2 M2;
struct mail_msg{
	long type;
	int numConst;
	float avgDensity;
	float maxMagnitude;
	float minMagnitude;
	//int numImatges;
	int midaImatges;
};

pid_t pid;

void paquitaData(char* file, int flag, int fileLength){

	//printf("flag: %d\n", flag);
	if (flag){
		//printf("size pre treat: %d\n", fileLength);
		Last_Astronomical_Data lad = treat_astronomical_data(file, fileLength);
/*
	int message;
	char buff[100];
			message = sprintf(buff, "\tNumber of Constelations %d\n", lad.numConst);
			write(1, buff, message);
			message = sprintf(buff, "\tAvergage Density: %f\n", lad.avgDensity);
			write(1, buff, message);
			message = sprintf(buff, "\tMaximum magnitude: %f\n", lad.maxMagnitude);
			write(1, buff, message);
			message = sprintf(buff, "\tMinimum magnitude: %f\n\n", lad.minMagnitude);
			write(1, buff, message);
*/
		struct mail_msg msg = {1, lad.numConst, lad.avgDensity, lad.maxMagnitude, lad.minMagnitude, -1};
    if (msgsnd(mailBox, &msg, sizeof(msg) - sizeof(long), 0) < 0){
    	write(1, ERR_MAIL_SEND, strlen(ERR_MAIL_SEND));
    }

	}else{
		struct mail_msg msg = {2, -1, -1, -1, -1, fileLength};
    if (msgsnd(mailBox, &msg, sizeof(msg) - sizeof(long), 0) < 0){
    	write(1, ERR_MAIL_SEND, strlen(ERR_MAIL_SEND));
    }
	}
}

void updateKalkunFile(int fileLength, int flag){
	char buff [100];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

	sprintf(buff, "%d-%d-%d %d:%d %d\n%c", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, fileLength, '\0');
  	//printf("dades a escriure al kalkun: %s\n", buff);

  	if (flag == 1){
  		//SEMPAHORE
			SEM_wait(&semTxt);
	    kalkunAst = realloc(kalkunAst, kalkunSizeAst + strlen(buff));
  		int kalkunSizeAux = kalkunSizeAst;
  		for (uint i = 0; i < strlen(buff); ++i){
	    	kalkunAst[i + kalkunSizeAux] = buff[i];
	    	kalkunSizeAst = i + kalkunSizeAux;
	  	}
	    kalkunSizeAst++;
	    kalkunAst[kalkunSizeAst] = '\n';
	    //END SEMPAHORE
			SEM_signal(&semTxt);
  	}else if (flag == 2){
  		//SEMAPHORE
			SEM_wait(&semImg);
  		kalkunImg = realloc(kalkunImg, kalkunSizeImg + strlen(buff));
  		int kalkunSizeAux = kalkunSizeImg;
  		for (uint i = 0; i < strlen(buff); ++i){
	    	kalkunImg[i + kalkunSizeAux] = buff[i];
	    	kalkunSizeImg = i + kalkunSizeAux;
  		}
	    kalkunSizeImg++;
	    kalkunImg[kalkunSizeImg] = '\n';
	    //END SEMPAHORE
			SEM_signal(&semImg);
  	}
    //kalkun = (char *) realloc(kalkun, sizeof(char) * (kalkunSize));
}

void tractaDadesAstronomiques(int con, int fileLength){
	char type, header;
	int size;

	//trama per rebre el fitxer
	read(con, &type, sizeof(char));//llegim el type
	if (type != 0x03){
		return;
	}
	do{
		read(con, &header, 1);//llegim el header
		//printf("llegeixo com a header (3.dades): %c\n", header);
	}while(header != ']');
	read(con, &size, sizeof(int));//llegim la mida del fitxer
	char* file = malloc(size * sizeof(char));
	read(con, file, size);//llegim el fitxer
	//printf("file tracta dades fileLength %d\n", fileLength);
	//printf("file tracta dades %d\n", size);

    updateKalkunFile(fileLength, 1);
    paquitaData(file, 1, fileLength);

    free(file);
}


void tractaImatges(int con, size_t fileLength){
	int size;
	char * totalImage;

	//llegim el md5sum
	char* md5sumRead = malloc(32 * sizeof(char));
	read(con, md5sumRead, 32);

	int image = open("image.jpg", O_RDWR | O_CREAT, 0644);
    if(image < 0){
        write(1, ERR_OPEN_IMAGE, strlen(ERR_OPEN_IMAGE));
    }

    totalImage = malloc(fileLength * sizeof(char));

	for (size_t i = 0; i <= fileLength; i++){
    	char* file;
		char type, header;
		//trama per rebre la imatge
		read(con, &type, sizeof(char));//llegim el type
		if (type != 0x03){
			return;
		}
		do{
			read(con, &header, 1);//llegim el header
		}while(header != ']');

		read(con, &size, sizeof(int));//llegim la mida del fitxer
		file = malloc(size * sizeof(char));

		read(con, file, 1);

		totalImage[i] = *file;

		free(file);
	}

	for (u_int i = 0; i < fileLength; ++i){
		int aux = write(image, &totalImage[i], 1);

	    if(aux < 0){
	        write(1, ERR_CREATE_IMAGE, strlen(ERR_CREATE_IMAGE));
	    }
	}

	free(totalImage);

	close(image);

    char* md5sum = get_md5sum("", "image.jpg");
    char* md5sumParsed = malloc(32 * sizeof(char));
    strncpy(md5sumParsed, md5sum, 32);

    //printf("m'han enviat el md5sum: %s\n", md5sumRead);
    //printf("he generat el md5sum: %s\n", md5sumParsed);

	int i = 0;
	int flag = 1;
	while(i < 32){
		if (md5sumRead[i] != md5sum[i]){
			write(1, IMAGE_MD5SUM_KO, strlen(IMAGE_MD5SUM_KO));
			flag = 0;
			break;
		}
		i++;
	}
	if (flag){
		write(1, IMAGE_MD5SUM_OK, strlen(IMAGE_MD5SUM_OK));
		updateKalkunFile(fileLength, 2);
		paquitaData(NULL, 0, fileLength);
	}

	free(md5sum);
	free(md5sumRead);
}

void *McThreadFunction(void* arg){
	int msg;
	char buff[100];
	char header;
	int con = *((int*) arg);
	//printf("con.: %ls\n", &con);

	while(1){
		char type;
		write(1, "Waiting...\n\n", strlen("Waiting...\n\n"));

		if (read(con, &type, sizeof(char)) < 0){//llegim el type (0x01, 0x02 o 0x03)
		//	write(1, "Error al llegir el missatge del usuari!\n", strlen("Error al llegir el missatge del usuari!\n"));
		}else{
			switch(type){
				case 0x01:
				do{
					read(con, &header, 1);//llegim el header
					//printf("llegeixo com a header (1): %c\n", header);
				}while(header != ']');
				int msgsize;
				read(con, &msgsize, sizeof(int));//mida de la info
				char * info = malloc(msgsize * sizeof(char));
				read(con, info, msgsize);//info
				msg = sprintf(buff, "Connection Lionel-%s ready.\n", info);
				write(1, buff, msg);
				break;

				case 0x02:
				do{
					read(con, &header, 1);//llegim el header
					//printf("llegeixo com a header (2): %c\n", header);
				}while(header != ']');
				int msgsize2;
				read(con, &msgsize2, sizeof(int));//mida de la info
				char * info2 = malloc(msgsize2 * sizeof(char));
				read(con, info2, msgsize2);//info
				free(info2);
				msg = sprintf(buff, "Disconnecting from %s.\n", info);
				write(1, buff, msg);
				close(con);
				free(info);
				//free(con);
				return 0;
				break;

				case 0x03:
				msg = sprintf(buff, "Receiving data from %s...\n", info);
				write(1, buff, msg);

				//llegint metadata
				do{
					read(con, &header, 1);//llegim el header
				}while(header != ']');
				int size;
				read(con, &size, sizeof(int));//mida de la info del metadata
				char* metadatainfo = malloc(size * sizeof(char));
				read(con, metadatainfo, size);//llegim la info

				File_Info fInfo = get_metadata_data(metadatainfo, size);
				free(metadatainfo);

				msg = sprintf(buff, "File %s recieved!\n", fInfo.creationDate);
				write(1, buff, msg);

				if (strcmp(fInfo.extension, "txt") == 0){
					tractaDadesAstronomiques(con, fInfo.fileLength);
				}else{
					tractaImatges(con, fInfo.fileLength);
				}
				break;
			}
			//Data d = get_metadata_fields(buffer);
			//char* telescopeName;
		}
	}
	return 0;
}

void newThread(int* con, pthread_t McThread){
	int thread = 0;
	thread = pthread_create (&McThread, NULL, McThreadFunction, (void*)con);
	if (thread != 0){
		write(1, ERR_CREATE_THREAD, strlen(ERR_CREATE_THREAD));
	}
}


void mcGruderSocket(Lionel_config l_config){
	struct sockaddr_in serverConfig;
	//arranquem el socket dels McGruders
	s_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s_fd < 0){
		write(1, "Error al crear Lionel.\n", strlen("Error al crear Lionel.\n"));
	}else{
		bzero (&serverConfig, sizeof (serverConfig));

		serverConfig.sin_family = AF_INET;
        serverConfig.sin_port = l_config.mcGruderPort;
        serverConfig.sin_addr.s_addr = inet_addr(l_config.serverIP);

        if (bind (s_fd, (void *) &serverConfig, sizeof(serverConfig)) < 0){
            write(1, "Error en el bind de Lionel.\n", strlen("Error en el bind de Lionel.\n"));
        }else {
        	socklen_t len = sizeof(serverConfig);
         	listen(s_fd, 10);
			int i = 0;
			int *con;
			con = (int *) malloc(sizeof(int) * 100);
			pthread_t *thread;
			thread = (pthread_t *) malloc(sizeof(pthread_t) * 10);
			if (con == NULL){
				write(1, ERR_CONN_MC, strlen(ERR_CONN_MC));
			}
			if (thread == NULL){
				write(1, ERR_CREATE_THREAD, strlen(ERR_CREATE_THREAD));
			}
         	while(1){
				con[i] = accept(s_fd, (void *) &serverConfig, &len);
	        	newThread(&con[i], thread[i]);
				thread = (pthread_t *) realloc(thread, sizeof(pthread_t) * (i + 1));
				con = (int *) realloc(con, sizeof(int) * (i + 1));
				i++;
         	}
			//free(con);
        }
	}
}


void readConfigFile(char * fileName){
	int fd = -1;
	Lionel_config l_config;

	//llegim el fitxer de configuracio
	fd = open(fileName, O_RDONLY);
	if (fd > 0){
		l_config.serverIP = read_string(fd, '\n');
		l_config.mcGruderPort = atoi(read_string(fd, '\n'));
		l_config.mcTavishPort = atoi(read_string(fd, '\n'));
		l_config.time = atoi(read_string(fd, '\n'));

		write(1, "Starting Lionel.\n", strlen("Starting Lionel.\n"));
		mcGruderSocket(l_config);
	}else{
		write(1, ERR_FILE, strlen(ERR_FILE));
	}
}


void sigkillhandler() {
    //obrim i creem (si fa falta) el fitxer temporal de KalkunAst
    int filedesc = open("KalkunAst.txt", O_RDWR | O_CREAT | O_APPEND, 0644);
    if(filedesc < 0){
        write(1, ERR_KALKUN, strlen(ERR_KALKUN));
    }

    for (int i = 0; i < kalkunSizeAst; ++i){
    	if(write(filedesc, &kalkunAst[i], strlen(&kalkunAst[i])) != (int) strlen(&kalkunAst[i])){
        	write(1, ERR_KALKUN, strlen(ERR_KALKUN));
    	}
    }

    //obrim i creem (si fa falta) el fitxer temporal de Kalkun
    filedesc = open("Kalkun.txt", O_RDWR | O_CREAT | O_APPEND, 0644);
    if(filedesc < 0){
        write(1, ERR_KALKUN, strlen(ERR_KALKUN));
	}

	for (int i = 0; i < kalkunSizeImg; ++i){
    	if(write(filedesc, &kalkunImg[i], strlen(&kalkunImg[i])) != (int) strlen(&kalkunImg[i])){
        	write(1, ERR_KALKUN, strlen(ERR_KALKUN));
    	}
    }
}

void siginthandler() {
	//obrim i creem (si fa falta) el fitxer KalkunAst
    int fs_kalkun_ast = open("KalkunAst.txt", O_RDWR | O_CREAT | O_APPEND, 0644);
    if(fs_kalkun_ast < 0){
        write(1, ERR_KALKUN, strlen(ERR_KALKUN));
    }

	for (int i = 0; i < kalkunSizeAst; ++i){
        //escrivim
	    if(write(fs_kalkun_ast, &kalkunAst[i], 1) != 1){
	        write(1, ERR_KALKUN, strlen(ERR_KALKUN));
	    }
    }
    free(kalkunAst);

    //obrim i creem (si fa falta) el fitxer Kalkun
    int fs_kalkun_img = open("Kalkun.txt", O_RDWR | O_CREAT | O_APPEND, 0644);
    if(fs_kalkun_img < 0){
        write(1, ERR_KALKUN, strlen(ERR_KALKUN));
    }

	for (int i = 0; i < kalkunSizeImg; ++i){
        //escrivim
	    if(write(fs_kalkun_img, &kalkunImg[i], 1) != 1){
	        write(1, ERR_KALKUN, strlen(ERR_KALKUN));
	    }
    }
    free(kalkunImg);
		//tanquem els semàfors
		SEM_destructor(&semTxt);
		SEM_destructor(&semImg);

    //tanquem la cua de misstages amb paquita
    msgctl (mailBox, IPC_RMID, NULL);

    //tanquem paquita
    kill(pid, 0);

    write(s_fd, "rip", strlen("rip"));

	exit(0);
}

int main(int argc, char *argv[]){
	kalkunAst = malloc(1 * sizeof(char));
	kalkunImg = malloc(1 * sizeof(char));

	SEM_constructor(&semImg);
	SEM_init(&semImg, 1);

	SEM_constructor(&semTxt);
	SEM_init(&semTxt, 1);

	key_t mailKey = ftok("../Paquita/Paquita.c", 12);
	if (mailKey == (key_t) -1){
		write(1, "Mailbox key error on Lionel!", strlen("Mailbox key error on Lionel!"));
	}
	mailBox = msgget(mailKey, 0600 | IPC_CREAT);
	//M1.idmis = 1;
	//M1.n1 = 1;

	if (mailBox < 0){
		write(1, ERR_MAIL, strlen(ERR_MAIL));
		exit(0);
	}

    //fork paquita
    if ((pid = fork()) == -1){
    	write(1, ERR_FORK, strlen(ERR_FORK));
    	write(1, WAR_PAQUITA, strlen(WAR_PAQUITA));
    }

    if ( pid == 0 ) {
        execl("../Paquita/paquita","paquita", (char *)0);
       	write(1, ERR_EXECL, strlen(ERR_EXECL));
        exit(0);
    }

	if (argc != 2){
		write(1, ERR_PARAM, strlen(ERR_PARAM));
		exit(0);
	}
	signal(SIGINT, siginthandler);
	signal(SIGKILL, sigkillhandler);
	readConfigFile(argv[1]);
	return 0;
}

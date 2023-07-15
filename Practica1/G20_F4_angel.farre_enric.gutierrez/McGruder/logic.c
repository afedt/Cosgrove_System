/*Angel Farre - angel.farre & Enric Gutierrez - enric.gutierrez*/

#include "../libs/utils.h"

Telescope_info t_info;
int sc;
char* PATH;

int sendingText(char* fileName){
	char buff[100];
	int msg;

	msg = sprintf(buff, "File: %s\n", fileName);
	write(1, buff, msg);
	msg = sprintf(buff, "Sending %s\n", fileName);
	write(1, buff, msg);

	sprintf(buff, "%s%s", PATH, fileName);
	int fd_file = open(buff, O_RDONLY);

    int m = fileSize(buff);
	char* file = malloc(m * sizeof(char));

    if (fd_file == -1){
       	write(1, ERR_FILE, strlen(ERR_FILE));
       	return -1;
    }

	int filesize = read(fd_file, file, m);
	//enivem metadata
	msg = sprintf(buff, "txt&%d&%s", filesize, fileName);
    write(sc, &msg, sizeof(int));
	write(sc, buff, msg);

	//enviem dades
	char type = 0x03;
	write(sc, &type, 4);
	write(sc, "[]", 2);
	write(sc, &m, sizeof(int));
	//printf("envio el seguent fitxer: \n%s\n", file);
	write(sc, file, m);

	close(fd_file);
	free(file);
	return 0;
}

int sendingImage(char* fileName){
	char buff[300];
	int msg;

	msg = sprintf(buff, "File: %s\n", fileName);
	write(1, buff, msg);
	msg = sprintf(buff, "Sending %s\n", fileName);
	write(1, buff, msg);

	sprintf(buff, "%s%s%c", PATH, fileName, '\0');
	int fd_file = open(buff, O_RDONLY);
    if (fd_file == -1){
       	write(1, ERR_OPEN_IMAGE, strlen(ERR_OPEN_IMAGE));
       	return -1;
    }
    //mem m = fileSize(buff);
    int m = fsize(buff);
	char* file = (char *) malloc(m);

	int filesize = read(fd_file, file, m);
	//enivem metadata
	msg = sprintf(buff, "%s&%d&%s", get_filename_ext(fileName), filesize, fileName);
    write(sc, &msg, sizeof(int));
	write(sc, buff, msg);


	//enviem md5sum de la imatge
	char* md5sum = get_md5sum(PATH, fileName);
	
	write(sc, md5sum, 32);

	//enviem dades
	int unitatPercentatge = filesize/10;
	int i = 0;
	int j = 1;
    while (i <= filesize){
    	//enviem dades
    	char type = 0x03;
		write(sc, &type, 4);
		write(sc, "[]", 2);
		int aux = 1;
		write(sc, &aux, sizeof(int));
		write(sc, &file[i], 1);
		//printf("escric: %c\n", file[i]);
		if (unitatPercentatge * j == i){
			msg = sprintf(buff, "Sending data... %d %c complete.\n", j*10, '%');
			write(1, buff, msg);
			j++;
		}
		i++;
	}

    close(fd_file);
	free(file);
	free(md5sum);
	return 0;

}

int socketClient(char *ip, int port){
	struct sockaddr_in clientConfig;
	sc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sc < 0){
		write(1, ERR_CREATE_THREAD, strlen(ERR_CREATE_THREAD));
		return -1;
	}else{
		memset(&clientConfig, 0, sizeof(clientConfig));

		clientConfig.sin_family = AF_INET;
        clientConfig.sin_port = port;
        clientConfig.sin_addr.s_addr = inet_addr(ip);

		if(connect(sc, (void *) &clientConfig, sizeof (clientConfig)) < 0){
			write(1, ERR_CONN_LIONEL, strlen(ERR_CONN_LIONEL));
			return -1;
		}

		return sc;
	}
}

void checkFiles(){
	char aux = (char) t_info.name[9];
	char* aux2 = (char *) malloc (16 * sizeof(char));
	//aux2 = "files";
	char * files = "files";
	strcpy(aux2, files);
	aux2[5] = aux;
	aux2[6] = '/';
	aux2[7] = '\0';
	PATH = aux2;

	DIR *dr = opendir(PATH);
	struct dirent *de;
	int file_count = 0;
	char buff[300];

	//consultem si hi ha algun fitxer nou a enviar
    if (dr == NULL){
        write(1, ERR_FOLDER, strlen(ERR_FOLDER));
    }else{
    	write(1, "Testing files...\n", strlen("Testing files...\n"));
    	while ((de = readdir(dr)) != NULL) {
		    //if (de->d_type == DT_REG) {
    		if (de->d_name[0] != '.'){
		        file_count++;
		    }
		}
		closedir(dr);
		
		dr = opendir(PATH);
		if (file_count <= 0){
			write(1, NO_FILES, strlen(NO_FILES));
			write(1, "\nWaiting...\n", strlen("\nWaiting...\n"));
		}else{
			while ((de = readdir(dr)) != NULL){
				if (de->d_name[0] != '.'){
					char type = 0x03;
					write(sc, &type, sizeof(char));
					write(sc, "[METADATA]", 10);
					char* ext = get_filename_ext(de->d_name);
					//en cas afirmatiu, actuem en base a sigui una imatge o un fitxer de dades
					if (strcmp(ext, "jpg") == 0){
						if(sendingImage(de->d_name) < 0){
							write(1, ERR_SENT, strlen(ERR_SENT));
						}else{
							write(1, SENT, strlen(SENT));
							write(1, "\nWaiting...\n", strlen("\nWaiting...\n"));
							sprintf(buff, "%s/%s", PATH, de->d_name);
                			remove(buff);
						}
					}else{
						if(sendingText(de->d_name) < 0){
							write(1, ERR_SENT, strlen(ERR_SENT));
						}else{
							write(1, SENT, strlen(SENT));
							write(1, "\nWaiting...\n", strlen("\nWaiting...\n"));
							sprintf(buff, "%s/%s", PATH, de->d_name);
                			remove(buff);		
						}
					}
				//free(ext);
				}
			}
		}
		closedir(dr);
	}	
	//free(dr);
	//free(de);
}

void sigAlarm(){
	checkFiles();
	alarm(t_info.checkTime);
}


void siginthandler() {
	char buff[100];
	//ficar el punter a carpeta com a variable global per a tancar-lo aqui en cas de que suspenguin el programa mentres esta fent la lectura?
	//lliberar la memoria dels punters a char tambe?
	char type = 0x02;
	write(sc, &type, sizeof(char));
	write(sc, "[]", 2);
	int aux = strlen(t_info.name);
	write(sc, &aux, sizeof(t_info.name));
	write(sc, t_info.name, strlen(t_info.name));

	close(sc);
	sprintf(buff, "Disconnecting %s.\n%c", t_info.name, '\0');
	write(1, buff, strlen(buff));
	exit(0);
}

void readConfigFile(char * fileName){
	int fd = -1;
	char buff[100];
	int msg;
	signal(SIGINT, siginthandler);

	//llegim el fitxer de configuracio
	fd = open(fileName, O_RDONLY);
	if (fd > 0){
		t_info.name = read_string(fd, '\n');
		t_info.checkTime = atoi(read_string(fd, '\n'));
		t_info.IP = read_string(fd, '\n');
		t_info.port = atoi(read_string(fd, EOF));

		msg = sprintf(buff, "Starting %s\n", t_info.name);
		write(1, buff, msg);
		write(1, CONN_MSG, strlen(CONN_MSG));

		//creem el socket per a transmetre informacio i comunico que em conecto
		sc = socketClient(t_info.IP, t_info.port);
		if (sc >= 0){
			char type = 0x01;
			write(sc, &type, sizeof(char));
			write(sc, "[]", 2);

			int aux = strlen(t_info.name);
			write(sc, &aux, sizeof(aux));
			write(sc, t_info.name, strlen(t_info.name));

			write(1, CONN_RDY, strlen(CONN_RDY));
			write(1, "\nWaiting...\n", strlen("\nWaiting...\n"));
			signal(SIGALRM, sigAlarm);
			alarm(t_info.checkTime);
			while(1){
				//TODO: llegir que el Lionel s'ha desconectat?

				read(sc, buff, 100);
				write(1, "Lionel died. Disconnecting Telescope...\n", strlen("Lionel died. Disconnecting Telescope...\n"));
				exit(0);
			}
		}else{
			write(1, ERR_CONN_LIONEL, strlen(ERR_CONN_LIONEL));
		}
	}else{
		write(1, ERR_FILE, strlen(ERR_FILE));
	}
}

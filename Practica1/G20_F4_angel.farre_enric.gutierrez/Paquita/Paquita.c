/*Angel Farre - angel.farre & Enric Gutierrez - enric.gutierrez*/

#include "../libs/utils.h"
/*
int numImatges = 0;
int midaImatges = 0;
int numDadesAstronomiques = 0;
int mitjanaContelacions = 0;
int totalContelacions = 0;

void treatData(char* file, char* flag, char* filesize){
	char buff[100];
	printf("flag: %s\n", flag);
	printf("filesize: %s\n", filesize);
	if (strcmp(flag, "1") == 0){
		printf("file: %s\n", file);
		Last_Astronomical_Data lad = treat_astronomical_data(file);
		numDadesAstronomiques++;
		totalContelacions = lad.numConst;
		mitjanaContelacions = totalContelacions/numDadesAstronomiques;
		printf("\n\nconst: %d\n", lad.numConst);
		printf("density: %f\n", lad.avgDensity);
		printf("max: %f\n", lad.maxMagnitude);
		printf("min: %f\n\n", lad.minMagnitude);
	}else{
		int aux = atoi(filesize);
		midaImatges = midaImatges + aux;
		numImatges++;
		sprintf(buff, "\nTenim un total de %d imatges amb una mida total de %dKB\n\n", numImatges, midaImatges);
		write(1, buff, strlen(buff));
	}
	//kill(getpid(), 0);
}
*/

struct mail_msg{
	long type;
	//int numDadesAstronomiques;
	//int mitjanaContelacions;
	int numConst;
	float avgDensity;
	float maxMagnitude;
	float minMagnitude;
	//int numImatges;
	int midaImatges;
};


int numFitxers = 0;
int numImatges = 0;
int midaTotalImatges = 0;
int totalConst = 0;
int mitjanaContelacions = 0;


void checkMailbox(int mailBox){
	struct mail_msg msg = {0, 0, 0, 0, 0, 0};

	if (msgrcv(mailBox, &msg, sizeof(msg), 0, 0) < 0){
		write(1, ERR_MAIL_READ, strlen(ERR_MAIL_READ));
	}else{
		int message;
		char buff[100];
		write(1, PQT_DATA, strlen(PQT_DATA));
		if (msg.midaImatges == -1){
			//TXT
			numFitxers++;
			totalConst = totalConst + msg.numConst;
			mitjanaContelacions = (totalConst)/(numFitxers);
			message = sprintf(buff, "\tNumber of astrinomical data files: %d\n", numFitxers);
			write(1, buff, message);
			message = sprintf(buff, "\tAverage: %d\n", mitjanaContelacions);
			write(1, buff, message);
			message = sprintf(buff, "\tNumber of Constelations %d\n", msg.numConst);
			write(1, buff, message);
			message = sprintf(buff, "\tAvergage Density: %f\n", msg.avgDensity);
			write(1, buff, message);
			message = sprintf(buff, "\tMaximum magnitude: %f\n", msg.maxMagnitude);
			write(1, buff, message);
			message = sprintf(buff, "\tMinimum magnitude: %f\n\n", msg.minMagnitude);
			write(1, buff, message);
		}else{
			//Image
			numImatges++;
			midaTotalImatges = midaTotalImatges + msg.midaImatges;
			message = sprintf(buff, "\tNumber of images: %d\n", numImatges);
			write(1, buff, message);
			message = sprintf(buff, "\tTotal size of images: %d\n\n", midaTotalImatges);
			write(1, buff, message);
		}
	}
}

int main(int argc, char *argv[]){
	key_t mailKey = ftok("../Paquita/Paquita.c", 12);
	if (mailKey == (key_t) -1)
		write(1, "Error mailBox key on Paquita!", strlen("Error mailBox key on Paquita!"));
	
	int mailBox = msgget(mailKey, 0600 | IPC_CREAT);

	if (mailBox < 0){
		write(1, ERR_MAIL, strlen(ERR_MAIL));
		exit(0);
	}

	while(1){
		checkMailbox(mailBox);
	}
	return 0;
}

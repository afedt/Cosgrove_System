/*Angel Farre - angel.farre & Enric Gutierrez - enric.gutierrez*/

#include "utils.h"

char *get_string() {
    char *aux = (char *) malloc(1);
    int i = 0;

    do {
        read(0, &aux[i], 1);
        i++;
        aux = realloc(aux, i + 1);

    } while(aux[i - 1] != '\n');

    aux[i - 1] = '\0';

/*
    char aux2[i];
    for (int j = 0; j < i; ++j){
        aux2[j] = aux[j];
    }
    free(aux);
    */

    return aux;
}

char* read_string(int fd, char end) {
    char *buffer = (char *) malloc(1);
    int i = 0;

    while ((read(fd, &buffer[i], 1)) > 0 && buffer[i++] != end)
        buffer = (char *) realloc(buffer, i + 1);

    buffer[i - 1] = '\0';
    //free(buffer);

/*
    char aux2[i];
    for (int j = 0; j < i; ++j){
        aux2[j] = buffer[j];
    }
    free(buffer);
*/
    return buffer;
}

char *get_filename_ext(char *filename) {
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

int fsize(const char *filename) {
    struct stat st;

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1;
}

Data get_metadata_fields (char* string){
    Data d;
    int i = 0;
    char *aux = (char *) malloc(1);

    while (string[i] != '[' && string[i] != '\0'){
        aux = (char *) realloc(aux, (i + 1));
        aux[i] = string[i];
        i++;
    }
    aux[i] = '\0';
    d.type = aux[3];

    free(aux);

    i++;

    int j = 0;
    aux = (char *) malloc(1);
    while (string[i] != ']' && string[i] != '\0'){
        aux = (char *) realloc(aux, j + 1);
        aux[j] = string[i];
        i++;
        j++;
    }
    aux[j] = '\0';
    d.header = aux;

    free(aux);

    i++;

    j = 0;
    aux = (char *) malloc(1);
    while (string[i] != '[' && string[i] != '\0'){
        aux = (char *) realloc(aux, j + 1);
        aux[j] = string[i];
        i++;
        j++;
    }
    aux[j] = '\0';
    d.length = atoi(aux);

    free(aux);

    i++;

    j = 0;
    aux = (char *) malloc(1);
    while (string[i] != ']' && string[i] != '\0'){
        aux = (char *) realloc(aux, j + 1);
        aux[j] = string[i];
        i++;
        j++;
    }
    aux[j] = '\0';
    d.data = aux;
    free(aux);

    return d;
}

File_Info get_metadata_data(char* string, int size){
    File_Info fInfo;

    int i = 0;
    char *aux = (char *) malloc(1);

    while (string[i] != '&' && string[i] != '\0'){
        aux = (char *) realloc(aux, (i + 1));
        aux[i] = string[i];
        i++;
    }
    aux[i] = '\0';
    fInfo.extension = aux;

    i++;

    int j = 0;
    aux = (char *) malloc(1);
    while (string[i] != '&' && string[i] != '\0'){
        aux = (char *) realloc(aux, j + 1);
        aux[j] = string[i];
        i++;
        j++;
    }
    aux[j] = '\0';
    fInfo.fileLength = atoi(aux);

    i++;

    j = 0;
    aux = (char *) realloc(aux, j + 1);
    while (string[i] != '\0' && i < size){
        aux = (char *) realloc(aux, j + 1);
        aux[j] = string[i];
        i++;
        j++;
    }
    aux[j] = '\0';
    fInfo.creationDate = aux;
    //free(aux);

    return fInfo;
}

char* retrieve_actual_data(char* data){
    int i = 0;
    while(data[i] != '['){
        i++;
    }
    i++;
    while(data[i] != '['){
        i++;
    }
    i++;

    int j = 0;
    while(data[i] != '\0'){
        data[j] = data[i];
        i++;
        j++;
    }
    data[j - 1] = '\0';
    return data;
}


Last_Astronomical_Data treat_astronomical_data(char* string, int size){
    Last_Astronomical_Data lad;
    int i = 0;
    char *aux = (char *) malloc(1);
    int k = 0;
    int cons = 0;
    float density = 0;
    float maxMagnitude = -100;
    float minMagnitude = 100;

    //printf("fitxer to parse: %s\n", string);
    //printf("size: %d\n", size);
    while(string[k] != '\0' && k < size){
        while (string[i] != ' '){
            //printf("nom[%d]: %c\n", i, string[i]);
            i++;
            //NOP
        }

        i++;

        int j = 0;
        aux = (char *) malloc(1);
        while (string[i] != ' '){
            aux = (char *) realloc(aux, j + 1);
            aux[j] = string[i];
            //printf("density[%d]: %c\n", i, string[i]);
            i++;
            j++;
        }
        aux[j] = '\0';
        //printf("\tdensity total: %s\n", aux);
        density = density + atof(aux);
        free(aux);
        i++;

        j = 0;
        aux = (char *) malloc(1);
        while (string[i] != '\n' && string[i] != '\0'){
            aux = (char *) realloc(aux, j + 1);
            aux[j] = string[i];
            //printf("magnitudes[%d]: %c\n", i, string[i]);
            i++;
            j++;
            if (j > 20){
                exit(0);
            }
        }
        aux[j] = '\0';
        //printf("\tmagnitude total: %s\n", aux);

        if (atof(aux) > maxMagnitude){
            maxMagnitude = atof(aux);
        }
        if (atof(aux) < minMagnitude){
            minMagnitude = atof(aux);
        }
        free(aux);
        k = i;
        k++;
        cons++;
    }

    lad.numConst = cons;
    lad.avgDensity = density/cons;
    lad.maxMagnitude = maxMagnitude;
    lad.minMagnitude = minMagnitude;
    //free(aux);

    return lad;
}

int fileSize(char* filepath){
/*
    int fd = open(filepath, O_RDONLY);

    int size = lseek(fd, 0, SEEK_END);

    close(fd);
    return size;
*/

    mem *m = malloc(sizeof(*m));
    int fd = open(filepath, O_RDONLY);

    off_t currentPos = lseek(fd, (size_t)0, SEEK_CUR);
    m->size = lseek(fd, (size_t)0, SEEK_END);
    lseek(fd, currentPos, SEEK_SET);
    //cursor al principi
    close(fd);
    int size = m->size;

    free(m);
    return(size);
}


char* get_md5sum(char* path, char * filename){
    int link[2];
    pid_t pid;
    char foo[4096];
    char aux[300];

    sprintf(aux, "%s%s%c", path, filename, '\0');

    //pipe
    if (pipe(link) == -1)
        write(1, ERR_PIPE, strlen(ERR_PIPE));

    //fork
    if ((pid = fork()) == -1)
        write(1, ERR_FORK, strlen(ERR_FORK));

    if ( pid == 0 ) {
        dup2 (link[1], STDOUT_FILENO);
        close(link[0]);
        close(link[1]);
        execl("/usr/bin/md5sum","md5sum", aux, (char *)0);
        write(1, ERR_EXECL, strlen(ERR_EXECL));
        kill(getpid(), SIGKILL);
        exit(9);
    } else {
        wait(NULL);
        kill(pid, 0);
        close(link[1]);
        read(link[0], foo, sizeof(foo));

        //printf("llegeixo foo: %s\n", foo);

        char* md5sum = (char *) malloc(32 * sizeof(char));

        size_t i = 0;
        while(i < 32){
            //printf("afegeixo el caracter %c index[%ld]\n", foo[i], i);
            md5sum[i] = foo[i];
        //    md5sum = (char *) realloc(md5sum, (i + 1));
            i++;
        }
/*
        i = 0;
        while(i < strlen(md5sum)){
            printf("tinc el caracter %c index[%ld]\n", md5sum[i], i);
            i++;
        }
*/
        //printf("return pre md5sum: %s de tamany %ld\n", md5sum, strlen(md5sum));
        //free(md5sum);
        return md5sum;
    }
}

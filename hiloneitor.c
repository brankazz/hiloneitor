//hiloneitor.c  # Crea hilos a partir de un archivo de texto

#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define TOOTLENGTH 501
#define IDLENGTH 19

int main(int argc, char *argv[])
{
    FILE* fd;
    unsigned int tootN = 0;
    unsigned int charI = 0;
    unsigned char tootCnt[TOOTLENGTH];
    unsigned char lastTootId[IDLENGTH];

    int sonId, status;

    if(argc != 2){
        fprintf(stderr, "Uso %s [fichero de texto]\n",argv[0]);
        exit(1);
    }

    if(!(fd = fopen(argv[1], "r"))){
        fprintf(stderr, "ERROR: No se pudo abrir <%s>\n",argv[1]);
        exit(2);
    }



    unsigned char c;
    while((c = fgetc(fd)) != EOF){
        tootCnt[charI] = c;
        charI++;
        if(((charI == TOOTLENGTH-1)||(c == '\n'))&&(charI>1)){     //Toot finalizado. Lanzar toot
            tootCnt[charI] = 0;                         //Cierra el string
            fprintf(stderr,"%s %d\n",tootCnt,charI);
            if(tootN==0){                                 //Primer toot del hilo
                tootN++;
                int pfd[2];
                if (pipe(pfd) == -1) fprintf(stderr, "ERROR: No se pudo realizar el pipe\n");
                switch(sonId = fork()){                 //Abre un nuevo proceso
                    case -1: fprintf(stderr,"ERROR: No se pudo realizar el fork\n");
                        break;
                    case 0:
                        if(close(1) == -1) fprintf(stderr,"ERROR: No se pudo cerrar la salida\n");
                        if(dup(pfd[1]) != 1) fprintf(stderr, "ERROR: No se abrio el pipe de salida en la posicion correcta\n");
                        execlp("toot", "toot", "post", tootCnt, NULL);
                        fprintf(stderr, "ERROR: No se ha podido lanzar <toot>\nEjecute\n\t> sudo apt install toot\n para instalar toot en el sistema\n");
                        break;
                    default:
                        if(close(0) == -1) fprintf(stderr, "ERROR: No se pudo cerrar la entrada");
                        if(dup(pfd[0]) != 0) fprintf(stderr, "ERROR: No se abrio el pipe de entrada en la posicion correcta\n");
                        memset(lastTootId, 'a',IDLENGTH);
                        unsigned char idc;
                        while((idc = getchar())!= 10){
                            unsigned int i;
                            for(i = 0; i < IDLENGTH - 2; i++) lastTootId[i] = lastTootId[i+1];
                            lastTootId[IDLENGTH - 2] = idc;
                        }
                        lastTootId[IDLENGTH - 1] = 0;
                        fprintf(stderr,"ID toot emitido: %s\n\n\n",lastTootId);
                        wait(&status);
                        charI = 0;
                }
            }else{
                tootN++;
                int pfd[2];
                if (pipe(pfd) == -1) fprintf(stderr, "ERROR: No se pudo realizar el pipe\n");
                switch(sonId = fork()){
                    case -1: fprintf(stderr,"ERROR: No se pudo realizar el fork\n");
                        break;
                    case 0:
                        if(close(1) == -1) fprintf(stderr,"ERROR: No se pudo cerrar la salida\n");
                        if(dup(pfd[1]) != 1) fprintf(stderr, "ERROR: No se abrio el pipe de salida en la posicion correcta\n");
                        execlp("toot", "toot", "post", "-r", lastTootId, tootCnt, NULL);
                        fprintf(stderr, "ERROR: No se ha podido lanzar <toot>\nEjecute\n\t> sudo apt install toot\n para instalar toot en el sistema\n");
                        break;
                    default:
                        if(close(0) == -1) fprintf(stderr, "ERROR: No se pudo cerrar la entrada");
                        if(dup(pfd[0]) != 0) fprintf(stderr, "ERROR: No se abrio el pipe de entrada en la posicion correcta\n");
                        memset(lastTootId, 'a',IDLENGTH);
                        unsigned char idc;
                        while((idc = getchar())!= 10){
                            unsigned int i;
                            for(i = 0; i < IDLENGTH - 2; i++) lastTootId[i] = lastTootId[i+1];
                            lastTootId[IDLENGTH - 2] = idc;
                        }
                        lastTootId[IDLENGTH - 1] = 0;
                        fprintf(stderr,"ID toot emitido: %s\n\n\n",lastTootId);
                        wait(&status);
                        charI = 0;
                }
            }
        }
    }
    fclose(fd);
    return 0;
}

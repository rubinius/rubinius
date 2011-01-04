#include "test.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>


int main(int argc, char * argv[])
{
    char b[100];
    char fexe[100];
    char fpass[100];
    char ffail[100];
    char fres[100];
    char fnob[100];
    char cmd[100];
    char *e;
    struct stat st;
    DIR *dir;
    struct dirent *ent;
    
    if (argc!=2) {
        printf("usage: runall <dir>\n");
        return 0; //EXIT_FAILURE;
    }
    dir = opendir (argv[1]);
    if (dir != NULL) {
        while ((ent = readdir (dir)) != NULL) {
             strcpy(b,  ent->d_name);
             if (strstr(b,"runall.c")) {
                /* skip myself */
                continue;
            }
            e = strchr(b,'.');
            if (e) {
                *e ++ = '\0';
                if (strcmp(e,"c")==0) {
                    sprintf(fexe,"%s.exe",b);
                    sprintf(fpass,"%s.pass",b);
                    sprintf(ffail,"%s.fail",b);
                    sprintf(fres,"%s.res",b);
                    sprintf(fnob,"%s.x",b);
                   
                    if (stat(fexe,&st)==0) { 
                        if ( (stat(fpass,&st)!=0) && (stat(ffail,&st)!=0)) {
                            sprintf(cmd,"%s 2> %s",fexe,fres);
                            if (system(cmd)) {
                                rename(fres,ffail);
                            } else {
                                rename(fres,fpass);
                            }
                        }
                    } else {
                        sprintf(cmd,"touch %s",fnob);
                        system(cmd);
                    }
                }
            }

        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("");
        return 0; //EXIT_FAILURE;
    }
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[]){

    pid_t pid;

    char* lastcommand[1024] = {"zip", "ebooks.zip"}; // cria o comando zip

    // cria um processo child por cada ficheiro por converter
    for(int i = 1; i < argc; i++){

        char *filename = (char*)malloc(sizeof(argv[i])-4);

        strcpy(filename,argv[i]);

        filename[strlen(argv[i])-4] = '\0'; // elimina a extensão '.txt' do ficheiro

        char *epub = (char*)malloc(sizeof(argv[i])+1);

        strcpy(epub,filename);

        strcat(epub,".epub"); // adiciona a extersão '.epub'

        lastcommand[i+1] = epub; // adiciona o ficheiro ao comando zip

        // cria o processo
        if ((pid = fork()) < 0) { // error
            perror("fork error");
            exit(EXIT_FAILURE);
        }   
        else if (pid > 0){
            /* Parent */
            if (waitpid(pid, NULL, 0) < 0){ // à espera do child
                fprintf(stderr, "Cannot wait for child: %s\n", strerror(errno));
            }
            printf("[pid%d] converting %s ...\n", pid, argv[i]);
        } 
        else {
            /* Child */
            char* command[1024] = {"pandoc", argv[i], "-o", epub, NULL};
            execvp("pandoc", command); // pandoc command
            return EXIT_SUCCESS;
        }
    }
    if(execvp("zip",lastcommand) == -1){ // zip command
        perror("Could not Zip");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
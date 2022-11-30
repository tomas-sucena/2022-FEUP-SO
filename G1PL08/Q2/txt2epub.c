#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// função auxiliar usada para converter os ficheiros '.txt' para '.epub'
int convert_to_epub(char* txt, char* epub){
    char* command[1024] = {"pandoc", txt, "-o", epub, NULL};

    // converter o ficheiro para '.epub'
    if (execvp("pandoc", command) < 0){
        char* error_msg = (char*) malloc(64 * sizeof(char));
        sprintf(error_msg, "Error! Could not convert %s to %s", txt, epub);

        perror(error_msg);

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]){
    // verificar se há argumentos suficientes
    if (argc < 2){
        perror("Error! Not enough arguments");
        return EXIT_FAILURE;
    }

    char* lastcommand[1024] = {"zip", "ebooks.zip"}; // comando zip

    // criar os processos
    for(int i = 1; i < argc; i++){
        char* filename = (char*) malloc(sizeof(argv[i]));

        // eliminar a extensão '.txt' do ficheiro
        strcpy(filename, argv[i]);
        filename[strlen(filename) - 4] = '\0'; 

        // adicionar a extensão '.epub'
        char *epub = (char*) malloc(sizeof(argv[i]) + 1);
        strcpy(epub, filename);
        strcat(epub,".epub"); 

        // adicionar o ficheiro ao comando zip
        lastcommand[i + 1] = epub; 

        pid_t pid = fork();

        if (pid < 0){ // error
            perror("fork error");
            return EXIT_FAILURE;
        }   
        else if (pid == 0){
            /* Child */
            return convert_to_epub(argv[i], epub);
        }
        else{
            /* Parent */
            printf("[pid%d] converting %s ...\n", pid, argv[i]);
        }
    }

    // esperar que os processos child terminem a conversão
    for (int i = 1; i < argc; i++) {
        if (waitpid(-1, NULL, 0) < 0){
            perror("Error! Could not wait for children processes");
            return EXIT_FAILURE;
        }
    }

    // criar o ficheiro '.zip'
    if (execvp("zip", lastcommand) < 0){
        perror("Error! Could not Zip");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
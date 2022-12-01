#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>

#define READ 0
#define WRITE 1

/* VARIÁVEIS GLOBAIS */
static volatile sig_atomic_t running = 1;
int n = 0;
char* pipename;
pid_t pid;

// função auxiliar usada para interromper o ciclo infinito
static void sig_handler(int sig){
    (void) sig;
    running = 0;

    // terminar os child processes
    if (pid == 0){
        exit(0);
    }

    killpg(pid, SIGKILL);

    // eliminar os named pipes
    for (int i = 1; i <= n; i++){
        int next = (i == n) ?  1 : i + 1;
        
        sprintf(pipename, "pipes/pipe%dto%d", i, next);

        unlink(pipename);
    }

    exit(0);
}

// função auxiliar usada para descobrir o nº de dígitos de um inteiro
int int_digits(int n){
    return (int) log10((double) n) + 1;
}

// função auxiliar usada para gerar números pseudo-aleatórios entre 0 e 1
float rng(){
    return (float) rand() / RAND_MAX;
}

int main(int argc, char* argv[]){
    // verificar se há argumentos suficientes
    if (argc < 4){
        perror("Error! Not enough arguments");
        return EXIT_FAILURE;
    }

    /* ARGUMENTOS */
    n = atoi(argv[1]);
    float p = atof(argv[2]);
    float t = atof(argv[3]);

    // verificar se os argumentos são válidos
    if (n <= 1){
        perror("Error! Number of processes must be greater than 1");
        return EXIT_FAILURE;
    }
    else if (p < 0 || p > 1){
        perror("Error! Probability must be a number between 0 and 1");
        return EXIT_FAILURE;
    }
    else if (t < 0){
        perror("Error! Time (seconds) cannot be negative");
        return EXIT_FAILURE;
    }

    int fd[2]; // file descriptor
    int MAX_PIPENAME_SIZE = 12 + 2 * int_digits(n);
    pipename = (char*) malloc(MAX_PIPENAME_SIZE * sizeof(char));

    signal(SIGINT, sig_handler);

    // criar os named pipes
    for (int i = 1; i <= n; i++){
        int next = (i == n) ?  1 : i + 1;
        
        sprintf(pipename, "pipes/pipe%dto%d", i, next);

        mkfifo(pipename, 0666);
    }

    // criar os processos
    int p_num = 1; // nº do processo

    for (int i = 2; i <= n; i++){
        pid = fork();

        if (pid < 0){
            perror("Error! Could not fork");
            return EXIT_FAILURE;
        }
        else if (pid == 0){
            p_num = i;
            srandom(p_num); // inicializar a seed

            goto r; // r -> read
        }
    }

    // criar o token
    long token = 0;
    
    int prev = n,
        next = (n == 1) ? 1 : 2;

    goto w; // w -> write
    
    // passar o token
r:  while (running){
        // LEITURA
        prev = (p_num == 1) ? n : p_num - 1; // processo anterior
            
        sprintf(pipename, "pipes/pipe%dto%d", prev, p_num);
        fd[READ] = open(pipename, O_RDONLY);

        if (fd[READ] < 0){ // caso não dê para abrir o pipe
            char* error_msg = (char*) malloc(64 * sizeof(char));
            sprintf(error_msg, "Error! Could not read from %s", pipename);

            perror(error_msg);

            return EXIT_FAILURE;
        }

        read(fd[READ], &token, sizeof(token));
        close(fd[READ]);

        // bloquear o envio do token
w:      if (p >= rng()){
            printf("[p%d] lock on token (val = %ld)\n", p_num, token);
            sleep(t);
            printf("[p%d] unlock token\n", p_num);
        }

        // ESCRITA
        next = (p_num == n) ? 1 : p_num + 1; // processo seguinte

        sprintf(pipename, "pipes/pipe%dto%d", p_num, next);
        fd[WRITE] = open(pipename, O_WRONLY);

        if (fd[WRITE] < 0){ // caso não dê para abrir o pipe
            char* error_msg = (char*) malloc(64 * sizeof(char));
            sprintf(error_msg, "Error! Could not write in %s", pipename);

            perror(error_msg);

            return EXIT_FAILURE;
        }

        token++; // incrementar o token
        write(fd[WRITE], &token, sizeof(token));
        close(fd[WRITE]);
    }
    
    return EXIT_SUCCESS;
}
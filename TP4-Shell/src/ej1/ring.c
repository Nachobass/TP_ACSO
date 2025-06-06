#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char **argv) {
    int start, n, buffer[1];

    if (argc != 4) {
        printf("Uso: anillo <n> <c> <s> \n");
        exit(EXIT_FAILURE);
    }

    n = atoi(argv[1]);
    buffer[0] = atoi(argv[2]);
    start = atoi(argv[3]);

    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], start);

    int pipes[n][2];        // ring de pipes
    int pipe_padre[2];      // ultimo proceso → padre

    for (int i = 0; i < n; i++)
        if (pipe(pipes[i]) == -1) { perror("pipe"); exit(EXIT_FAILURE); }

    if (pipe(pipe_padre) == -1) { perror("pipe"); exit(EXIT_FAILURE); }

    int ultimo = (start - 1 + n) % n;

    for (int i = 0; i < n; i++) {
        if (fork() == 0) {
            // cerrar todos los pipes innecesarios
            for (int j = 0; j < n; j++) {
                if (j != i) close(pipes[j][0]);
                if (j != (i + 1) % n) close(pipes[j][1]);
            }
            close(pipe_padre[0]); // solo el padre lee

            int val;
            read(pipes[i][0], &val, sizeof(int));
            printf("Proceso %d recibió: %d\n", i, val);
            val++;

            if (i == ultimo) {
                write(pipe_padre[1], &val, sizeof(int));
            } else {
                write(pipes[(i + 1) % n][1], &val, sizeof(int));
            }
            exit(0);
        }
    }

    // proceso padre
    for (int i = 0; i < n; i++) {
        close(pipes[i][0]);
        if (i != start) close(pipes[i][1]);
    }
    close(pipe_padre[1]); // el padre no escribe en pipe_padre

    write(pipes[start][1], buffer, sizeof(int));

    int result;
    read(pipe_padre[0], &result, sizeof(int));
    printf("Resultado final recibido en el padre: %d\n", result);

    for (int i = 0; i < n; i++)
        wait(NULL);

    return 0;
}
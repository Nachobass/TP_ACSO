#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// int main(int argc, char **argv)
// {	
// 	int start, status, pid, n;
// 	int buffer[1];

// 	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
//     /* Parsing of arguments */
//     n = atoi(argv[1]);
//     buffer[0] = atoi(argv[2]);
//     start = atoi(argv[3]);

//     printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], start);
    
//    	/* You should start programming from here... */
//     int pipes[n+1][2]; // Se necesita un pipe extra para que el último envíe al padre

//     // Crear todos los pipes
//     for (int i = 0; i <= n; i++) {
//         if (pipe(pipes[i]) == -1) {
//             perror("pipe");
//             exit(EXIT_FAILURE);
//         }
//     }

//     for (int i = 0; i < n; i++) {
//         pid = fork();
//         if (pid == 0) {
//             // Cierre de extremos no usados
//             for (int j = 0; j <= n; j++) {
//                 if (j != i) close(pipes[j][0]);     // cerrar lectura no usada
//                 if (j != i + 1) close(pipes[j][1]); // cerrar escritura no usada
//             }

//             int val;
//             read(pipes[i][0], &val, sizeof(int));
//             printf("Proceso %d recibió: %d\n", i, val);
//             val++;
//             write(pipes[i+1][1], &val, sizeof(int));
//             exit(0);
//         }
//     }

//     // Proceso padre
//     for (int i = 0; i <= n; i++) {
//         if (i != start) close(pipes[i][1]);  // escribe solo en el pipe[start]
//         if (i != n) close(pipes[i][0]);      // lee solo del último pipe
//     }

//     // Padre inicia el mensaje
//     write(pipes[start][1], buffer, sizeof(int));

//     // Padre espera el resultado del último proceso
//     int result;
//     read(pipes[n][0], &result, sizeof(int));
//     printf("Resultado final recibido en el padre: %d\n", result);

//     // Espera a todos los hijos
//     for (int i = 0; i < n; i++)
//         wait(&status);

//     return 0;
// }


int main(int argc, char **argv)
{	
	int start, status, pid, n;
	int buffer[1];

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
    /* Parsing of arguments */
    n = atoi(argv[1]);
    buffer[0] = atoi(argv[2]);
    start = atoi(argv[3]);

    pid_t parent_pid = getpid(); // Guardar pid del padre para imprimir una sola vez
    if (getpid() == parent_pid)
        printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], start);
    
    int pipes[n][2];  // Cada pipe conecta i → (i+1)%n
    int pipe_padre[2]; // Pipe para que último proceso envíe al padre

    for (int i = 0; i < n; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    if (pipe(pipe_padre) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int ultimo = (start - 1 + n) % n;

    for (int i = 0; i < n; i++) {
        pid = fork();
        if (pid == 0) {
            // Cerrar pipes no usados
            for (int j = 0; j < n; j++) {
                if (j != i) close(pipes[j][0]); // leer solo de pipe[i]
                if (j != (i + 1) % n) close(pipes[j][1]); // escribir solo a siguiente
            }
            close(pipe_padre[0]); // padre solo lee

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


    // Proceso padre
    for (int i = 0; i < n; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    close(pipe_padre[1]); // el padre solo lee de este pipe

    // Padre inicia la comunicación
    write(pipes[start][1], buffer, sizeof(int));

    int result;
    read(pipe_padre[0], &result, sizeof(int));
    printf("Resultado final recibido en el padre: %d\n", result);

    // Esperar a todos los hijos
    for (int i = 0; i < n; i++)
        wait(&status);

    return 0;
}

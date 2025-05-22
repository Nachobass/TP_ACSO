// #include <sys/types.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/wait.h>


// int main(int argc, char **argv)
// {	
// 	int start, status, pid, n;
// 	int buffer[1];

// 	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
//     /* Parsing of arguments */
//   	/* TO COMPLETE */
//     printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], start);
    
//    	/* You should start programming from here... */
// }


// int main(int argc, char **argv) {
//     int start, status, pid, n;
//     int buffer[1];
//     int pipes[2][2];  // We need 2 pipes for the ring communication

//     if (argc != 4) {
//         printf("Uso: anillo <n> <c> <s> \n");
//         exit(0);
//     }

//     // Parse arguments
//     n = atoi(argv[1]);        // Number of child processes
//     buffer[0] = atoi(argv[2]); // Initial message value
//     start = atoi(argv[3]);    // Starting process number

//     printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], start);

//     // Create pipes for the ring
//     if (pipe(pipes[0]) < 0 || pipe(pipes[1]) < 0) {
//         perror("Error creating pipes");
//         exit(1);
//     }

//     // Create child processes
//     for (int i = 1; i <= n; i++) {
//         pid = fork();
        
//         if (pid == 0) {  // Child process
//             int value;
            
//             // Close unused pipe ends
//             if (i == 1) {
//                 close(pipes[0][1]);  // Close write end of first pipe
//                 close(pipes[1][0]);  // Close read end of second pipe
//             } else if (i == n) {
//                 close(pipes[1][1]);  // Close write end of second pipe
//                 close(pipes[0][0]);  // Close read end of first pipe
//             }

//             // If this is the starting process
//             if (i == start) {
//                 read(pipes[0][0], &value, sizeof(int));
//                 printf("Process %d received %d\n", i, value);
//                 value++;
//                 write(pipes[1][1], &value, sizeof(int));
//             } else {
//                 read(pipes[0][0], &value, sizeof(int));
//                 printf("Process %d received %d\n", i, value);
//                 value++;
//                 write(pipes[1][1], &value, sizeof(int));
//             }
            
//             exit(0);
//         }
//     }

//     // Parent process
//     if (start == 0) {
//         write(pipes[0][1], buffer, sizeof(int));
//     }
    
//     // Wait for the message to complete the ring
//     read(pipes[1][0], buffer, sizeof(int));
//     printf("Final value after completing the ring: %d\n", buffer[0]);

//     // Wait for all children to finish
//     while (wait(&status) > 0);
    
//     return 0;
// }



// #include <sys/types.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/wait.h>

// int main(int argc, char **argv) {
//     int start, status, pid, n;
//     int buffer[1];

//     if (argc != 4) {
//         printf("Uso: anillo <n> <c> <s> \n");
//         exit(0);
//     }

//     // Parseo de argumentos
//     n = atoi(argv[1]);         // cantidad de procesos
//     buffer[0] = atoi(argv[2]); // mensaje inicial
//     start = atoi(argv[3]);     // proceso que inicia

//     printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], start);

//     // Crear n pipes para el anillo
//     int pipes[n][2];
//     for (int i = 0; i < n; i++) {
//         if (pipe(pipes[i]) < 0) {
//             perror("pipe");
//             exit(1);
//         }
//     }

//     // Crear procesos hijos
//     for (int i = 0; i < n; i++) {
//         if ((pid = fork()) == 0) {
//             // Cada hijo cierra todos los pipes excepto el suyo
//             for (int j = 0; j < n; j++) {
//                 if (j != (i + n - 1) % n)
//                     close(pipes[j][0]); // leer de anterior
//                 if (j != i)
//                     close(pipes[j][1]); // escribir al siguiente
//             }

//             int val;
//             read(pipes[(i + n - 1) % n][0], &val, sizeof(int));
//             printf("Proceso %d recibió %d\n", i, val);
//             val++;
//             write(pipes[i][1], &val, sizeof(int));

//             exit(0);
//         }
//     }

//     // Proceso padre
//     // Cierra todos los pipes excepto los que usa para enviar y recibir
//     for (int i = 0; i < n; i++) {
//         if (i != start) close(pipes[i][1]); // solo escribe en start
//         if (i != (start + n - 1) % n) close(pipes[i][0]); // solo lee desde el anterior a start
//     }

//     // Envía mensaje inicial al pipe de entrada del proceso start
//     write(pipes[start][1], buffer, sizeof(int));

//     // Espera mensaje final desde el último pipe del anillo
//     read(pipes[(start + n - 1) % n][0], buffer, sizeof(int));
//     printf("Mensaje final después de completar el anillo: %d\n", buffer[0]);

//     // Espera que terminen todos los hijos
//     while (wait(&status) > 0);

//     return 0;
// }


#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
    int start, c, n;
    int buffer[1];

    if (argc != 4) {
        printf("Uso: anillo <n> <c> <s> \n");
        exit(EXIT_FAILURE);
    }

    n = atoi(argv[1]);      // cantidad de procesos hijos
    buffer[0] = atoi(argv[2]); // valor inicial
    start = atoi(argv[3]);  // proceso que inicia

    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], start);

    int pipes[n+1][2]; // pipes[n] conecta el último hijo con el padre

    for (int i = 0; i <= n; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // Cierre de extremos no usados
            for (int j = 0; j <= n; j++) {
                if (j != i) close(pipes[j][0]); // cerrar lectura no propia
                if (j != i+1) close(pipes[j][1]); // cerrar escritura no al siguiente
            }

            int val;
            read(pipes[i][0], &val, sizeof(int));
            printf("Proceso %d recibió: %d\n", i, val);
            val++;
            write(pipes[i+1][1], &val, sizeof(int));
            exit(0);
        }
    }

    // PADRE

    // Cierra extremos que no usa
    for (int i = 0; i <= n; i++) {
        if (i != start) close(pipes[i][1]); // no escribe en otros pipes
        if (i != n) close(pipes[i][0]);     // no lee de otros pipes (excepto el último)
    }

    write(pipes[start][1], buffer, sizeof(int)); // escribe valor inicial
    int final;
    read(pipes[n][0], &final, sizeof(int));      // lee desde el último pipe
    printf("Resultado final recibido en el padre: %d\n", final);

    // Espera a los hijos
    for (int i = 0; i < n; i++)
        wait(NULL);

    return 0;
}

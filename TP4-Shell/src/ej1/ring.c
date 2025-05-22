#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char **argv)
{
	int start, status, pid, n;
	int buffer[1];
    if (argc != 4) { printf("Uso: anillo <n> <c> <s>\n"); exit(1);}

    int n = atoi(argv[1]);
    int msg = atoi(argv[2]);
    int start = atoi(argv[3]);

    int pipes[n][2];
    for (int i = 0; i < n; i++)
        pipe(pipes[i]);

    for (int i = 0; i < n; i++) {
        if (fork() == 0) {
            int in = pipes[(i + n - 1) % n][0];
            int out = pipes[i][1];
            while (1) {
                int val;
                read(in, &val, sizeof(int));
                val++;
                write(out, &val, sizeof(int));
            }
            exit(0); // nunca se alcanza
        }
    }

    // el proceso padre inicia el mensaje en el proceso start
    write(pipes[start][1], &msg, sizeof(int));

    // espera a que el mensaje recorra el anillo y vuelva
    int final;
    read(pipes[(start + n - 1) % n][0], &final, sizeof(int));
    printf("Mensaje final: %d\n", final);

    // termina hijos (opcional, porque no tienen salida en este diseño)
    for (int i = 0; i < n; i++)
        kill(0, SIGKILL);

    return 0;
}



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
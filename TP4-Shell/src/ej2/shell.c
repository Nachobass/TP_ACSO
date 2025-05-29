// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/wait.h>
// #include <string.h>

// #define MAX_COMMANDS 200

// int main() {

//     char command[256];
//     char *commands[MAX_COMMANDS];
//     int command_count = 0;

//     while (1) 
//     {
//         printf("Shell> ");
        
//         /*Reads a line of input from the user from the standard input (stdin) and stores it in the variable command */
//         fgets(command, sizeof(command), stdin);
        
//         /* Removes the newline character (\n) from the end of the string stored in command, if present. 
//            This is done by replacing the newline character with the null character ('\0').
//            The strcspn() function returns the length of the initial segment of command that consists of 
//            characters not in the string specified in the second argument ("\n" in this case). */
//         command[strcspn(command, "\n")] = '\0';

//         /* Tokenizes the command string using the pipe character (|) as a delimiter using the strtok() function. 
//            Each resulting token is stored in the commands[] array. 
//            The strtok() function breaks the command string into tokens (substrings) separated by the pipe character |. 
//            In each iteration of the while loop, strtok() returns the next token found in command. 
//            The tokens are stored in the commands[] array, and command_count is incremented to keep track of the number of tokens found. */
//         char *token = strtok(command, "|");
//         while (token != NULL) 
//         {
//             commands[command_count++] = token;
//             token = strtok(NULL, "|");
//         }

//         /* You should start programming from here... */
//         for (int i = 0; i < command_count; i++) 
//         {
//             printf("Command %d: %s\n", i, commands[i]);
//         }    
//     }
//     return 0;
// }


// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/wait.h>
// #include <string.h>

// #define MAX_COMMANDS 200

// void ejecutar_comandos_con_pipes(char *commands[], int count) {
//     int pipes[MAX_COMMANDS - 1][2];
//     pid_t pids[MAX_COMMANDS];

//     // Crear todos los pipes necesarios
//     for (int i = 0; i < count - 1; i++) {
//         if (pipe(pipes[i]) == -1) {
//             perror("pipe");
//             exit(EXIT_FAILURE);
//         }
//     }

//     for (int i = 0; i < count; i++) {
//         pids[i] = fork();

//         if (pids[i] == 0) { // Proceso hijo
//             // Redirigir entrada si no es el primer comando
//             if (i > 0) {
//                 dup2(pipes[i - 1][0], STDIN_FILENO);
//             }
//             // Redirigir salida si no es el último comando
//             if (i < count - 1) {
//                 dup2(pipes[i][1], STDOUT_FILENO);
//             }

//             // Cerrar todos los pipes en el hijo
//             for (int j = 0; j < count - 1; j++) {
//                 close(pipes[j][0]);
//                 close(pipes[j][1]);
//             }

//             // Separar comando y argumentos
//             char *args[64];
//             char *token = strtok(commands[i], " ");
//             int arg_count = 0;
//             while (token != NULL) {
//                 args[arg_count++] = token;
//                 token = strtok(NULL, " ");
//             }
//             args[arg_count] = NULL;

//             // Ejecutar comando
//             if (execvp(args[0], args) == -1) {
//                 perror("execvp");
//                 exit(EXIT_FAILURE);
//             }
//         }
//     }

//     // Cerrar todos los pipes en el padre
//     for (int i = 0; i < count - 1; i++) {
//         close(pipes[i][0]);
//         close(pipes[i][1]);
//     }

//     // Esperar a todos los hijos
//     for (int i = 0; i < count; i++) {
//         waitpid(pids[i], NULL, 0);
//     }
// }

// int main() {
//     char command[256];
//     char *commands[MAX_COMMANDS];
//     int command_count;

//     while (1) {
//         printf("Shell> ");
//         fflush(stdout);

//         if (fgets(command, sizeof(command), stdin) == NULL) {
//             printf("\n");
//             break;
//         }

//         command[strcspn(command, "\n")] = '\0';

//         // Salir si el usuario escribe 'exit'
//         if (strcmp(command, "exit") == 0)
//             break;

//         // Tokenizar por pipes
//         command_count = 0;
//         char *token = strtok(command, "|");
//         while (token != NULL && command_count < MAX_COMMANDS) {
//             commands[command_count++] = token;
//             token = strtok(NULL, "|");
//         }

//         // Ejecutar comandos con pipe
//         ejecutar_comandos_con_pipes(commands, command_count);
//     }

//     return 0;
// }




#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_COMMANDS 200

// === NUEVO: parser que respeta comillas ===
// void parse_args_con_comillas(char *input, char *args[]) {
//     int i = 0;
//     while (*input) {
//         while (isspace(*input)) input++;  // saltar espacios

//         if (*input == '\0') break;

//         if (*input == '"') {
//             input++;
//             args[i++] = input;
//             while (*input && *input != '"') input++;
//         } else {
//             args[i++] = input;
//             while (*input && !isspace(*input)) input++;
//         }

//         if (*input) {
//             *input = '\0';
//             input++;
//         }
//     }
//     args[i] = NULL;
// }
void parse_args_con_comillas(char *input, char *args[]) {
    int i = 0;

    while (*input) {
        while (isspace(*input)) input++;  // Saltar espacios

        if (*input == '\0') break;

        // Comillas simples o dobles
        if (*input == '"' || *input == '\'') {
            char quote = *input;
            input++; // saltar comilla inicial
            args[i++] = input;

            bool escape = false;
            while (*input) {
                if (*input == '\\' && !escape) {
                    escape = true;
                    input++;
                    continue;
                }
                if (*input == quote && !escape) {
                    break; // cerró la comilla
                }
                escape = false;
                input++;
            }

            if (*input != quote) {
                fprintf(stderr, "Error: comillas no cerradas\n");
                args[0] = NULL;
                return;
            }

            *input = '\0'; // terminar string
            input++;       // avanzar tras la comilla
        } else {
            args[i++] = input;
            while (*input && !isspace(*input)) input++;
            if (*input) {
                *input = '\0';
                input++;
            }
        }
    }

    args[i] = NULL;
}


// ==========================================

void ejecutar_comandos_con_pipes(char *commands[], int count) {
    int pipes[MAX_COMMANDS - 1][2];
    pid_t pids[MAX_COMMANDS];

    for (int i = 0; i < count - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < count; i++) {
        pids[i] = fork();
        if (pids[i] == 0) { // hijo
            if (i > 0) dup2(pipes[i - 1][0], STDIN_FILENO);
            if (i < count - 1) dup2(pipes[i][1], STDOUT_FILENO);

            for (int j = 0; j < count - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            char *args[64];
            parse_args_con_comillas(commands[i], args);  // usar parser que respeta comillas

            if (execvp(args[0], args) == -1) {
                perror("execvp");                               // ESTO ES LO QUE COMPARA EL TEST DE INVALIDO????
                exit(EXIT_FAILURE);
            }
        }
    }

    for (int i = 0; i < count - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < count; i++) {
        waitpid(pids[i], NULL, 0);
    }
}

int main() {
    char command[256];
    char *commands[MAX_COMMANDS];
    int command_count;

    while (1) {
        printf("Shell> ");
        fflush(stdout);

        if (fgets(command, sizeof(command), stdin) == NULL) {
            printf("\n");
            break;
        }

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0 || strcmp(command, "q") == 0)
            break;

        command_count = 0;
        char *token = strtok(command, "|");
        while (token != NULL && command_count < MAX_COMMANDS) {
            commands[command_count++] = token;
            token = strtok(NULL, "|");
        }

        if (strncmp(command, "cd", 2) == 0) {
            char *path = command + 2;
            while (isspace(*path)) path++;

            if (*path == '\0') {
                path = getenv("HOME");
            }

            if (chdir(path) != 0) {
                perror("cd");
            }
            continue;
        }

        ejecutar_comandos_con_pipes(commands, command_count);
    }

    return 0;
}

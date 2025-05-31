// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/wait.h>
// #include <string.h>
// #include <ctype.h>
// #include <stdbool.h>

// #define MAX_COMMANDS 200

// // === NUEVO: parser que respeta comillas ===
// #define MAX_ARGS 63

// bool parse_args_con_comillas(char *input, char *args[]) {
//     int i = 0;
//     while (*input) {
//         while (isspace(*input)) input++;
//         if (*input == '\0') break;

//         if (i >= MAX_ARGS) {
//             fprintf(stderr, "Error: exceso de argumentos\n");
//             return false;
//         }

//         if (*input == '"') {
//             input++;
//             args[i++] = input;
//             while (*input && *input != '"') input++;
//             if (*input != '"') {
//                 fprintf(stderr, "Error: comillas abiertas sin cerrar\n");
//                 return false;
//             }
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
//     return true;
// }



// // ==========================================

// void ejecutar_comandos_con_pipes(char *commands[], int count) {
//     int pipes[MAX_COMMANDS - 1][2];
//     pid_t pids[MAX_COMMANDS];

//     for (int i = 0; i < count - 1; i++) {
//         if (pipe(pipes[i]) == -1) {
//             perror("pipe");
//             exit(EXIT_FAILURE);
//         }
//     }

//     for (int i = 0; i < count; i++) {
//         pids[i] = fork();
//         if (pids[i] == 0) { // hijo
//             if (i > 0) dup2(pipes[i - 1][0], STDIN_FILENO);
//             if (i < count - 1) dup2(pipes[i][1], STDOUT_FILENO);

//             for (int j = 0; j < count - 1; j++) {
//                 close(pipes[j][0]);
//                 close(pipes[j][1]);
//             }

//             char *args[64];
//             // parse_args_con_comillas(commands[i], args);  // usar parser que respeta comillas
//             if (!parse_args_con_comillas(commands[i], args)) {
//                 exit(EXIT_FAILURE);  // error por comillas
//             }

//             if (strcmp(args[0], "exit") == 0) {
//                 exit(0);  // No terminar el shell padre, solo el hijo
//             }

//             if (execvp(args[0], args) == -1) {
//                 perror("execvp");                         
//                 exit(EXIT_FAILURE);
//             }
//         }
//     }

//     for (int i = 0; i < count - 1; i++) {
//         close(pipes[i][0]);
//         close(pipes[i][1]);
//     }

//     for (int i = 0; i < count; i++) {
//         waitpid(pids[i], NULL, 0);
//     }
// }


// // ================== MAIN ==================
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

//         if (strcmp(command, "exit") == 0 || strcmp(command, "q") == 0)
//             break;

//         // Chequeos de sintaxis inválida antes de tokenizar
//         if (command[0] == '|' || command[strlen(command) - 1] == '|') {
//             fprintf(stderr, "Error de sintaxis: pipe al inicio o al final\n");
//             continue;
//         }
//         if (strstr(command, "||") != NULL) {
//             fprintf(stderr, "Error de sintaxis: pipes consecutivos\n");
//             continue;
//         }

//         command_count = 0;
//         char *token = strtok(command, "|");
//         while (token != NULL && command_count < MAX_COMMANDS) {
//             commands[command_count++] = token;
//             token = strtok(NULL, "|");
//         }

//         if (strncmp(command, "cd", 2) == 0) {
//             char *path = command + 2;
//             while (isspace(*path)) path++;

//             if (*path == '\0') {
//                 path = getenv("HOME");
//             }

//             if (chdir(path) != 0) {
//                 perror("cd");
//             }
//             continue;
//         }

//         bool sintaxis_invalida = false;

//         for (int i = 0; i < command_count; i++) {
//             // Saltar espacios
//             while (isspace(*commands[i])) commands[i]++;
//             if (*commands[i] == '\0') {
//                 sintaxis_invalida = true;
//                 break;
//             }
//         }

//         if (sintaxis_invalida) {
//             fprintf(stderr, "Error de sintaxis: comando vacío entre pipes\n");
//             continue;
//         }

//         ejecutar_comandos_con_pipes(commands, command_count);
//     }

//     return 0;
// }



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

#define MAX_LINE 1024
#define MAX_CMDS 200
#define MAX_ARGS 63

// ─────────────── Funciones auxiliares ───────────────

int es_sintaxis_invalida(const char *input) {
    int len = strlen(input);
    if (len == 0 || input[0] == '|' || input[len - 1] == '|') return 1;
    for (int i = 0; i < len - 1; ++i) {
        if (input[i] == '|' && input[i + 1] == '|') return 1;
    }
    return 0;
}

char *trim_espacios(char *s) {
    while (isspace(*s)) s++;
    if (*s == 0) return s;
    char *end = s + strlen(s) - 1;
    while (end > s && isspace(*end)) *end-- = '\0';
    return s;
}

int dividir_por_pipes(char *input, char *comandos[]) {
    int count = 0;
    char *token = strtok(input, "|");
    while (token && count < MAX_CMDS) {
        comandos[count++] = trim_espacios(token);
        token = strtok(NULL, "|");
    }
    return count;
}

int parsear_argumentos(char *comando, char *argv[]) {
    int i = 0;
    while (*comando) {
        while (isspace(*comando)) comando++;
        if (*comando == '\0') break;

        if (i >= MAX_ARGS - 1) {
            fprintf(stderr, "Error: demasiados argumentos\n");
            return 0;
        }

        if (*comando == '"') {
            comando++;
            argv[i++] = comando;
            while (*comando && *comando != '"') comando++;
            if (*comando != '"') {
                fprintf(stderr, "Error: comillas sin cerrar\n");
                return 0;
            }
        } else {
            argv[i++] = comando;
            while (*comando && !isspace(*comando)) comando++;
        }

        if (*comando) {
            *comando = '\0';
            comando++;
        }
    }

    argv[i] = NULL;
    return 1;
}

// ─────────────── Ejecución ───────────────

void ejecutar_pipeline(char *comandos[], int num_cmds) {
    int pipes[MAX_CMDS - 1][2];
    pid_t procesos[MAX_CMDS];

    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_cmds; i++) {
        procesos[i] = fork();
        if (procesos[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (procesos[i] == 0) {  // hijo
            if (i > 0) dup2(pipes[i - 1][0], STDIN_FILENO);
            if (i < num_cmds - 1) dup2(pipes[i][1], STDOUT_FILENO);

            for (int j = 0; j < num_cmds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            char *argv[MAX_ARGS];
            if (!parsear_argumentos(comandos[i], argv)) exit(EXIT_FAILURE);

            if (strcmp(argv[0], "exit") == 0) exit(0);

            execvp(argv[0], argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < num_cmds; i++) {
        waitpid(procesos[i], NULL, 0);
    }
}

// ─────────────── Main ───────────────

int main() {
    char linea[MAX_LINE];
    char *comandos[MAX_CMDS];

    while (1) {
        printf("Shell> ");
        fflush(stdout);

        if (!fgets(linea, sizeof(linea), stdin)) {
            printf("\n");
            break;
        }

        linea[strcspn(linea, "\n")] = '\0';
        if (strcmp(linea, "exit") == 0 || strcmp(linea, "q") == 0) break;

        if (es_sintaxis_invalida(linea)) {
            fprintf(stderr, "Error de sintaxis\n");
            continue;
        }

        int num_cmds = dividir_por_pipes(linea, comandos);

        int vacio = 0;
        for (int i = 0; i < num_cmds; i++) {
            if (*comandos[i] == '\0') {
                vacio = 1;
                break;
            }
        }
        if (vacio) {
            fprintf(stderr, "Error: comando vacío entre pipes\n");
            continue;
        }

        if (strncmp(comandos[0], "cd", 2) == 0) {
            char *path = comandos[0] + 2;
            while (isspace(*path)) path++;
            if (*path == '\0') path = getenv("HOME");
            if (chdir(path) != 0) perror("cd");
            continue;
        }

        ejecutar_pipeline(comandos, num_cmds);
    }

    return 0;
}

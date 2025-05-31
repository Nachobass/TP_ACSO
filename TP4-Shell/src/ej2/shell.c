#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_COMMANDS 200

// === NUEVO: parser que respeta comillas ===
#define MAX_ARGS 63

bool parse_args_con_comillas(char *input, char *args[]) {
    int i = 0;
    while (*input) {
        while (isspace(*input)) input++;
        if (*input == '\0') break;

        if (i >= MAX_ARGS) {
            fprintf(stderr, "Error: exceso de argumentos\n");
            return false;
        }

        if (*input == '"') {
            input++;
            args[i++] = input;
            while (*input && *input != '"') input++;
            if (*input != '"') {
                fprintf(stderr, "Error: comillas abiertas sin cerrar\n");
                return false;
            }
        } else {
            args[i++] = input;
            while (*input && !isspace(*input)) input++;
        }

        if (*input) {
            *input = '\0';
            input++;
        }
    }

    args[i] = NULL;
    return true;
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
            // parse_args_con_comillas(commands[i], args);  // usar parser que respeta comillas
            if (!parse_args_con_comillas(commands[i], args)) {
                exit(EXIT_FAILURE);  // error por comillas
            }

            if (strcmp(args[0], "exit") == 0) {
                // exit(0);  // No terminar el shell padre, solo el hijo
            }

            if (execvp(args[0], args) == -1) {
                perror("execvp");                         
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


// ================== MAIN ==================
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

        // Chequeos de sintaxis inválida antes de tokenizar
        if (command[0] == '|' || command[strlen(command) - 1] == '|') {
            fprintf(stderr, "Error de sintaxis: pipe al inicio o al final\n");
            continue;
        }
        if (strstr(command, "||") != NULL) {
            fprintf(stderr, "Error de sintaxis: pipes consecutivos\n");
            continue;
        }

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

        bool sintaxis_invalida = false;

        for (int i = 0; i < command_count; i++) {
            // Saltar espacios
            while (isspace(*commands[i])) commands[i]++;
            if (*commands[i] == '\0') {
                sintaxis_invalida = true;
                break;
            }
        }

        if (sintaxis_invalida) {
            fprintf(stderr, "Error de sintaxis: comando vacío entre pipes\n");
            continue;
        }

        ejecutar_comandos_con_pipes(commands, command_count);
    }

    return 0;
}

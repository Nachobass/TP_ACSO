#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_COMMANDS 200

// === NUEVO: parser que respeta comillas ===
void parse_args_con_comillas(char *input, char *args[]) {
    int i = 0;
    while (*input) {
        while (isspace(*input)) input++;  // saltar espacios

        if (*input == '\0') break;

        if (*input == '"') {
            input++;
            args[i++] = input;
            while (*input && *input != '"') input++;
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

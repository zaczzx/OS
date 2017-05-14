#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE_LEN 256
#define RUNNING   1
#define FINISHED  0

typedef struct {
	pid_t pid;
	int is_background;
	int status;
	char *command;
} process_t;

// parse command from user
int parse_command(char command[], char *args[]) {
	char *p;
	int i = 0;
	command[strlen(command) - 1] = '\0';
	p = strtok(command, " ");
	if (p == NULL) {
		return 1;
	}
	args[i++] = strdup(p);
	p = strtok(NULL, " ");
	while (p) {
		args[i++] = strdup(p);
		p = strtok(NULL, " ");
	}
	return 0;
}

// check if this job js a background job
int is_background_job(char *args[]) {
	int i = 0;
	while (args[i] != NULL) {
		if (args[i + 1] == NULL && strncmp(args[i], "&", 1) == 0) {
			args[i] = NULL;
			return 1;
		}
		i++;
	}
	return 0;
}

// print the status of process
void print_process_status(process_t process[]) {
	int i;
	int index = 1;
	int need_print = 0;
	for (i = 0; i < SIZE_LEN; i++) {
		if (process[i].is_background && process[i].status == RUNNING) {
			need_print = 1;
		}
	}
	if (!need_print) {
		return;
	}
	// print running process
	printf("    Running:\n");
	for (i = 0; i < SIZE_LEN; i++) {
		if (process[i].is_background && process[i].status == RUNNING) {
			printf("        [%d] %s\n", index++, process[i].command);
		}
	}
	index = 1;
	// print finished process
	printf("    Finished:\n");
	for (i = 0; i < SIZE_LEN; i++) {
		if (process[i].is_background && process[i].status == FINISHED) {
			printf("        [%d] %s\n", index++, process[i].command);
		}
	}
}

// check if this command contain redirection
int contain_redirection(char *args[], char *inout[]) {
	int i = 0;
	int contain = 0;
	while (args[i] != NULL) {
		if (strncmp(args[i], "<", 1) == 0) {
			inout[0] = strdup(args[i + 1]);
			args[i] = NULL;
			args[i++] = NULL;
			contain = 1;
		}
		if (strncmp(args[i], ">", 1) == 0) {
			inout[1] = strdup(args[i + 1]);
			args[i] = NULL;
			args[i++] = NULL;
			contain = 1;
		}
		i++;
	}
	return contain;
}

// handle build in commands like cd, wait, exit.
int handle_build_in_commands(char *args[], process_t process[]) {
	char buffer[SIZE_LEN] = { 0 };
	// handle cd
	if (strncmp(args[0], "cd", 2) == 0 && args[1] != NULL) {
		if (chdir(args[1]) == -1) {
			printf("cd: %s: No such file or directory\n", args[1]);
		} else {
			getcwd(buffer, SIZE_LEN);
			printf("%s\n", buffer);
		}
		return 1;
	}
	// handle wait
	int found = 0;
	if (strncmp(args[0], "wait", 4) == 0 && args[1] != NULL) {
		int i;
		int index = 1;
		for (i = 0; i < SIZE_LEN; i++) {
			if (process[i].pid != 0) {
				if ((process[i].is_background)
						&& (process[i].status == RUNNING)) {
					if (atoi(args[1]) == index) {
						printf("waiting for %s\n", process[i].command);
						waitpid(process[i].pid, NULL, 0);
						process[i].status = FINISHED;
						// print finished process
						printf("    Finished:\n");
						for (i = 0; i < SIZE_LEN; i++) {
							if (process[i].is_background && process[i].status == FINISHED) {
								printf("        [%d] %s\n", index++, process[i].command);
							}
						}
						found = 1;
					}
					index++;
				}
			}
		}
		if (!found) {
			printf("Process not found.\n");
		}
	}
	// handle exit
	if (strncmp(args[0], "exit", 4) == 0) {
		exit(0);
	}
	return 0;
}

// handle pipes
int handle_pipes(char *args[], process_t process[]) {
	int i = 0, j, index = 0;
	int num = 0;
	int pipes[SIZE_LEN][2] = { 0 };
	while (args[i] != NULL) {
		if (strncmp(args[i], "|", 1) == 0) {
			num++;
		}
		i++;
	}
	index = 0;
	i = 0;
	if (num != 0) {
		j = 0;
		char *arg[SIZE_LEN / 2] = { NULL };
		while (args[i] != NULL) {
			if (strncmp(args[i], "|", 1) == 0) {
				args[j] = NULL;
				j = 0;
				pipe(pipes[index]);
				pid_t pid = fork();
				if (pid < 0) {
					fprintf(stderr, "Error on fork.\n");
					exit(1);
				} else if (pid == 0) {
					close(STDIN_FILENO);
					execvp(args[0], arg);
					exit(0);
				} else {
					int k;
					for (k = 0; k < SIZE_LEN; k++) {
						if (process[k].pid == 0) {
							process[k].pid = pid;
							process[k].is_background = 0;
							process[k].status = RUNNING;
							break;
						}
					}
				}
			}
			i++;
		}
	}
	return num;
}

int main(int argc, char *argv[]) {
	process_t process[SIZE_LEN];
	bzero(process, sizeof(process));
	int i, fd, nread;
	printf("wsh: ");
	char command[SIZE_LEN] = { '\0' };
	char buffer[SIZE_LEN] = { '\0' };
	while (fgets(command, 256, stdin) != NULL) {
		// parse command
		command[255] = '\0';
		char *args[SIZE_LEN / 2] = { NULL };
		if (parse_command(command, args)
				|| handle_build_in_commands(args, process)
				|| handle_pipes(args, process)) {
			printf("wsh: ");
			continue;
		}

		int is_background = is_background_job(args);
		char *inout[2] = { NULL };
		int contain_redict = contain_redirection(args, inout);
		int pipefd1[2], pipefd2[2];
		pipe(pipefd1);
		pipe(pipefd2);

		pid_t pid = fork();
		if (pid < 0) {
			fprintf(stderr, "Error on fork.\n");
			exit(1);
		} else if (pid == 0) {
			if (contain_redict) {
				close(pipefd1[1]);
				close(pipefd2[0]);

				close(STDIN_FILENO);
				dup2(pipefd1[0], STDIN_FILENO);
				close(pipefd1[0]);

				close(STDOUT_FILENO);
				dup2(pipefd2[1], STDOUT_FILENO);
				close(pipefd2[1]);
			}
			execvp(args[0], args);
			exit(0);
		} else {
			if (contain_redict) {
				close(pipefd1[0]);
				close(pipefd2[1]);
				if (inout[0] != NULL) {
					if ((fd = open(inout[0], O_RDONLY, 0)) == -1) {
						fprintf(stderr, "%s: Input file error\n", args[0]);
						continue;
					}
					while ((nread = read(fd, buffer, sizeof(buffer))) != 0) {
						if (nread == -1) {
							perror("failed in read");
							break;
						}
						if (write(pipefd1[1], buffer, nread) == -1) {
							perror("failed in write");
							break;
						}
					}
					close(fd);
					close(pipefd1[1]);
				}
				if (inout[1] != NULL) {
					int i = 0;
					if ((fd = open(inout[1], O_CREAT | O_WRONLY, 0666)) == -1) {
						fprintf(stderr, "%s: Output file error\n", args[0]);
						continue;
					}
					while ((nread = read(pipefd2[0], buffer, sizeof(buffer)))
							!= 0) {
						write(fd, buffer, nread);
					}
					close(fd);
					close(pipefd2[0]);
				}
			}
			// save this process
			for (i = 0; i < SIZE_LEN; i++) {
				if (process[i].pid == 0) {
					process[i].pid = pid;
					process[i].is_background = is_background;
					process[i].status = RUNNING;
					process[i].command = strdup(command);
					break;
				}
			}
			// wait for process
			for (i = 0; i < SIZE_LEN; i++) {
				if (process[i].pid != 0) {
					if (!process[i].is_background) {
						waitpid(process[i].pid, NULL, 0);
						process[i].pid = 0;
					} else {
						if (waitpid(process[i].pid, NULL, WNOHANG) != 0) {
							process[i].status = FINISHED;
						}
					}
				}
			}
			usleep(10000);
			print_process_status(process);
			fflush(stdout);
			fflush(stderr);
			printf("wsh: ");
		}
	}

	return 0;
}

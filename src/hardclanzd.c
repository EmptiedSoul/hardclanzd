#include "cstd.h"
#define LOG_FILE "/var/log/service.log"
#define PIDFILE_DIR "/run/service/"
int mode = -1;
char process[1024];
char pidfile_location[1024];
pid_t pid;
pid_t pidexec;
FILE* logfile;
FILE* pidfile;
void USR1handler(int sig){
	logfile = fopen(LOG_FILE, "a");
	fprintf(logfile, "[ superviser %i ] Catched signal: %i\n",getpid(), sig);
	signal(sig, USR1handler);
	fclose(logfile);
}
void TERMhandler(int sig){
	logfile = fopen(LOG_FILE, "a");
	fprintf(logfile, "[ superviser %i ] Catched murdering signal: %i; Exiting\n",getpid(), sig);
	fclose(logfile);
	exit(0);
}
void USR2handler(){
	logfile = fopen(LOG_FILE, "a");
	fprintf(logfile, "[ superviser %i ] stopping %s (%i)\n", getpid(), process, pidexec);
	fclose(logfile);
	kill(pidexec, SIGTERM);
	sleep(5);
	kill(pidexec, SIGKILL);
	remove(pidfile_location);
	exit(0);	
}
int main(int argc, char *argv[]){
	signal(SIGUSR1, USR1handler);
	signal(SIGUSR2, USR2handler);
	signal(SIGTERM, TERMhandler);
	switch (argc){
		case 1:
			printf("No args passed. Aborting\n");
			exit(1);
		case 2:
			printf("No executable passed. Aborting\n");
			exit(1);
	}
	if (strcmp(argv[1], "once") == 0) mode = 1;
	else if (strcmp(argv[1], "keep-alive") == 0) mode = 2;
	strcpy(process, argv[2]);
	strcpy(pidfile_location, PIDFILE_DIR);
	strcat(pidfile_location, process);
	switch (mode){
		case 1:
			switch (pid = fork()){
				case -1:
				       perror("fork");
				       exit(1);
				case 0:
				       logfile = fopen(LOG_FILE, "a");
				       pidexec = fork();
				       switch (pidexec){
					       case -1:
							fprintf(logfile, "Fork: %s\n", strerror(errno));
							fclose(logfile);
							exit(1);
					       case 0:
							fprintf(logfile, "[ superviser %i ] starting %s (%i)\n", getppid(), argv[2] , getpid());
							fclose(logfile);
							pidfile = fopen(pidfile_location, "w");
							fprintf(pidfile, "%i", getppid());
							fclose(pidfile);
							if (execl(argv[2], argv[2], NULL) == -1) {
							logfile = fopen(LOG_FILE, "a");
							fprintf(logfile, "Failed to execute %s: %s\n", argv[2], strerror(errno));
							fclose(logfile);
							exit(1);
							};
				       }
				       wait(0);
				       remove(pidfile_location);
				       exit(0);
				       };
			logfile = fopen(LOG_FILE, "a");
			fprintf(logfile,"[ superviser %i ] started\n", pid);
			fflush(logfile);   			       
			printf("[ superviser %i ] started\n", pid);
			exit(0);
		case 2:
			switch (pid = fork()){
				case -1:
				       perror("fork");
				       exit(1);
				case 0:
					while(true){
				       logfile = fopen(LOG_FILE, "a");
				       pidexec = fork();
				       switch (pidexec){
					       case -1:
							fprintf(logfile, "Fork: %s\n", strerror(errno));
							fclose(logfile);
							exit(1);
					       case 0:
							fprintf(logfile, "[ superviser %i ] starting %s (%i)\n", getppid(), argv[2] , getpid());
							fclose(logfile);
							pidfile = fopen(pidfile_location, "w");
							fprintf(pidfile, "%i", getppid());
							fclose(pidfile);
							if (execl(argv[2], argv[2], NULL) == -1) {
							logfile = fopen(LOG_FILE, "a");
							fprintf(logfile, "Failed to execute %s: %s\n", argv[2], strerror(errno));
							fclose(logfile);
							exit(1);
							};
				       }
				       wait(0);
				   }
				   	   remove(pidfile_location);
				       exit(0);
				       };
			logfile = fopen(LOG_FILE, "a");
			fprintf(logfile,"[ superviser %i ] started\n", pid);
			fflush(logfile); 	       
			printf("[ superviser %i ] spawned\n", pid);
			exit(0);
		default:
			puts("Unknown option");
			exit(1);
	}
	return 0;
}

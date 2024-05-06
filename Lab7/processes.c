#include <sys/stat.h> 
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
//#include <fcnt1.h> 
#include <dirent.h> 
#include <string.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include<unistd.h> 
#include <signal.h> 
int amountFiles = 0;
int amountProc = 1;
int maxAmountProc;
char fileName[255];
char filya[] = "/home/egor/Документы/ACTOS7_.txt";
pid_t pid;
pid_t pidMain;
int lenCurrF;
FILE* saved;

char MainDir[512] = {};
struct dirent* MainDirD;
typedef struct argument {
	int len1;
	int len2;
	DIR* curr;
	struct dirent data;
	struct stat dataFile;
	char Dir[];
} arg;

void find_(int* len1, int* len2, DIR* curr, struct dirent* data,
	struct stat* dataFile, char Dir[]) {
	// saved=fopen(filya, "w"); 
	while (curr != 0 && (data = readdir(curr)) != NULL) {
		char str[255] = {};
		char strName[255];
		strcpy(str, Dir);
		strcat(str, "/");
		strcat(str, data->d_name);
		stat(str, dataFile);
		// int res=(dataFile->st_mode) & 170000; 
		if ((dataFile->st_mode & 0x49) && S_ISREG(dataFile->st_mode) && 
               (dataFile->st_size >= *len1) && (dataFile->st_size <= *len2)){
			// saved=fopen(filya, "w"); 
			DIR* newCurr;
			char newDir[255] = {};
			strcpy(newDir, MainDir);
			newCurr = opendir(MainDir);
			lenCurrF = dataFile->st_blocks;
			fputs(str, saved);
			char strSize[30];
			printf("%s\n", str);
			sprintf(strSize, " size = %d", (int)dataFile->st_size);
			fputs(strSize, saved);
			fputs("\n", saved);
			amountFiles++;
		}
		if (S_ISDIR(dataFile->st_mode) && strcmp(data->d_name, ".")&& 
                 strcmp(data->d_name, "..")) {
			DIR* newCurr;
			char strName[255];
			strcpy(strName, str);
			newCurr = opendir(str);
			// fputs(str,saved); 
			char strSize[30];
			// sprintf(strSize," size = %d",(int)dataFile-
                      >st_size); 
			//fputs(strSize, saved); 
			//fputs("\n",saved); amountFiles++; 
			int num = 0;



			if (amountProc >= maxAmountProc) {

				//waitpid(pid,&num, WUNTRACED); 
				find_(len1, len2, newCurr, data, dataFile, strName);
	 
			}



			pid = fork();
			if (pid == 0) {
				// printf("curr pid is %d. amount of proccesses is 
                                   %d\n", getpid(), ++amountProc); 
				// printf("%s\n", str); 
				find_(len1, len2, newCurr, data, dataFile, strName);
			}
			else { 
				waitpid(pid, &num, WUNTRACED);
				// kill(pid, SIGKILL); 
				amountFiles += num;
				pid = 0;
			};
		}
	}
	// fclose(saved); 
	//exit(0); 
	//if(pid){ 
	//pid_t pidH=getppid(); 
	//kill(pid,SIGKILL); 
	//pid=pidH; 
	amountProc--;
	//exit(0);//amountFiles); 


	if (getpid() != pidMain) {
		kill(getpid(), SIGKILL);
	}
}

int main(int argc, char* argv[]) {
	struct dirent* currData = (struct dirent*)malloc(sizeof(struct dirent));
	char* end;
	struct stat* dataFile = (struct stat*)malloc(sizeof(struct stat));
	DIR* currDir;
	if (argc<5){ 
	 printf("usage terminated\n"); 
	 return 0; 
	 } 
	strcpy(MainDir, argv[1]);
	currDir = opendir(MainDir);
	int Min = strtol(argv[2], &end, 10);
	int Max = strtol(argv[3], &end, 10);// 
	char filya[256] = {};
	strcpy(filya, argv[4]);
	maxAmountProc = strtol(argv[5], &end, 10);
	pidMain = getpid();
	saved = fopen(filya, "w");
	find_(&Min, &Max, currDir, currData, dataFile, MainDir);
	fclose(saved);
	printf("\n amount of found Files= %d\n", amountFiles);
	return 0;
}

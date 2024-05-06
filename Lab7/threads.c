#include <sys/stat.h> 
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/wait.h>  
#include <dirent.h> 
#include <string.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include<unistd.h> 
#include <signal.h> 
#include <pthread.h> 
int amountFiles = 0;
int amountProc = 1;
int maxAmountProc;
char fileName[255];
pthread_t threadCurr;
int lenCurrF;
FILE* saved;

char MainDir[512] ={};
struct dirent* MainDirD;
typedef struct argument {
	int len1;
	int len2;
	DIR* curr;
	struct dirent data;
	struct stat dataFile;
	char Dir[512];
} Thread_arg;


void caller(void* args);


void find_(int* len1, int* len2, DIR* curr, struct dirent* data,
	struct stat* dataFile, char Dir[]) {
	while (curr != 0 && (data = readdir(curr)) != NULL) {
		char str[255] = {};
		char strName[255];
		strcpy(str, Dir);
		strcat(str, "/");
		strcat(str, data->d_name);
		stat(str, dataFile);
		// int res=(dataFile->st_mode) & 170000; 
		if ((dataFile->st_mode & 0x49) && S_ISREG(dataFile->st_mode) && 
                                           (dataFile->st_size >= *len1) && 
                                           (dataFile->st_size <= *len2)) {
 
			DIR* newCurr;
			char newDir[255] = {};
			strcpy(newDir, MainDir);
			newCurr = opendir(MainDir);
			lenCurrF = dataFile->st_blocks;
			fputs(str, saved);
			printf("%s\n", str);
			char strSize[30];
			sprintf(strSize, " size = %d", (int)dataFile->st_size);
			fputs(strSize, saved);
			fputs("\n", saved); amountFiles++;
		}
		if (S_ISDIR(dataFile->st_mode) && strcmp(data->d_name, ".") && 
                strcmp(data->d_name, "..")) {
			DIR* newCurr;
			char strName[255];
			strcpy(strName, str);
			newCurr = opendir(str);
			//fputs(str,saved); 
			char strSize[30];
			//sprintf(strSize," size = %d",(int)dataFile->st_size); 
			//fputs(strSize, saved); 
			//fputs("\n",saved); amountFiles++; 
			int num = 0;
			if (amountProc >= maxAmountProc) {
				find_(len1, len2, newCurr, data, dataFile, 
                              strName); 
				//pthread_join(threadCurr, NULL);  
				// amountProc--; 
			}
			Thread_arg* point=(Thread_arg*)malloc(sizeof(Thread_arg));
			point->dataFile = *dataFile;
			point->len1 = *len1;
			point->len2 = *len2;
			point->curr = newCurr;
			point->data = *data;
			strcpy(point->Dir, strName);
			amountProc++;
			// printf("amount proc=%d\n", amountProc); 
			pthread_create(&threadCurr, NULL, caller, (void*)point);
			pthread_join(threadCurr, NULL);



			// printf("curr pid is %d. amount of proccesses is %d\n", 
                             getpid(),++amountProc); 
			// printf("%s\n", str); 
			// find_(len1,len2,newCurr,data,dataFile,strName); 
			// } 
			// else{ 
			// }; 
		}
	}
	if (amountProc > 1) {
		amountProc--;
		pthread_exit(0);
	}
}



void caller(void* arg) {
	DIR* curr;
	Thread_arg* args = (Thread_arg*)arg;


	args->curr = opendir(args->Dir);
	find_(&(args->len1), &(args->len2), args->curr, &(args->data), 
            &(args->dataFile), args->Dir);
}

int main(int argc, char* argv[]) {
	struct dirent* currData=(struct dirent*)malloc(sizeof(struct dirent));
	char* end;
	struct stat* dataFile = (struct stat*)malloc(sizeof(struct stat));
	DIR* currDir;
	 if (argc<5){ 
	 printf("usage terminated\n"); 
	 return 0; 
	 } 

	 strcpy(MainDir,argv[1]); 
	currDir = opendir(MainDir);
	MainDirD = opendir(MainDir);
	// char nameFile[]="/home/egor/ACTOS7.txt"; 
	int Min = strtol(argv[2],&end,10); 
	int Max = strtol(argv[3],&end,10); 
	 saved=fopen(agrv[4], "w"); 
	maxAmountProc = strtol(argv[5],&end,10); 

	find_(&Min, &Max, currDir, currData, dataFile, MainDir);
	fclose(saved);
	printf("\n amount of found Files= %d\n", amountFiles);
	return 0;
}

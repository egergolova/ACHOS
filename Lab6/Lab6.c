#include <sys/stat.h> 
#include <stdio.h> 
#include <sys/types.h> 
//#include <fcnt1.h> 
#include <dirent.h> 
#include <string.h> 
#include <stdlib.h> 
/*Найти все дубликаты (с одинаковым содержимым) файлов в заданном диапазоне размеров от N1 до N2 
(N1, N2 задаются в аргументах командной строки), начиная с исходного каталога и ниже.
 Имя исходного каталога зада-ётся пользователем в качестве первого аргумента командной строки.*/
char fileName[255]; 
int lenCurrF; 
FILE* tocompare; 
FILE* compared; // soon will be remade 
char MainDir[512]= "/home/egor"; 
struct dirent *MainDirD; 
void find(DIR *curr, struct dirent *data, struct stat    
          *dataFile, char Dir[], 
     char fileName[]){ 
     while(curr!=0 && (data=readdir(curr))!=NULL){ 
          char str[255]={}; 
          strcpy(str, Dir); 
          strcat(str, "/"); 
          strcat(str,data->d_name); 
          stat(str, dataFile); 
          if(S_ISDIR(dataFile->st_mode) && strcmp(data->d_name,".")
             && strcmp(data->d_name,"..") ){ 
             DIR *newCurr; 
             char newStr[255]={}; 
             strcpy(newStr,str); 
             newCurr=opendir(newStr); 
             find(newCurr,data,dataFile,newStr,fileName); 
          } 
          if(S_ISREG(dataFile->st_mode)){  
             if(dataFile->st_blocks==lenCurrF){ 
                tocompare=fopen(str, "r"); 
                compared=fopen(fileName,"r"); 
                char str1[512]={}; 
                char str2[512]={}; 
                int fullySame=0; 
                if((tocompare!=0) && (compared!=0)){ 
                   int same=0; 
                   if(fgets(str1,512,tocompare)){ 
                      fclose(tocompare); 
                      same=1; 
                      tocompare=fopen(str, "r");  
                   } 
                   while(fgets(str1, 512, tocompare) && same){ 
                         fgets(str2, 512, compared); 
                         if(strcmp(str1,str2) ){ 
                            same=0; 

                         } 
                   } 
                   fclose(compared); 
                   fclose(tocompare); 
                   if(same){ 
                      printf("%s/%s %s\n",MainDir, data->d_name,
                              fileName); 
                   } 
                } 
             } 
 
         } 
     } 
} 
 
void choose_comparedFile( int *len1, int *len2,DIR *curr, 
                         struct dirent *data, struct stat
                         *dataFile, char Dir[]){ 
    while(curr!=0 && (data=readdir(curr))!=NULL){ 
          char str[255]={}; 
          char strName[255]; 
          strcpy(str, Dir); 
          strcat(str, "/"); 
          strcat(str,data->d_name); 
          stat(str, dataFile); 
          if(S_ISDIR(dataFile->st_mode) && strcmp(data->d_name,".")  
            && strcmp(data->d_name,"..") ){ 
            DIR *newCurr; 
            char strName[255]; 
            strcpy(strName,str); 
            newCurr=opendir(str); 
  choose_comparedFile(len1, len2,    
  newCurr,data,dataFile,strName);//,strName); 
         } 
 
         if(S_ISREG(dataFile->st_mode) && (dataFile->st_blocks>= 
            *len1) && (dataFile->st_blocks<= *len2)){ 
            DIR *newCurr; 
            char newDir[255]={}; 
            strcpy(newDir,MainDir); 
            newCurr=opendir(MainDir); 
            lenCurrF=dataFile->st_blocks; 
            find(newCurr,data,dataFile,newDir,str); 
         } 
 
    } 
} 
 
int main(int argc,char *argv[]){ 
    struct dirent *currData=(struct dirent*)malloc(sizeof(struct    
                             dirent)); 
    char *end; 
    struct stat *dataFile=(struct stat*)malloc(sizeof(struct 
                                               stat)); 
    DIR *currDir; 
    if (argc<3){ 
       printf("usage terminated\n"); 
       return 0; 
    } 
    strcpy(MainDir,argv[1]); 
    currDir=opendir(MainDir); 
    MainDirD=opendir(MainDir); 
    int Min=strtol(argv[2],&end,10); 
    int Max=strtol(argv[3],&end,10); 
    choose_comparedFile(&Min, &Max,currDir,currData,dataFile,
                        MainDir); 
    return 0; 
}

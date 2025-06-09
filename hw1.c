#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#define READ_FLAGS O_RDONLY
#define WRITE_FLAGS (O_WRONLY | O_TRUNC | O_EXCL)
#define WRITE_PERMS (S_IRUSR | S_IWUSR)

void replaceAll(int fromfd, char* inputPath, char* buff, char* str1, char* str2){
	char *bp;
	char *buf;
	char *temp;
	int bytesread = 0;
	int byteswritten = 0;
	int totalbytes = 0;
	int writefd;
	
	int j;
	
	struct flock lock;
	
	memset(&lock, 0, sizeof(lock));
	lock.l_type = F_RDLCK;
	fcntl(fromfd, F_SETLKW, &lock);
	
	buf=(char*)malloc(1);
	
	char template[] = "/tmp/myTmpFile-XXXXXX";
	
	int tempfd = mkstemp(template);
	
	if (tempfd == -1)	perror("\nError in generating temporary file\n");

	unlink(template);
	
	for( ; ; ){
		
		temp = (char*)malloc(strlen(buff));
	
		while(((bytesread = read(fromfd, buf, 1)) == -1) && (errno == EINTR)) ;
		if(bytesread <= 0)
			break;
		
		bp = buf;
		temp[0] = buf[0];
		
		while(bytesread > 0){

			if(temp[0] == str1[0]){
				for(j=1; j<strlen(str1); j++){
				
					while(((bytesread = read(fromfd, buf, 1)) == -1) && (errno == EINTR)) ;
					
					strncat(temp, buf,1);
					
					if (bytesread <= 0 || buf[0] != str1[j])	break;
				} 
				
				if(strcmp(temp, str1) == 0){
					strcpy(temp, str2);
				}
			} 
			
			bytesread = strlen(temp);
			strcpy(bp, temp);
			byteswritten = strlen(temp);
			
			while(((byteswritten = write(tempfd, bp, byteswritten)) == -1) && (errno == EINTR));
			if(byteswritten < 0)
				break;
			totalbytes += byteswritten;
			bytesread -= byteswritten;
			bp += byteswritten;
			
		}
		if(byteswritten == -1)
			break;
	}
	
	bytesread = 0;
	byteswritten = 0;
	totalbytes = 0;
	
	lseek(tempfd, 0, SEEK_SET);
	
	if(close(fromfd) == -1){
		perror("\nFailed to close input file\n");
		return;
	}
	
	if((writefd = open(inputPath, WRITE_FLAGS)) == -1){
		perror("\nFailed to open input file for writing\n");
		return;
	}
	
	for( ; ; ){
		
		while(((bytesread = read(tempfd, bp, 1)) == -1) && (errno == EINTR)) ;
		if(bytesread <= 0)
			break;
			
		
		while(bytesread > 0){
			while(((byteswritten = write(writefd, bp, bytesread)) == -1) && (errno == EINTR));
			if(byteswritten < 0)
				break;
			totalbytes += byteswritten;
			bytesread -= byteswritten;
			bp += byteswritten;
			
		}
		if(byteswritten == -1)
			break;
	}
	
	free(buf);
	free(temp);
	
	lock.l_type = F_UNLCK;
	fcntl(fromfd, F_SETLKW, &lock);
	
	if(close(tempfd) == -1){
		perror("\nError removing temporary file\n");
		return;
	}
}

void replaceAllCaseInsensitive(int fromfd, char* inputPath, char* buff, char* str1, char* str2){
	char *bp;
	char *buf;
	char *temp;
	int bytesread = 0;
	int byteswritten = 0;
	int totalbytes = 0;
	int writefd;
	
	int j;
	
	struct flock lock;
	
	memset(&lock, 0, sizeof(lock));
	lock.l_type = F_RDLCK;
	fcntl(fromfd, F_SETLKW, &lock);
	
	buf=(char*)malloc(1);
	
	char template[] = "/tmp/myTmpFile-XXXXXX";
	
	int tempfd = mkstemp(template);
	
	if (tempfd == -1)	perror("\nError in generating temporary file\n");

	unlink(template);
	
	for( ; ; ){
		
		temp = (char*)malloc(strlen(buff));
	
		while(((bytesread = read(fromfd, buf, 1)) == -1) && (errno == EINTR)) ;
		if(bytesread <= 0)
			break;
		
		bp = buf;
		temp[0] = buf[0];
		
		while(bytesread > 0){

			if(temp[0] == str1[0]){
				for(j=1; j<strlen(str1); j++){
				
					while(((bytesread = read(fromfd, buf, 1)) == -1) && (errno == EINTR)) ;
					
					strncat(temp, buf, 1);
					
					if (bytesread <= 0 || buf[0] != str1[j])	break;
				} 
				
				if(strcmp(temp, str1) == 0){
					strcpy(temp, str2);
				}
			}else if(temp[0] >= 'A' && temp[0] <= 'Z' && temp[0] + 32 == str1[0]){
				for(j=1; j<strlen(str1); j++){
				
					while(((bytesread = read(fromfd, buf, 1)) == -1) && (errno == EINTR)) ;
					
					strncat(temp, buf, 1);
					
					if (bytesread <= 0 || buf[0] != str1[j])	break;
				} 
				
				if(strcasecmp(temp, str1) == 0){
					strcpy(temp, str2);
					temp[0] -= 32;
				}
			}else if(temp[0] >= 'a' && temp[0] <= 'a' && temp[0] - 32 == str1[0]){
				for(j=1; j<strlen(str1); j++){
				
					while(((bytesread = read(fromfd, buf, 1)) == -1) && (errno == EINTR)) ;
					
					strncat(temp, buf, 1);
					
					if (bytesread <= 0 || buf[0] != str1[j])	break;
				} 
				
				if(strcasecmp(temp, str1) == 0){
					strcpy(temp, str2);
					temp[0] += 32;
				}
			}
			
			bytesread = strlen(temp);
			strcpy(bp, temp);
			byteswritten = strlen(temp);
			
			while(((byteswritten = write(tempfd, bp, byteswritten)) == -1) && (errno == EINTR));
			if(byteswritten < 0)
				break;
			totalbytes += byteswritten;
			bytesread -= byteswritten;
			bp += byteswritten;
			
		}
		if(byteswritten == -1)
			break;
	}
	
	bytesread = 0;
	byteswritten = 0;
	totalbytes = 0;
	
	lseek(tempfd, 0, SEEK_SET);
	
	if(close(fromfd) == -1){
		perror("\nFailed to close input file\n");
		return;
	}
	
	if((writefd = open(inputPath, WRITE_FLAGS)) == -1){
		perror("\nFailed to open input file for writing\n");
		return;
	}
	
	for( ; ; ){
		
		while(((bytesread = read(tempfd, bp, 1)) == -1) && (errno == EINTR)) ;
		if(bytesread <= 0)
			break;
			
		while(bytesread > 0){
			while(((byteswritten = write(writefd, bp, bytesread)) == -1) && (errno == EINTR));
			if(byteswritten < 0)
				break;
			totalbytes += byteswritten;
			bytesread -= byteswritten;
			bp += byteswritten;
			
		}
		if(byteswritten == -1)
			break;
	}
	
	free(buf);
	free(temp);
	
	lock.l_type = F_UNLCK;
	fcntl(fromfd, F_SETLKW, &lock);
	
	if(close(tempfd) == -1){
		perror("\nError removing temporary file\n");
		return;
	}
}

void multipleMatching(int fromfd, char* inputPath, char* buff, char* str1, char* str2, char* str3, int caseSen){ /* caseSen is 1 if case insensitive, 0 if case sensitive */
	
	char* temp;
	
	int i, j;
	for(i=0; i<strlen(str3); i++){
		temp = (char*)malloc(strlen(str1));
		strcpy(temp, str1);
		temp[strlen(temp)] = '\0';
		for(j=0; j<strlen(temp); j++){
			if(temp[j] == '.')	temp[j] = str3[i];
		}
		if(caseSen == 1){	/*replaceAllCaseInsensitive(fromfd, inputPath, buff, temp, str2);*/
		}else if(caseSen == 0){	/*replaceAll(fromfd, inputPath, buff, temp, str2);*/}
		free(temp);
	}
}

void matchLineStarts(int fromfd, char* inputPath, char* buff, char* str1, char* str2){

}

void matchLineEnds(int fromfd, char* inputPath, char* buff, char* str1, char* str2){

}

void replaceRepetitions(int fromfd, char* inputPath, char* buff, char* str1, char* str2){
	char *bp;
	char *buf;
	char *temp;
	int bytesread = 0;
	int byteswritten = 0;
	int totalbytes = 0;
	int writefd;
	char *c;
	
	int j;
	
	struct flock lock;
	
	memset(&lock, 0, sizeof(lock));
	lock.l_type = F_RDLCK;
	fcntl(fromfd, F_SETLKW, &lock);
	
	buf=(char*)malloc(1);
	
	char template[] = "/tmp/myTmpFile-XXXXXX";
	
	int tempfd = mkstemp(template);
	
	if (tempfd == -1)	perror("\nError in generating temporary file\n");

	unlink(template);
	
	for( ; ; ){
		
		temp = (char*)malloc(strlen(buff));
	
		while(((bytesread = read(fromfd, buf, 1)) == -1) && (errno == EINTR)) ;
		if(bytesread <= 0)
			break;
		
		bp = buf;
		temp[0] = buf[0];
		
		while(bytesread > 0){

			if(temp[0] == str1[0]){
				for(j=1; str1[j] != '\0'; j++){
				
					while(((bytesread = read(fromfd, buf, 1)) == -1) && (errno == EINTR)) ;
					
					c = buf;
					
					while(((bytesread = read(fromfd, buf, 1)) == -1) && (errno == EINTR)) ;
					
					j++;
					
					strncat(temp, c, 1);
					
					while(((bytesread = read(fromfd, buf, 1)) == -1) && (errno == EINTR)) ;
						
					c = buf;
					
					if(str1[j] == '*' && c[0] == '*'){
					
						while(str1[j] == c[0]){
						
							while(((bytesread = read(fromfd, buf, 1)) == -1) && (errno == EINTR)) ;
						
							c = buf;
						
						}
					
					}
					
					strncat(temp, buf,1);
					
					if (bytesread <= 0 || buf[0] != str1[j])	break;
				} 
				
				if(strcmp(temp, str1) == 0){
					strcpy(temp, str2);
				}
			} 
			
			bytesread = strlen(temp);
			strcpy(bp, temp);
			byteswritten = strlen(temp);
			
			while(((byteswritten = write(tempfd, bp, byteswritten)) == -1) && (errno == EINTR));
			if(byteswritten < 0)
				break;
			totalbytes += byteswritten;
			bytesread -= byteswritten;
			bp += byteswritten;
			
		}
		if(byteswritten == -1)
			break;
	}
	
	bytesread = 0;
	byteswritten = 0;
	totalbytes = 0;
	
	lseek(tempfd, 0, SEEK_SET);
	
	if(close(fromfd) == -1){
		perror("\nFailed to close input file\n");
		return;
	}
	
	if((writefd = open(inputPath, WRITE_FLAGS)) == -1){
		perror("\nFailed to open input file for writing\n");
		return;
	}
	
	for( ; ; ){
		
		while(((bytesread = read(tempfd, bp, 1)) == -1) && (errno == EINTR)) ;
		if(bytesread <= 0)
			break;
			
		
		while(bytesread > 0){
			while(((byteswritten = write(writefd, bp, bytesread)) == -1) && (errno == EINTR));
			if(byteswritten < 0)
				break;
			totalbytes += byteswritten;
			bytesread -= byteswritten;
			bp += byteswritten;
			
		}
		if(byteswritten == -1)
			break;
	}
	
	free(buf);
	free(temp);
	
	lock.l_type = F_UNLCK;
	fcntl(fromfd, F_SETLKW, &lock);
	
	if(close(tempfd) == -1){
		perror("\nError removing temporary file\n");
		return;
	}
}

void operation(char* buff, int fromfd, char* inputPath){

	char* str1;
	char* str2;
	char* str3;
	char* temp;
	
	str1 = (char*)malloc(strlen(buff));
	str2 = (char*)malloc(strlen(buff));
	str3 = (char*)malloc(strlen(buff));
	temp = (char*)malloc(strlen(buff));
	
	int linestart = 0;
	int lineend = 0;

	if(buff[0] != '/'){
		perror("\nStrings must be entered as '/str1/str2/...'\n");
		return;
	}
	
	int i,j, k;
	for(i=1, j=0, k=0; i<strlen(buff); i++){
		if(buff[i] != '/' && buff[i] != '['){
			str1[j] = buff[i];
			j++;
		}else if(buff[i] == '['){
			str1[j] = '.';
			j++;
			i++;
			while(buff[i] != ']'){
				str3[k] = buff[i];
				i++;
				k++;
			}
		}else if(buff[i] == '^'){
			linestart = 1;
			i++;
		}	
		else break;
	}

	str1[j] = '\0';
	str3[k] = '\0';
	
	i++;
	j=0;
	
	for( ; buff[i] != '/'; i++, j++){
		str2[j] = buff[i];
	}
	
	str2[j]='\0';
	
	str1[strlen(str1)] = '\0';
	str2[strlen(str2)] = '\0';
	str3[strlen(str3)] = '\0';
	buff[strlen(buff)] = '\0';
	
	if(strchr(str1, '*') != NULL)	replaceRepetitions(fromfd, inputPath, buff, str1, str2);
	
	if(strlen(str3) > 0 && buff[i+1] == 'i'){
		multipleMatching(fromfd, inputPath, buff, str1, str2, str3, 1);
		i++;
	}else if(strlen(str3) > 0 && buff[i+1] != 'i'){
		multipleMatching(fromfd, inputPath, buff, str1, str2, str3, 0);
	}
	if(strlen(str3) > 0 && buff[i] == '\0'){
		free(str1);
		free(str2);
		free(str3);
		close(fromfd);
		return;
	}
	i++;
	
	if(buff[i] == '\0'){
		replaceAll(fromfd, inputPath, buff, str1, str2);
		i++;
		free(str1);
		free(str2);
		free(str3);
		close(fromfd);
		return;
	}else if(buff[i] == ';'){
		replaceAll(fromfd, inputPath, buff, str1, str2);
		i++;
	}else if(buff[i] == 'i'){
		 replaceAllCaseInsensitive(fromfd, inputPath, buff, str1, str2);
		 if(buff[i+1] == ';')	i += 2;
		 else if(buff[i+1] == ';'){
		 	free(str1);
			free(str2);
			free(str3);
		 	close(fromfd);
		 	return;
		 }
	}
	
	if(buff[i] == '/'){
	
		for(j=0; buff[i] != '\0'; j++, i++)	temp[j] = buff[i];
		
		temp[j] = '\0';
		
		free(str1);
		free(str2);
		free(str3);
		
		close(fromfd);
		
		if((fromfd = open(inputPath, READ_FLAGS)) == -1){
			perror("\nFailed to open input file\n");
			return;
		}
		
		operation(temp, fromfd, inputPath);
	}
}

int main(int argc, char *argv[]){

	int fromfd;
	
	if(argc != 3){
		perror("\nUsage: string replacement\n");
		return 1;
	}
	
	if((fromfd = open(argv[2], READ_FLAGS)) == -1){
		perror("\nFailed to open input file\n");
		return 1;
	}
	
	operation(argv[1], fromfd, argv[2]);
	
	close(fromfd);
	
	return 0;
}

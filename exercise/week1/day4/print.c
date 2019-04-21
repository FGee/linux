#include <func.h>                                                   
int findFile(char* dirName, char* fileName)
{
	DIR *dir;
	dir=opendir(dirName);
	ERROR_CHECK(dir,NULL,"opendir");
	struct dirent *p;
	char path[512]={0};
	while((p=readdir(dir)))
	{
		if(!strcmp(p->d_name,".")||!strcmp(p->d_name,".."))
		{
			continue;
		}
		if (!strcmp(p->d_name, fileName)){
			printf("%s\n", dirName);
			break;
		}
		//printf("%*s%s\n",width,"-",p->d_name);
		sprintf(path,"%s%s%s",dirName,"/",p->d_name);
		if(4==p->d_type)
		{
			findFile(path,fileName);
		}
	}
	closedir(dir);
}

int main(int argc,char* argv[])
{
	ARGS_CHECK(argc,3);
	findFile(argv[1], argv[2]);

	return 0;
}

#include <func.h>    

void produceMode(long mode, char *ps)
{
	int cnt = 0;
	switch ((mode >> 12)) {
	case 1:
		ps[cnt] = 'f';
		break;
	case 2:
		ps[cnt] = 'c';
		break;
	case 4:
		ps[cnt] = 'd';
		break;
	case 6:
		ps[cnt] = 'b';
		break;
	case 8:
		ps[cnt] = '-';
		break;
	case 10:
		ps[cnt] = 'l';
		break;
	case 12:
		ps[cnt] = 's';
		break;
	}
	++cnt;
	for (int i = 6; i >= 0; i = i - 3) {
		if ((mode >> i) & (1 << 0)) {
			ps[cnt] = 'r';
			++cnt;
		}
		else {
			ps[cnt] = '-';
			++cnt;
		}
		if ((mode >> i) & (1 << 1)) {
			ps[cnt] = 'w';
			++cnt;
		}
		else {
			ps[cnt] = '-';
			++cnt;
		}
		if ((mode >> i) & (1 << 2)) {
			ps[cnt] = 'x';
			++cnt;
		}
		else {
			ps[cnt] = '-';
			++cnt;
		}
	}
	ps[cnt] = '\0';
} 

int printStat(char *dirName)
{
	DIR *dir;
	dir = opendir(dirName);
	struct dirent *p;
	while((p = readdir(dir)))
	{
		char pathName[30];
		strcpy(pathName, dirName);
		strcat(pathName, p->d_name);
		if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
		{
			continue;
		}
		int ret;
		struct stat buf;
		ret = stat(pathName, &buf);
		ERROR_CHECK(ret, -1, "stat");
		char md[20] = {0};
		produceMode(buf.st_mode, md);
		
		char *month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		struct tm *pTm;
		pTm = localtime(&buf.st_mtime);
		printf("%s %ld %s %s %-5ld %s %2d %2d:%2d %s\n", md, buf.st_nlink, getpwuid(buf.st_uid)->pw_name, getgrgid(buf.st_gid)->gr_name, buf.st_size, month[pTm->tm_mon], pTm->tm_mday, pTm->tm_hour, pTm->tm_min,p->d_name);
	}
}

int main(int argc,char* argv[])
{
	ARGS_CHECK(argc,2);
	printStat(argv[1]);

	return 0;
}

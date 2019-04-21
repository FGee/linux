#include <func.h>

typedef struct student {
	int id;
	char name[20];
	float scores;
} Stu, *pStu;

int main(int argc, char ** argv)
{
	ARGS_CHECK(argc, 2);
	int fd;
	Stu stuArr[3] ={ { 1901, "lihua", 89.5 },
		{1902, "xiaoming", 89},
			{1943, "dayu", 92}};
	//pStu p[3] = {stuArr[0], stuArr[1], stuArr[3]);
	fd = open(argv[1], O_CREAT | O_RDWR, 777);
	int ret;
	ret = write(fd, stuArr ,sizeof(Stu)*3);
	lseek(fd, 0, SEEK_SET);
	memset(stuArr, 0, sizeof(Stu)*3);
	read(fd, stuArr, sizeof(Stu)*3);
	for (int i = 0; i < 3; ++i){
		printf("%d %s %f\n", stuArr[i].id, stuArr[i].name, stuArr[i].scores);
	}
	close(fd);

	return 0;
}

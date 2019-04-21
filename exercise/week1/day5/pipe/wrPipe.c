#include <func.h>

int main(int argc, char** argv)
{
	ARGS_CHECK(argc, 3);
	int fdw;
	fdw = open(argv[1], O_WRONLY);
	ERROR_CHECK(fdw, -1, "open");
	int fdr;
	fdr = open(argv[2], O_RDONLY);
	ERROR_CHECK(fdr, -1, "open");

	char buf[20] = { 0 };
	printf("i am %d\n", fdw);
	write(fdw, "how are you?",12);
	read(fdr, buf, sizeof(buf));
	printf("%s\n", buf);
	printf("i am %d\n", fdr);
	close(fdw);
	close(fdr);


	return 0;
}


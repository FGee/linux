#include <func.h>

int main(int argc, char** argv)
{
	ARGS_CHECK(argc, 3);
	int fdr;
	fdr = open(argv[1], O_RDONLY);
	ERROR_CHECK(fdr, -1, "open");
	int fdw;
	fdw = open(argv[2], O_WRONLY);
	ERROR_CHECK(fdw, -1, "open");

	char buf[20] = {0};
	printf("i am %d\n", fdr);
	read(fdr, buf, sizeof(buf));
	printf("%s\n", buf);
	printf("i am %d\n", fdw);
	write(fdw, "i am fine.", 10);

	close(fdw);
	close(fdr);


	return 0;
}


#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

int main(int argc, char *argv[]) {
	int file;
	int chr;
	int count;
		if ((file = open(argv[1], O_RDONLY)) < 0) {
			write(2, "error", 6);
		}
		int err;
		while ((err = read(file, &chr, 1)) != 0) {
			write(0, &chr, 1);
		}
		if (err < 0)
			write(1, "error", 6);
		close(file);
	return 0;
}
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/drivers/memento.h>
#include <sys/ioctl.h>

void tell_read(int fd, char * buf) {
	read(fd, buf, 1);
	printf("I read \033[33m0x%x\033[0m\n", buf[0]);
}

int main(int argc, char *argv[]) {
	printf("%d\n", MOMENTO_GET);
	int ret, i;
	char read_data[4] = {0};
	char write_data[4] = {1, 2, 3, 60};
	int device_number;
	if (argc > 1) {
		device_number = argv[1][0] - '0';
	} else {
		device_number = 0;
	}
	char device[13];
	sprintf(device, "/dev/memento%d", device_number);
	int fd = open(device, O_RDWR);
	if (fd != -1) {
		printf("First, i expecting 0x%x\n", device_number);
		tell_read(fd, read_data);
		ret = write(fd, write_data, 4);
		printf("I would write: ");
		for (i = 0; i < 4; ++i) {
			printf(" 0x%x", write_data[i]);
		}
		printf("\n");
		printf("I expecting 0x%x\n", write_data[3]);
		tell_read(fd, read_data);
		ioctl(fd, MOMENTO_RESET);
		printf("I reset it to origin, expecting 0x%x\n", device_number);
		tell_read(fd, read_data);
		char new_origin = 0x3b;
		printf("I set its origin value to 0x%x\n", new_origin);
		ioctl(fd, MOMENTO_SET, &new_origin);
		printf("I would read its origin value, expecting 0x%x\n", new_origin);
		ioctl(fd, MOMENTO_GET, read_data);
		printf("I read 0x%x\n", read_data[0]);
		tell_read(fd, read_data);
	} else {
		printf("open %s failed! \n", device);
	}
	return 0;
}

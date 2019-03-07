/*
 * ServoControl.cpp
 *
 *  Created on: Nov 11, 2014
 *      Author: dev
 */

#include "ServoControl.h"

int maestroOpenConnect(const char * device) {
	int handle;
	handle = open(device, O_RDWR | O_NOCTTY);
	if (handle == -1) {
		perror(device);
	}
	return handle;
}

void maestroDisconnect(int fd) {
	close(fd);
}

int maestroGetPosition(int fd, unsigned char channel) {
	unsigned char command[] = { 0x90, channel };

	if (write(fd, command, sizeof(command)) == -1) {
		perror("error writing");
		return -1;
	}

	unsigned char response[2];

	if (read(fd, response, 2) != 2) {
		perror("error reading");
		return -1;
	}
	return response[0] + 256 * response[1];
}

int maestroSetTarget(int fd, unsigned char channel, unsigned short target) {
	// {0x84, channel, target & 0x7F, target >> 7 & 0x7F};
	unsigned char command[4];
	command[0] = 0x84;
	command[1] = channel;
	command[2] = target & 0x7F;
	command[3] = target >> 7 & 0x7F;

	if (write(fd, command, sizeof(command)) == -1) {
		perror("error writing");
		return -1;
	} else {
		return 0;
	}
}

int maestroSetMultiTarget(int fd, int count, unsigned char firstChannel, int target, ...){
	int commandLength = 3 + count * 2;
	unsigned char command[commandLength];
	command[0] = 0x9F;
	command[1] = count;
	command[2] = firstChannel;

	va_list  ap;
	va_start(ap, target);

	unsigned short t = target;
	//printf("%d:%d\n",0,t);
	command[3] = t & 0x7F;
	command[4] = t >> 7 & 0x7F;

	for(int i = 1;i<count;i++){
		t = va_arg(ap, int);
		//printf("%d:%d\n",i,t);
		command[i*2+3] = t & 0x7F;
		command[i*2+4] = t >> 7 & 0x7F;
	}

	va_end(ap);

	if (write(fd, command, sizeof(command)) == -1) {
		perror("error writing");
		return -1;
	} else {
		return 0;
	}
}

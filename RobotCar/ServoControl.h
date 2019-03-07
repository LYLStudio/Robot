/*
 * ServoControl.h
 *
 *  Created on: Nov 11, 2014
 *      Author: dev
 */

#ifndef SERVOCONTROL_H_
#define SERVOCONTROL_H_

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

int maestroOpenConnect(const char * device);

void maestroDisconnect(int fd);

// Gets the position of a Maestro channel.
// See the "Serial Servo Commands" section of the user's guide.
int maestroGetPosition(int fd, unsigned char channel);

// Sets the target of a Maestro channel.
// See the "Serial Servo Commands" section of the user's guide.
// The units of 'target' are quarter-microseconds.
int maestroSetTarget(int fd, unsigned char channel, unsigned short target);

//0x9F, 0x0n, ...
//command[i+3] = (va_arg(arguments, int)) & 0x7F;
//command[i+4] = (va_arg(arguments, int)) >> 7 & 0x7F;
//unsigned char command[] = {0x9F, totalChannel, target & 0x7F, target >> 7 & 0x7F};
int maestroSetMultiTarget(int fd, int count, unsigned char firstChannel, int target, ...);

#endif /* SERVOCONTROL_H_ */

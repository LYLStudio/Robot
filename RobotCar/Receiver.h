/*
 * Receiver.h
 *
 *  Created on: Nov 12, 2014
 *      Author: dev
 */

#ifndef RECEIVER_H_
#define RECEIVER_H_

#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "Robot.h"

void error(char *msg);

void sendData(int sockfd, int x);

vector<int> getData(int sockfd);

int startListen(int portno);

void ControllerCommandReceive(vector<int> v);

void UICommandReceive(vector<int> v);
#endif /* RECEIVER_H_ */

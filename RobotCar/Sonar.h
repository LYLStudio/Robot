/*
 * Sonar.h
 *
 *  Created on: Feb 11, 2015
 *      Author: dev
 */

#ifndef SONAR_H_
#define SONAR_H_

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <wiringPi.h>
#include <wiringSerial.h>

using namespace std;

int OpenArduino(const char *dev);
int OpenArduino1(const char *dev);
void CloseArduino(int uart0_filestream);
void CloseArduino1(int uart0_filestream);
//vector<int> GetSonarValues(int uart0_filestream, unsigned char cmd);
vector<int> GetSonarValues(int uart0_filestream);
vector<int> GetSonarValues1(int uart0_filestream);
#endif /* SONAR_H_ */

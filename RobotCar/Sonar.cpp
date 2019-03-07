/*
 * Sonar.cpp
 *
 *  Created on: Feb 11, 2015
 *      Author: dev
 */
#include "Sonar.h"
int isTerminate = false;

int OpenArduino(const char *dev) {
    int uart0_filestream = -1;
    uart0_filestream = open(dev, O_RDWR | O_NOCTTY | O_NDELAY); //Open in non blocking read/write mode
    if (uart0_filestream == -1) {
        //ERROR - CAN'T OPEN SERIAL PORT
        printf(
                "Error - Unable to open UART.  Ensure it is not in use by another application\n");
        return uart0_filestream;
    }

    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD; //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    return uart0_filestream;
}

int OpenArduino1(const char *dev) {
    int fd;
    fd = serialOpen(dev, 9600);
    return fd;
}

void CloseArduino(int uart0_filestream) {
    isTerminate = true;
    close(uart0_filestream);
}

void CloseArduino1(int uart0_filestream) {
    isTerminate = true;
    serialClose(uart0_filestream);
}
//vector<int> GetSonarValues(int uart0_filestream, unsigned char cmd) {

vector<int> GetSonarValues(int uart0_filestream) {
    //----- TX BYTES -----
    //unsigned char tx_buffer[20];
    //unsigned char *p_tx_buffer;
    //p_tx_buffer = &tx_buffer[0];
    //*p_tx_buffer++ = cmd;
    //*p_tx_buffer++ = cmd;
    //*p_tx_buffer++ = cmd;
    /*
     if (uart0_filestream != -1) {
     int count = write(uart0_filestream, &tx_buffer[0],
     (p_tx_buffer - &tx_buffer[0]));	//Filestream, bytes to write, number of bytes to write
     if (count < 0) {
     printf("UART TX error\n");
     }
     delay(60);
     //sleep(1);
     }
     */
    //----- CHECK FOR ANY RX BYTES -----
    while (true) {
        if (isTerminate)
            break;
        if (uart0_filestream != -1) {

            // Read up to 255 characters from the port if they are there
            //unsigned
            char rx_buffer[256];
            int rx_length = read(uart0_filestream, (void*) rx_buffer, 255); //Filestream, buffer to store in, number of bytes to read (max)
            if (rx_length < 0) {
                //An error occured (will occur if there are no bytes)
            } else if (rx_length == 0) {
                //No data waiting
            } else {
                //Bytes received
                //rx_buffer[rx_length] = '\0';
                //printf("%i bytes read : %s", rx_length, rx_buffer);
                //cout << rx_buffer << endl;
                if (rx_length == 11) {
                    //cout << rx_buffer << endl;
                    stringstream ss(reinterpret_cast<char*> (rx_buffer));
                    vector<int> distance;
                    int i;
                    while (ss >> i) {
                        //cout << i << endl;
                        distance.push_back(i);

                        if (ss.peek() == ',')
                            ss.ignore();
                    }
                    return distance;
                }
            }
        }
        delay(200);
    }
}

vector<int> GetSonarValues1(int uart0_filestream) {
    char buffer[256];
    // Loop, getting and printing characters
    int seq = 0;
    for (;;) {
        if (isTerminate)
            break;
        char c = serialGetchar(uart0_filestream);
        if (c != '\r') {
            buffer[seq] = c;
            seq++;
        } else {
            buffer[seq] = '\0';
            seq = 0;
            string s((char *) buffer);
            if (s.length() == 11) {
                //cout << s << endl;
                stringstream ss(s);
                vector<int> distance;
                int i;
                while (ss >> i) {
                    //cout << i << endl;
                    distance.push_back(i);

                    if (ss.peek() == ',')
                        ss.ignore();
                }
                return distance;
            }
        }
    }
}
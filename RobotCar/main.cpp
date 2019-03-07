/*
 * main.cpp
 *
 *  Created on: Nov 11, 2014
 *      Author: dev
 */
#include "Receiver.h"
#include <iostream>
#include <fstream>

using namespace std;

int LeftWheelBasePower;
int RightWheelBasePower;
double WheelTurnPowerLevels[24];
bool showInfo = false;
int port = 9999;
string devName = "/dev/ttyACM0"; //pololu port
string configFile = "RobotCar.cfg";
string arduinoName = "/dev/ttyUSB0";

void ShowAthorMessage(char *argv0) {
    cout << "######################################################" << endl;
    cout << "Robot Platform 2015(alpha version)" << endl;
    cout << "Athor: LYLStudio" << endl;
    cout << "######################################################" << endl;
    cout << "Usage:" << endl;
    cout << "RobotCar -d [pololu_dev_path] -p [port_number] -c [config_file] [-v] [-h]" << endl;
    cout << "Optional: '-v' show all status message." << endl;
    cout << "Optional: '-h' show this." << endl;
    cout << endl;
}

void ReadConfig(string filePath) {
    ifstream powerLVSettings(configFile);
    double level;
    int count = 0;

    while (powerLVSettings >> level) {
        if (count <= 23) {
            WheelTurnPowerLevels[count] = level;
            //cout << WheelTurnPowerLevels[count] << endl;
        }
        if (count == 24)
            LeftWheelBasePower = level;

        if (count == 25)
            RightWheelBasePower = level;
        count++;
    }
}

int main(int argc, char *argv[]) {
    /*
    int fd = OpenArduino("/dev/ttyUSB0");
    //cout << fd << endl;
    vector<int> sonarv = GetSonarValues(fd,'A');
    for(size_t i=0;i < sonarv.size();i++){
            cout << sonarv[i] << endl;
    }
    CloseArduino(fd);
     */
    int cmd_opt = 0;

    //fprintf(stderr, "argc:%d\n", argc);
    while (1) {
        //fprintf(stderr, "proces index:%d\n", optind);
        cmd_opt = getopt(argc, argv, "d:p:c:vh");

        /* End condition always first */
        if (cmd_opt == -1) {
            break;
        }

        /* Print option when it is valid
        if (cmd_opt != '?') {
                fprintf(stderr, "option:-%c,", cmd_opt);
        }
         */
        /* Lets parse */
        switch (cmd_opt) {
                /* No args */
            case 'v':
                showInfo = true;
                break;

            case 'h':
                ShowAthorMessage(argv[0]);
                exit(1);
                break;

                /* Single arg */
            case 'd':
                devName = optarg;
                //fprintf(stderr, "option arg:%s\n", optarg);
                break;

            case 'p':
                port = atoi(optarg);
                //fprintf(stderr, "option arg:%s\n", optarg);
                break;

            case 'c':
                configFile = optarg;
                //fprintf(stderr, "option arg:%s\n", optarg);
                break;

                /* Error handle: Mainly missing arg or illegal option */
            case '?':
                fprintf(stderr, "Illegal option:-%c\n", isprint(optopt) ? optopt : '#');
                ShowAthorMessage(argv[0]);
                exit(1);
                break;
            default:
                fprintf(stderr, "Not supported option\n");
                break;
        }
    }

    cout << "ConfPath\t:\t" << configFile << endl;
    cout << "DevicePort\t:\t" << devName << endl;
    cout << "SonarDevicePort\t:\t" << arduinoName << endl;
    cout << "PortNumber\t:\t" << port << endl;
    cout << "ShowInfo\t:\t" << showInfo << endl;

    ReadConfig(configFile);
    delay(300);
    Initialize();
    delay(300);
    startListen(port);

    return 0;
}


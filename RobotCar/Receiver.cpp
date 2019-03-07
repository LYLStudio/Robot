/*
 * Receiver.cpp
 *
 *  Created on: Nov 12, 2014
 *      Author: dev
 */

#include "Receiver.h"

extern MoveActType needMoveActType;
extern int fd;

void ControllerCommandReceive(vector<int> v) {
    //cout << "Controller Command Rec: ";
    CommandType cmdType = (CommandType) v[1];
    switch (cmdType) {
        case WheelCommand:
            //cout << "Wheel Aciton" << v[2] << "," << v[3] << endl;
            maestroSetMultiTarget(fd, 2, 0x16, v[2], v[3]);
            break;
        case ArmCommand:
            //cout << "Arm Aciton " << v[2] << endl;
            ArmAction((ArmActType) v[2]);
            break;
        case AudioCommand:
            //cout << "Alarm Action " << v[1] << endl;
            PlaySound();
            break;
        case PowerCommand:
            //cout << "Power Switch Action " << v[2] << endl;
            PowerControl((PowerType) v[2]);
            break;
        case CamCommand:
            //cout << "Cam Aciton" << v[2] << "," << v[3] << endl;
            maestroSetMultiTarget(fd, 2, 0x0C, v[2], v[3]);
            break;
    }
}

void UICommandReceive(vector<int> v) {
    //cout << "UI Command Rec: ";

    CommandType ct = (CommandType) v[0];
    switch (ct) {
        case WheelCommand:
            //cout << "Wheel Aciton " << endl;
            needMoveActType = (MoveActType) v[1];
            //CollisionProtect();
            Move(needMoveActType);
            break;
        case ArmCommand:
            //cout << "Arm Aciton" << endl;
            ArmAction((ArmActType) v[1]);
            break;
        case ModeCommand:
        {
            //cout << "Auto Mode Switch Aciton" << endl;
            RobotMode robotMode = ((RobotMode) v[1]);
            if (robotMode == AutoMode) {
                StartAutoMode();
                //StartSonarAutoMode();
            } else if (robotMode == ManualMode) {
                StopAutoMode();
                StopSonarAutoMode();
                //cout << "Stop Auto Mode!" << endl;
            }
            break;
        }
        case VisionCommand:
        {
            //cout << "Motion Switch Aciton" << endl;
            int motionCmd = v[1];
            if (motionCmd == 0)
                KillMotion();
            else if (motionCmd == 1) {
                StartMotion();
            }
            break;
        }
        case AudioCommand:
            //cout << "Play Alarm Aciton" << endl;
            PlaySound();
            break;
        case SettingCommand:
            //cout << "Change Parameter Aciton" << endl;
            SetBasicParameters(v);
            break;
    }
}

void error(char *msg) {
    perror(msg);
    exit(1);
}

void sendData(int sockfd, int x) {
    int n;

    char buffer[32];
    sprintf(buffer, "%d\n", x);
    if ((n = write(sockfd, buffer, strlen(buffer))) < 0)
        error(const_cast<char *> ("ERROR writing to socket"));
    buffer[n] = '\0';
}

vector<int> getData(int sockfd) {
    vector<int> cmd;
    char buffer[256];
    int n;
    if ((n = read(sockfd, buffer, 255)) < 0)
        error(const_cast<char *> ("ERROR reading from socket"));

    buffer[n] = '\0';
    string s(buffer);
    //cout << "send from client: " << s << endl;
    stringstream ss(s);
    int i;
    while (ss >> i) {
        //cout << i << endl;
        cmd.push_back(i);
        if (ss.peek() == ',')
            ss.ignore();
    }
    return cmd; //atoi(buffer);
}

int startListen(int portno) {
    vector<int> data;
    int sockfd, newsockfd, clilen;
    struct sockaddr_in serv_addr, cli_addr;

    printf("using port #%d\n", portno);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error(const_cast<char *> ("ERROR opening socket"));

    //unbind socket option
    int myFlag = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &myFlag, sizeof (int));
    bzero((char *) &serv_addr, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0)
        error(const_cast<char *> ("ERROR on binding"));
    listen(sockfd, 5);
    clilen = sizeof (cli_addr);

    //Initialize();
    //sleep(5);
    //--- infinite wait on a connection ---
    while (true) {
        printf("waiting for new client...\n");

        if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
                (socklen_t*) & clilen)) < 0)
            error(const_cast<char *> ("ERROR on accept"));

        printf("opened new communication with client(%s:%d)\n",
                inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        while (true) {
            //---- wait for a number from client ---
            data = getData(newsockfd);
            //printf( "got %d\n", data[0]);
            if (data[0] < 0)
                break;

            CommandType commandType = (CommandType) data[0];

            switch (commandType) {
                case ControllerCommand:
                    ControllerCommandReceive(data);
                    break;

                default:
                    UICommandReceive(data);
                    break;
            }
            //--- send new data back ---
            //printf( "sending back %d\n", data );
            //sendData( newsockfd, data );
        }
        Move(Stop);
        delay(100);
        PowerControl(ArmLeftPowerOff);
        delay(100);
        PowerControl(ArmRightPowerOff);
        delay(100);

        printf("Disconnect from client(%s:%d).\n",
                inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        close(newsockfd);
        //--- if -2 sent by client, we can quit ---
        if (data[0] == -2)
            break;
    }
    RobotTerminate();
    return 0;
}

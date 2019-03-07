/*
 * Robot.cpp
 *
 *  Created on: Nov 11, 2014
 *      Author: dev
 */
#include "Robot.h"

int centerValue = 35;
int fd = 0;
int fdArduino = 0;
vector<int> sonarDistance;
int cc = 0; //all white / black area counts

int trackingSensorLL = 12;
int trackingSensorL = 3;
int trackingSensorM = 2;
int trackingSensorR = 0;
int trackingSensorRR = 7;

int ll = 1, l = 1, m = 1, r = 1, rr = 1;
//int sonarPin[] = { 1, 4, 5, 6 };
bool isArmPressed = false;
bool IsAutoMode = false;
bool IsSonarAutoMode = false;
char currentTime[80] = {0};
struct timeval t11, t22;

extern bool showInfo;
extern int LeftWheelBasePower;
extern int RightWheelBasePower;
extern double WheelTurnPowerLevels[];

MoveActType needMoveActType = Stop;
MoveActType currentMoveActType = Stop;
MoveActType lastMoveActType = Stop;

ServoDefine Servos[24];
RobotMessage RobotMsg;

void SetBasicParameters(vector<int> paras) {
    LeftWheelBasePower = paras[1];
    RightWheelBasePower = paras[2];

    for (int i = 0; i < 8; i++) {
        if (i % 2 == 0) {
            WheelTurnPowerLevels[i] = paras[3] / 10.0;
        } else if (i % 2 == 1) {
            WheelTurnPowerLevels[i] = paras[4] / 10.0;
        }
    }
}

int WaitForPin(int pin, int level, int timeOut) {
    struct timeval now, start;
    int done;
    long micros;
    gettimeofday(&start, NULL);
    micros = 0;
    done = 0;
    while (!done) {
        gettimeofday(&now, NULL);
        if (now.tv_sec > start.tv_sec) {
            micros = 1000000L;
        } else {
            micros = 0;
        }

        micros = micros + (now.tv_usec - start.tv_usec);
        if (micros > timeOut || digitalRead(pin) == level) {
            done = 1;
        }
    }
    return micros;
}

void DetectDistance(int &pin, int &dist) {
    int pulseWidth;
    while (true) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
        delayMicroseconds(2);
        digitalWrite(pin, HIGH);
        delayMicroseconds(5);
        digitalWrite(pin, LOW);
        pinMode(pin, INPUT);
        WaitForPin(pin, HIGH, 2000);
        if (digitalRead(pin) == HIGH) {
            pulseWidth = WaitForPin(pin, LOW, 37000L);
            if (digitalRead(pin) == LOW) {
                dist = pulseWidth / 29 / 2;
            }
        }
        delay(500);
    }
}

void DetectDistance1() {
    while (true) {
        sonarDistance = GetSonarValues1(fdArduino);
        for (int i = 0; i < 4; i++) {
            RobotMsg.SonarDistance[i] = sonarDistance[i];
        }
    }
}

void ShowAllInfo() {
    while (1) {
        ShowTimeNow(currentTime);

        cout << "/////////////////////////////////////////////////////" << endl;
        cout << "DATE_TIME_NOW" << endl;
        cout << currentTime << endl;
        cout << endl;
        cout << "ROBOT_INFO   " << endl;
        cout << "RobotName\t:" << RobotMsg.RobotName << endl;
        cout << "AutoMode\t:" << *RobotMsg.AutoMode << endl;
        cout << "ArmAction\t:" << RobotMsg.RobotArmAction << endl;
        cout << "WheelAction\t:" << RobotMsg.RobotMoveAction << "("
                << Servos[0].Target << "," << Servos[1].Target << ")" << endl;
        cout << endl;
        cout << "IR_TRACKER   " << endl;
        cout << *RobotMsg.IrTracker[0] << "," << *RobotMsg.IrTracker[1] << ","
                << *RobotMsg.IrTracker[2] << "," << *RobotMsg.IrTracker[3]
                << "," << *RobotMsg.IrTracker[4] << endl;
        cout << endl;
        cout << "SONAR_SENSOR " << endl;
        cout << "\t" << RobotMsg.SonarDistance[1] << "\n\t\t"
                //<< RobotMsg.SonarDistance[4] << "\t\t"
                << RobotMsg.SonarDistance[3] << "\n"
                << RobotMsg.SonarDistance[0] << "\t\t"
                << RobotMsg.SonarDistance[2] << endl; // << "\n\t"
        //<< RobotMsg.SonarDistance[1] << endl;

        delay(50);
    }
}

void ShowTimeNow(char *strTime) {
    timeval curTime;
    gettimeofday(&curTime, NULL);
    int milli = curTime.tv_usec / 1000;
    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
    sprintf(strTime, "%s.%03d", buffer, milli);
}

void Initialize() {
    extern string devName;
    extern string arduinoName;

    fd = maestroOpenConnect(devName.c_str());
    //fdArduino = OpenArduino(arduinoName.c_str());
    fdArduino = OpenArduino1(arduinoName.c_str());
    if (fd < 0 || fdArduino < 0) {
        cout << "Open Device Error: " << endl;
        cout << "maestro: " << fd << endl;
        cout << "arduino: " << fdArduino << endl;
    }

    RobotMsg.RobotName = "Robot Car";
    RobotMsg.AutoMode = &IsAutoMode;
    RobotMsg.RobotMoveAction = ROBOT_STOP;
    RobotMsg.RobotArmAction = ROBOT_ARM_NORMAL;
    RobotMsg.SonarDistance[0] = 0;
    RobotMsg.SonarDistance[1] = 0;
    RobotMsg.SonarDistance[2] = 0;
    RobotMsg.SonarDistance[3] = 0;
    //RobotMsg.SonarDistance[4] = 0;
    //RobotMsg.SonarDistance[5] = 0;
    RobotMsg.IrTracker[0] = &ll;
    RobotMsg.IrTracker[1] = &l;
    RobotMsg.IrTracker[2] = &m;
    RobotMsg.IrTracker[3] = &r;
    RobotMsg.IrTracker[4] = &rr;

    wiringPiSetup();

    pinMode(trackingSensorLL, INPUT);
    pinMode(trackingSensorL, INPUT);
    pinMode(trackingSensorM, INPUT);
    pinMode(trackingSensorR, INPUT);
    pinMode(trackingSensorRR, INPUT);

    thread threadSonar(DetectDistance1);
    threadSonar.detach();

    /*
     thread mySonarThread0(DetectDistance, ref(sonarPin[0]),
     ref(RobotMsg.SonarDistance[0]));
     thread mySonarThread1(DetectDistance, ref(sonarPin[1]),
     ref(RobotMsg.SonarDistance[1]));
     thread mySonarThread2(DetectDistance, ref(sonarPin[2]),
     ref(RobotMsg.SonarDistance[2]));
     thread mySonarThread3(DetectDistance, ref(sonarPin[3]),
     ref(RobotMsg.SonarDistance[3]));
     mySonarThread0.detach();
     mySonarThread1.detach();
     mySonarThread2.detach();
     mySonarThread3.detach();
     */
    if (showInfo) {
        thread showAllInfoThread(ShowAllInfo);
        showAllInfoThread.detach();
    }

    /*
     * Wheel Servo
     */
    Servos[0].ServoName = "LeftWheel";
    Servos[0].Channel = 0x16;
    Servos[0].Target = 0; //DEFAULT_LEFT_WHEEL_TARGET;

    Servos[1].ServoName = "RightWheel";
    Servos[1].Channel = 0x17;
    Servos[1].Target = 0; //DEFAULT_RIGHT_WHEEL_TARGET;

    //Left Arm
    Servos[2].ServoName = "LeftA";
    Servos[2].Channel = 0x00;
    Servos[2].Target = 0;

    Servos[3].ServoName = "LeftB";
    Servos[3].Channel = 0x01;
    Servos[3].Target = 0;

    Servos[4].ServoName = "LeftC";
    Servos[4].Channel = 0x02;
    Servos[4].Target = 0;

    Servos[5].ServoName = "LeftD";
    Servos[5].Channel = 0x03;
    Servos[5].Target = 0;

    //Right Arm
    Servos[6].ServoName = "RightA";
    Servos[6].Channel = 0x06;
    Servos[6].Target = 0;

    Servos[7].ServoName = "RightB";
    Servos[7].Channel = 0x07;
    Servos[7].Target = 0;

    Servos[8].ServoName = "RightC";
    Servos[8].Channel = 0x08;
    Servos[8].Target = 0;

    Servos[9].ServoName = "RightD";
    Servos[9].Channel = 0x09;
    Servos[9].Target = 0;

    Servos[10].ServoName = "NeckTop";
    Servos[10].Channel = 0x0C;
    Servos[10].Target = 0;

    Servos[11].ServoName = "NeckBotom";
    Servos[11].Channel = 0x0D;
    Servos[11].Target = 0;

    Move(Stop);
    //ArmAction(ArmLeftPowerOff);
    //ArmAction(ArmRightPowerOff);
    PowerControl(ArmLeftPowerOff);
    PowerControl(ArmRightPowerOff);
}

void RobotTerminate() {
    maestroDisconnect(fd);
    //CloseArduino(fdArduino);
    CloseArduino1(fdArduino);
}

void OneServoTest(unsigned char ch, unsigned short target) {
    maestroSetTarget(fd, ch, target);
}

void SetWheelTarget(MoveActType moveActType) {
    switch (moveActType) {
        case Stop:
            Servos[0].Target = DEFAULT_LEFT_WHEEL_TARGET;
            Servos[1].Target = DEFAULT_RIGHT_WHEEL_TARGET;
            RobotMsg.RobotMoveAction = ROBOT_STOP;
            break;
        case Forward:
            Servos[0].Target = DEFAULT_LEFT_WHEEL_TARGET
                    - LeftWheelBasePower * WheelTurnPowerLevels[0];
            Servos[1].Target = DEFAULT_RIGHT_WHEEL_TARGET
                    + RightWheelBasePower * WheelTurnPowerLevels[1];
            RobotMsg.RobotMoveAction = ROBOT_FORWARD;
            break;
        case Back:
            Servos[0].Target = DEFAULT_LEFT_WHEEL_TARGET
                    + LeftWheelBasePower * WheelTurnPowerLevels[2];
            Servos[1].Target = DEFAULT_RIGHT_WHEEL_TARGET
                    - RightWheelBasePower * WheelTurnPowerLevels[3];
            RobotMsg.RobotMoveAction = ROBOT_BACK;
            break;
        case LeftTurn:
            Servos[0].Target = DEFAULT_LEFT_WHEEL_TARGET
                    + LeftWheelBasePower * WheelTurnPowerLevels[4];
            Servos[1].Target = DEFAULT_RIGHT_WHEEL_TARGET
                    + RightWheelBasePower * WheelTurnPowerLevels[5];
            RobotMsg.RobotMoveAction = ROBOT_LEFT_TURN;
            break;
        case RightTurn:
            Servos[0].Target = DEFAULT_LEFT_WHEEL_TARGET
                    - LeftWheelBasePower * WheelTurnPowerLevels[6];
            Servos[1].Target = DEFAULT_RIGHT_WHEEL_TARGET
                    - RightWheelBasePower * WheelTurnPowerLevels[7];
            RobotMsg.RobotMoveAction = ROBOT_RIGHT_TURN;
            break;
        case LeftLevel1:
            Servos[0].Target = DEFAULT_LEFT_WHEEL_TARGET
                    - LeftWheelBasePower * WheelTurnPowerLevels[8];
            Servos[1].Target = DEFAULT_RIGHT_WHEEL_TARGET
                    + RightWheelBasePower * WheelTurnPowerLevels[9];
            RobotMsg.RobotMoveAction = ROBOT_LEFT_LEVEL1;
            break;
        case LeftLevel2:
            Servos[0].Target = DEFAULT_LEFT_WHEEL_TARGET
                    - LeftWheelBasePower * WheelTurnPowerLevels[10];
            Servos[1].Target = DEFAULT_RIGHT_WHEEL_TARGET
                    + RightWheelBasePower * WheelTurnPowerLevels[11];
            RobotMsg.RobotMoveAction = ROBOT_LEFT_LEVEL2;
            break;
        case LeftLevel3:
            Servos[0].Target = DEFAULT_LEFT_WHEEL_TARGET
                    - LeftWheelBasePower * WheelTurnPowerLevels[12];
            Servos[1].Target = DEFAULT_RIGHT_WHEEL_TARGET
                    + RightWheelBasePower * WheelTurnPowerLevels[13];
            RobotMsg.RobotMoveAction = ROBOT_LEFT_LEVEL3;
            break;
        case LeftLevel4:
            Servos[0].Target = DEFAULT_LEFT_WHEEL_TARGET
                    - LeftWheelBasePower * WheelTurnPowerLevels[14];
            Servos[1].Target = DEFAULT_RIGHT_WHEEL_TARGET
                    + RightWheelBasePower * WheelTurnPowerLevels[15];
            RobotMsg.RobotMoveAction = ROBOT_LEFT_LEVEL4;
            break;
        case RightLevel1:
            Servos[0].Target = DEFAULT_LEFT_WHEEL_TARGET
                    - LeftWheelBasePower * WheelTurnPowerLevels[16];
            Servos[1].Target = DEFAULT_RIGHT_WHEEL_TARGET
                    + RightWheelBasePower * WheelTurnPowerLevels[17];
            RobotMsg.RobotMoveAction = ROBOT_RIGHT_LEVEL1;
            break;
        case RightLevel2:
            Servos[0].Target = DEFAULT_LEFT_WHEEL_TARGET
                    - LeftWheelBasePower * WheelTurnPowerLevels[18];
            Servos[1].Target = DEFAULT_RIGHT_WHEEL_TARGET
                    + RightWheelBasePower * WheelTurnPowerLevels[19];
            RobotMsg.RobotMoveAction = ROBOT_RIGHT_LEVEL2;
            break;
        case RightLevel3:
            Servos[0].Target = DEFAULT_LEFT_WHEEL_TARGET
                    - LeftWheelBasePower * WheelTurnPowerLevels[20];
            Servos[1].Target = DEFAULT_RIGHT_WHEEL_TARGET
                    + RightWheelBasePower * WheelTurnPowerLevels[21];
            RobotMsg.RobotMoveAction = ROBOT_RIGHT_LEVEL3;
            break;
        case RightLevel4:
            Servos[0].Target = DEFAULT_LEFT_WHEEL_TARGET
                    - LeftWheelBasePower * WheelTurnPowerLevels[22];
            Servos[1].Target = DEFAULT_RIGHT_WHEEL_TARGET
                    + RightWheelBasePower * WheelTurnPowerLevels[23];
            RobotMsg.RobotMoveAction = ROBOT_RIGHT_LEVEL4;
            break;
        case LeftTurnFineTune:
            Servos[0].Target = DEFAULT_LEFT_WHEEL_TARGET
                    - LeftWheelBasePower * 0.85;
            Servos[1].Target = DEFAULT_RIGHT_WHEEL_TARGET
                    + RightWheelBasePower * 0.9;
            RobotMsg.RobotMoveAction = "SonarLeftFinetune";
            break;
        case RightTurnFineTune:
            Servos[0].Target = DEFAULT_LEFT_WHEEL_TARGET - LeftWheelBasePower * 0.9;
            Servos[1].Target = DEFAULT_RIGHT_WHEEL_TARGET
                    + RightWheelBasePower * 0.85;
            RobotMsg.RobotMoveAction = "SonarRightFinetune";
            break;
    }
}

void Move(MoveActType moveActType) {
    currentMoveActType = moveActType;

    //if (currentMoveActType == lastMoveActType)
    //	return;

    SetWheelTarget(currentMoveActType);

    maestroSetMultiTarget(fd, 2, Servos[0].Channel, Servos[0].Target,
            Servos[1].Target);
    lastMoveActType = currentMoveActType;
}

void ArmAction(ArmActType armActType) {
    //cout << "Arm: " << armActType << endl;
    switch (armActType) {
        case HoldLeft:
            RobotMsg.RobotArmAction = ROBOT_ARM_GRAB;
            Servos[5].Target = 9984;
            maestroSetTarget(fd, Servos[5].Channel, Servos[5].Target);
            break;
        case HoldRight:
            RobotMsg.RobotArmAction = ROBOT_ARM_GRAB;
            Servos[9].Target = 9984;
            maestroSetTarget(fd, Servos[9].Channel, Servos[9].Target);
            break;
        case LooseLeft:
            Servos[5].Target = 6400;
            maestroSetTarget(fd, Servos[5].Channel, Servos[5].Target);
            break;
        case LooseRight:
            Servos[9].Target = 6400;
            maestroSetTarget(fd, Servos[9].Channel, Servos[9].Target);
            break;
        case ArmUpFront1Left:
            Servos[2].Target = 2816;
            Servos[3].Target = 2816;
            Servos[4].Target = 7600;
            maestroSetMultiTarget(fd, 3, Servos[2].Channel, Servos[2].Target,
                    Servos[3].Target, Servos[4].Target);
            break;
        case ArmUpFront1Right:
            Servos[6].Target = 9984;
            Servos[7].Target = 9984;
            Servos[8].Target = 5200;
            maestroSetMultiTarget(fd, 3, Servos[6].Channel, Servos[6].Target,
                    Servos[7].Target, Servos[8].Target);
            break;
        case ArmUpFront2Left:
            Servos[2].Target = 2816;
            Servos[3].Target = 2816;
            Servos[4].Target = 9984;
            maestroSetMultiTarget(fd, 3, Servos[2].Channel, Servos[2].Target,
                    Servos[3].Target, Servos[4].Target);
            break;
        case ArmUpFront2Right:
            Servos[6].Target = 9984;
            Servos[7].Target = 9984;
            Servos[8].Target = 2816;
            maestroSetMultiTarget(fd, 3, Servos[6].Channel, Servos[6].Target,
                    Servos[7].Target, Servos[8].Target);
            break;
        case ArmUpHorizon1Left:
            Servos[2].Target = 2816;
            Servos[3].Target = 6400;
            Servos[4].Target = 6400;
            maestroSetMultiTarget(fd, 3, Servos[2].Channel, Servos[2].Target,
                    Servos[3].Target, Servos[4].Target);
            break;
        case ArmUpHorizon1Right:
            Servos[6].Target = 9984;
            //Servos[7].Target = 6400;
            Servos[7].Target = 8192;
            Servos[8].Target = 6400;
            maestroSetMultiTarget(fd, 3, Servos[6].Channel, Servos[6].Target,
                    Servos[7].Target, Servos[8].Target);
            break;
        case ArmUpHorizon2Left:
            Servos[2].Target = 4600;
            Servos[3].Target = 6400;
            Servos[4].Target = 6400;
            maestroSetMultiTarget(fd, 3, Servos[2].Channel, Servos[2].Target,
                    Servos[3].Target, Servos[4].Target);
            break;
        case ArmUpHorizon2Right:
            Servos[6].Target = 8200;
            Servos[7].Target = 6400;
            Servos[8].Target = 6400;
            maestroSetMultiTarget(fd, 3, Servos[6].Channel, Servos[6].Target,
                    Servos[7].Target, Servos[8].Target);
            break;
        case ArmDownLeft:
            Servos[2].Target = 6400;
            Servos[3].Target = 2816;
            Servos[4].Target = 7600;
            maestroSetMultiTarget(fd, 3, Servos[2].Channel, Servos[2].Target,
                    Servos[3].Target, Servos[4].Target);
            break;
        case ArmDownRight:
            Servos[6].Target = 6200;
            Servos[7].Target = 9984;
            Servos[8].Target = 5200;
            maestroSetMultiTarget(fd, 3, Servos[6].Channel, Servos[6].Target,
                    Servos[7].Target, Servos[8].Target);
            break;
    }
}
bitset<5> lastBinCurrent;
int stopCount = 0;
bool isAtStartPoint = true;

void MovementCheck() {
    ll = digitalRead(trackingSensorLL);
    l = digitalRead(trackingSensorL);
    m = digitalRead(trackingSensorM);
    r = digitalRead(trackingSensorR);
    rr = digitalRead(trackingSensorRR);

    int IrArrayData[] = {ll, l, m, r, rr};
    string s;
    for (int i = 0; i < 5; i++) {
        s += to_string(IrArrayData[i]);
    }
    //cout << s << endl;
    bitset<5> binCurrent(s);
    //TODO: mode transfer
    //if (binCurrent == binSignalAllWhite || binCurrent == binSignalAllWhite) {
    if (binCurrent == binSignalAllBlack) {
        cc++;
        if (cc > 80) {
            cc = 0;
            if (!IsSonarAutoMode) {
                StartSonarAutoMode();
                StopAutoMode();
            }
        }
        return;
    }

    if (binCurrent == binSignalLeftLevel3_1 || binCurrent == binSignalRightLevel3_1) {
        stopCount++;
        //cout << stopCount << endl;
        if (stopCount > 40) {
            stopCount = 0;
            //cout << stopCount << endl;

            needMoveActType = Stop;
            Move(needMoveActType);
            delay(500);
            needMoveActType = Back;
            Move(needMoveActType);
            delay(200);
            needMoveActType = Stop;
            Move(needMoveActType);
            delay(500);
            needMoveActType = LeftTurn;
            Move(needMoveActType);
            delay(2500);
            needMoveActType = Stop;
            Move(needMoveActType);
            delay(500);
            needMoveActType = Back;
            Move(needMoveActType);
            delay(500);
            needMoveActType = Stop;
            Move(needMoveActType);
            StopAutoMode();
            return;
        }
    }

    if (binCurrent == binSignalCenter || binCurrent == binSignalCenter_1) {
        needMoveActType = Forward;
        stopCount = 0;
        return;
    }
    if (binCurrent == binSignalLeftLevel1
            || binCurrent == binSignalLeftLevel1_1) {
        needMoveActType = LeftLevel1;
        stopCount = 0;
        return;
    }
    if (binCurrent == binSignalLeftLevel2
            || binCurrent == binSignalLeftLevel2_1) {
        needMoveActType = LeftLevel2;
        stopCount = 0;
        return;
    }
    if (binCurrent == binSignalLeftLevel3
            || binCurrent == binSignalLeftLevel3_1) {
        needMoveActType = LeftLevel3;
        return;
    }
    if (binCurrent == binSignalLeftLevel4
            || binCurrent == binSignalLeftLevel4_1) {
        needMoveActType = LeftLevel4;
        stopCount = 0;
        return;
    }
    if (binCurrent == binSignalRightLevel1
            || binCurrent == binSignalRightLevel1_1) {
        needMoveActType = RightLevel1;
        stopCount = 0;
        return;
    }
    if (binCurrent == binSignalRightLevel2
            || binCurrent == binSignalRightLevel2_1) {
        needMoveActType = RightLevel2;
        stopCount = 0;
        return;
    }
    if (binCurrent == binSignalRightLevel3
            || binCurrent == binSignalRightLevel3_1) {
        needMoveActType = RightLevel3;
        return;
    }
    if (binCurrent == binSignalRightLevel4
            || binCurrent == binSignalRightLevel4_1) {
        needMoveActType = RightLevel4;
        stopCount = 0;
        return;
    }
}

void PerformAuto() {
    RobotMsg.RobotMoveAction = ROBOT_AUTO;
    gettimeofday(&t11, NULL);

    if (LeftWheelBasePower < 2000 || RightWheelBasePower < 2000) {
        LeftWheelBasePower = 2000;
        RightWheelBasePower = 2000;
    }

    //TODO: AUTO MODE STARTUP LOGIC

    if (isAtStartPoint) {
        needMoveActType = RightLevel2;
        Move(needMoveActType);
        //delay(200);
        isAtStartPoint = false;
    }

    while (1) {
        if (!IsAutoMode) {
            break;
        }

        MovementCheck();
        //CollisionProtect();
        Move(needMoveActType);
        delay(20);

    }
    //Move(Stop);
}

void StartAutoMode() {
    cout << "Start Auto Mode!" << endl;

    IsAutoMode = true;
    thread(PerformAuto).detach();
}

void StopAutoMode() {
    IsAutoMode = false;

    LeftWheelBasePower = 2000;
    RightWheelBasePower = 2000;
    if (IsAutoMode == false && IsSonarAutoMode == false)
        isArmPressed = false;
}

void MovementCheckSonar() {
    //RobotMsg.SonarDistance[]
    //DetectDistance1();
    ll = digitalRead(trackingSensorLL);
    l = digitalRead(trackingSensorL);
    m = digitalRead(trackingSensorM);
    r = digitalRead(trackingSensorR);
    rr = digitalRead(trackingSensorRR);

    int IrArrayData[] = {ll, l, m, r, rr};
    string s;
    for (int i = 0; i < 5; i++) {
        s += to_string(IrArrayData[i]);
    }
    //cout << s << endl;
    bitset<5> binCurrent(s);

    if (binCurrent == binSignalCenter_1 || binCurrent == binSignalLeftLevel1_1
            || binCurrent == binSignalRightLevel1_1
            || binCurrent == binSignalLeftLevel2_1
            || binCurrent == binSignalRightLevel2_1) {
        //|| binCurrent == binSignalLeftLevel3_1
        //|| binCurrent == binSignalRightLevel3_1) {
        //todo: formal area need uncomment
        StartAutoMode();
        StopSonarAutoMode();
    }

    if (binCurrent == binSignalAllWhite) {
        if (!isArmPressed) {
            isArmPressed = true;
            thread pressThread(PressLightSwitch);
            pressThread.detach();
        }

    }

    if (sonarDistance[1] < 25) {
        needMoveActType = Stop;
        Move(needMoveActType);
        delay(200);
        needMoveActType = Back;
        Move(needMoveActType);
        delay(500);
        return;
    }

    if (sonarDistance[1] < 30 || sonarDistance[3] < 30) {
        needMoveActType = LeftTurn;
        Move(needMoveActType);
        delay(40);
    } else if (sonarDistance[1] < 30 || sonarDistance[0] < 20) {
        //needMoveActType = RightTurn;
        //needMoveActType = RightTurnFineTune;
        needMoveActType = RightLevel1;
        Move(needMoveActType);
        delay(40);
    } else {
        if (sonarDistance[2] < centerValue) {
            needMoveActType = LeftTurnFineTune;
            Move(needMoveActType);
        } else if (sonarDistance[2] > centerValue) {
            needMoveActType = RightTurnFineTune;
            Move(needMoveActType);
        } else {
            needMoveActType = Forward;
            Move(needMoveActType);
        }
        delay(40);
    }
}

void PerformSonarAuto() {
    RobotMsg.RobotMoveAction = ROBOT_AUTO;
    gettimeofday(&t11, NULL);
    //centerValue = RobotMsg.SonarDistance[2];
    LeftWheelBasePower *= 0.70;
    RightWheelBasePower *= 0.70;
    while (1) {
        if (!IsSonarAutoMode) {
            break;
        }
        MovementCheckSonar();
    }
    Move(Stop);
}

void StartSonarAutoMode() {
    cout << "Start Sonar Auto Mode!" << endl;
    IsSonarAutoMode = true;
    //centerValue = RobotMsg.SonarDistance[3];
    thread(PerformSonarAuto).detach();
}

void StopSonarAutoMode() {
    IsSonarAutoMode = false;
    LeftWheelBasePower = 2000;
    RightWheelBasePower = 2000;
}

void StartMotion() {
    system("sudo motion &");
}

void KillMotion() {
    system("sudo killall motion &");
}

void PlaySound() {
    system("aplay /home/pi/tmp/alarm.wav &");
}

void CollisionProtect() {
    if (RobotMsg.SonarDistance[0] < DISTANCE_LIMIT
            && RobotMsg.SonarDistance[1] < DISTANCE_LIMIT
            && RobotMsg.SonarDistance[2] < DISTANCE_LIMIT) {
        //&& RobotMsg.SonarDistance[3] < DISTANCE_LIMIT) {
        needMoveActType = Stop;
        return;
    }
    /*
     if(RobotMsg.SonarDistance[0] <= DISTANCE_LIMIT){
     needMoveActType = LeftTurn;
     return;
     }
     if(RobotMsg.SonarDistance[2] <= DISTANCE_LIMIT){
     needMoveActType = LeftLevel1;
     return;
     }
     if(RobotMsg.SonarDistance[2] > (DISTANCE_LIMIT + 20)){
     needMoveActType = RightLevel1;
     }
     */
}

void PowerControl(PowerType powerType) {
    switch (powerType) {
        case WheelPowerOn:
            maestroSetMultiTarget(fd, 2, Servos[0].Channel, 6000, 6000);
            break;
        case WheelPowerOff:
            maestroSetMultiTarget(fd, 2, Servos[0].Channel, 0, 0);
            break;
        case ArmLeftPowerOn:
            RobotMsg.RobotArmAction = ROBOT_ARM_NORMAL;
            Servos[2].Target = 6400;
            Servos[3].Target = 2816;
            Servos[4].Target = 7600;
            Servos[5].Target = 9984;
            maestroSetMultiTarget(fd, 4, Servos[2].Channel, Servos[2].Target,
                    Servos[3].Target, Servos[4].Target, Servos[5].Target);

            break;
        case ArmLeftPowerOff:
            Servos[2].Target = 0;
            Servos[3].Target = 0;
            Servos[4].Target = 0;
            Servos[5].Target = 0;
            maestroSetMultiTarget(fd, 4, Servos[2].Channel, Servos[2].Target,
                    Servos[3].Target, Servos[4].Target, Servos[5].Target);

            break;
        case ArmRightPowerOn:
            Servos[6].Target = 6200;
            Servos[7].Target = 9984;
            Servos[8].Target = 5200;
            Servos[9].Target = 9984;
            maestroSetMultiTarget(fd, 4, Servos[6].Channel, Servos[6].Target,
                    Servos[7].Target, Servos[8].Target, Servos[9].Target);

            break;
        case ArmRightPowerOff:
            Servos[6].Target = 0;
            Servos[7].Target = 0;
            Servos[8].Target = 0;
            Servos[9].Target = 0;
            maestroSetMultiTarget(fd, 4, Servos[6].Channel, Servos[6].Target,
                    Servos[7].Target, Servos[8].Target, Servos[9].Target);

            break;
    }
}

void PressLightSwitch() {
    ArmAction(ArmUpFront1Right);
    delay(2000);
    ArmAction(ArmUpHorizon1Right);
    delay(1000);
    ArmAction(ArmUpFront1Right);
    delay(500);
    ArmAction(ArmDownRight);
    delay(500);
    PowerControl(ArmRightPowerOff);
    //delay(50);
    //isArmPressed = false;
}
/*
 * Robot.h
 *
 *  Created on: Nov 11, 2014
 *      Author: dev
 */

#ifndef ROBOT_H_
#define ROBOT_H_

#include <thread>
#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <sys/time.h>
#include <wiringPi.h>
#include "ServoControl.h"
#include "Sonar.h"

using namespace std;

enum CommandType{
	WheelCommand = 0,
	ArmCommand,
	ModeCommand,
	VisionCommand,
	AudioCommand,
	SettingCommand,
	ControllerCommand,
	PowerCommand,
	CamCommand
};

enum PowerType{
	WheelPowerOn = 0,
	WheelPowerOff,
	ArmLeftPowerOn,
	ArmLeftPowerOff,
	ArmRightPowerOn,
	ArmRightPowerOff
};

enum MoveActType {
	Stop = 0,
	Forward,
	Back,
	LeftTurn,
	RightTurn,
	LeftLevel1,
	LeftLevel2,
	LeftLevel3,
	LeftLevel4,
	RightLevel1,
	RightLevel2,
	RightLevel3,
	RightLevel4,
	LeftTurnFineTune,
	RightTurnFineTune,
};

enum RobotMode{
	ManualMode = 0,
	AutoMode
};

enum SettingType{
	WheelBasePower = 0,
	WheelLevel,
};

enum ArmActType {
	ArmDownLeft = 0,
	ArmUpFront1Left,
	ArmUpHorizon1Left,
	ArmUpFront2Left,
	ArmUpHorizon2Left,
	HoldLeft,
	LooseLeft,

	ArmDownRight = 10,
	ArmUpFront1Right,
	ArmUpHorizon1Right,
	ArmUpFront2Right,
	ArmUpHorizon2Right,
	HoldRight,
	LooseRight
};

struct ServoDefine {
	string ServoName;
	unsigned char Channel;
	unsigned short Target;
};

struct RobotMessage {
	string RobotName;
	string RobotMoveAction;
	string RobotArmAction;
	int SonarDistance[4];
	int *IrTracker[5];
	bool *AutoMode;
};

const int DEFAULT_LEFT_WHEEL_TARGET = 6000;
const int DEFAULT_RIGHT_WHEEL_TARGET = 6000;
const int DISTANCE_LIMIT = 30;
const string ROBOT_STOP = "STOP";
const string ROBOT_FORWARD = "FORWARD";
const string ROBOT_BACK = "BACK";
const string ROBOT_LEFT_TURN = "LEFT TURN";
const string ROBOT_RIGHT_TURN = "RIGHT TURN";
/*
const string ROBOT_LEFT_FINETUNE = "LEFT FINE-TUNE";
const string ROBOT_RIGHT_FINETUNE = "RIGHT FINE-TUNE";
*/
const string ROBOT_LEFT_LEVEL1="LEFT LEVEL 1";
const string ROBOT_LEFT_LEVEL2="LEFT LEVEL 2";
const string ROBOT_LEFT_LEVEL3="LEFT LEVEL 3";
const string ROBOT_LEFT_LEVEL4="LEFT LEVEL 4";
const string ROBOT_RIGHT_LEVEL1="RIGHT LEVEL 1";
const string ROBOT_RIGHT_LEVEL2="RIGHT LEVEL 2";
const string ROBOT_RIGHT_LEVEL3="RIGHT LEVEL 3";
const string ROBOT_RIGHT_LEVEL4="RIGHT LEVEL 4";
const string ROBOT_ARM_NORMAL = "ARM NORMAL";
const string ROBOT_ARM_GRAB = "ARM GRAB";
const string ROBOT_AUTO = "AUTO";

const bitset<5> binSignalAllWhite("00000");
const bitset<5> binSignalAllBlack("11111");

const bitset<5> binSignalLeftLevel4("10000");
const bitset<5> binSignalLeftLevel3("11000");
const bitset<5> binSignalLeftLevel2("01000");
const bitset<5> binSignalLeftLevel1("01100");
const bitset<5> binSignalCenter("00100");
const bitset<5> binSignalRightLevel1("00110");
const bitset<5> binSignalRightLevel2("00010");
const bitset<5> binSignalRightLevel3("00011");
const bitset<5> binSignalRightLevel4("00001");

const bitset<5> binSignalLeftLevel4_1("01111");
const bitset<5> binSignalLeftLevel3_1("00111");
const bitset<5> binSignalLeftLevel2_1("10111");
const bitset<5> binSignalLeftLevel1_1("10011");
const bitset<5> binSignalCenter_1("11011");
const bitset<5> binSignalRightLevel1_1("11001");
const bitset<5> binSignalRightLevel2_1("11101");
const bitset<5> binSignalRightLevel3_1("11100");
const bitset<5> binSignalRightLevel4_1("11110");

void Initialize();

void ShowTimeNow(char *strTime);

void RobotTerminate();

void OneServoTest(unsigned char ch, unsigned short target);

void Move(MoveActType moveActType);

void ArmAction(ArmActType armActType);

void MovementCheck();

void PerformAuto();

void StartAutoMode();

void StopAutoMode();

void MovementCheckSonar();

void PerformSonarAuto();

void StartSonarAutoMode();

void StopSonarAutoMode();

void SetBasicParameters(vector<int> paras);

int WaitForPin(int pin, int level, int timeOut);

void DetectDistance1();

void DetectDistance(int &pin, int &dist);

void ShowAllInfo();

void StartMotion();

void KillMotion();

void PlaySound();

void CollisionProtect();

void PowerControl(PowerType powerType);

void SetCamPos();

void PressLightSwitch();
#endif /* ROBOT_H_ */

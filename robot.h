#ifndef ROBOT_H_INCLUDED
#define ROBOT_H_INCLUDED


#include "stdio.h"
#include "stdlib.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL2_gfxPrimitives.h"
#include "formulas.h"
#include "time.h"
#include "wall.h"
#include "math.h"

//Setup
void setup_robot(struct Robot *robot);

//Error/Finish Checking
int robot_off_screen(struct Robot * robot);
int checkRobotHitWall(struct Robot * robot, struct Wall * wall);
//int checkRobotHitWalls(struct Robot * robot, struct Wall_collection * head);
int checkRobotHitWalls(struct Robot * robot, SDL_Surface* surface, SDL_Renderer* renderer);
int checkRobotReachedEnd(struct Robot * robot, int x, int y, int width, int height);
void robotCrash(struct Robot * robot);
void robotSuccess(struct Robot * robot, int msec);

void checkRobotSensors(struct Robot * robot, SDL_Surface* surface, int* dest);

//Sensor Checking
int checkRobotSensor(int x, int y, int sensorSensitivityLength, struct Wall * wall) ;
int checkRobotSensorFrontCentreAllWalls(struct Robot * robot, struct Wall_collection * head);
int checkRobotSensorRightAllWalls(struct Robot * robot, struct Wall_collection * head);
int checkRobotSensorLeftAllWalls(struct Robot * robot, struct Wall_collection * head);

//Visual Display
void robotUpdate(struct SDL_Renderer * renderer, struct Robot * robot, SDL_Surface* surface);

//Movement
void robotMotorMove(struct Robot * robot, int crashed);
void robotAutoMotorMove(struct Robot * robot, int f, int l, int r, int isActive);

#endif // ROBOT_H_INCLUDED

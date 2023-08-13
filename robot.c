#include "robot.h"

void setup_robot(struct Robot *robot){
    /*
    IMPORTANT!!!
    We have modified our robot so that the coordinates point to the center, not the top left hand corner.
    hence this is why 10 is added to it.
    
    the +7 and +9 is added for y instead of 10, because there is a bug in our collision detection that
    detects the outside of the maze as solid, so it would instantly crash if the robot
    is too far down. sorry about that.
    */
    /*
    robot->x = 640-10-270 + 10;
    robot->true_x = 640-10-270 + 10;
    robot->angle = 10;
    robot->y = 460 + 7;
    robot->true_y = 460 + 7;
    */
    ///*
    robot->x = 440-10-170 + 15;
    robot->true_x = 440-10-170 + 15;
    robot->angle = 0;
    robot->y = 460 + 9;
    robot->true_y = 460 + 9;
    //*/
    robot->width = ROBOT_WIDTH;
    robot->height = ROBOT_HEIGHT;
    robot->direction = 0;
    robot->currentSpeed = 0;
    robot->currentSpeed2 = 0;
    robot->crashed = 0;
    robot->auto_mode = 0;
    robot->differential_mode = 0;
    robot->display_tangent = 1;
    robot->display_incidence = 1;

    if (robot->differential_mode){
        printf("differential drive mode is on! Use Q and A to move the left wheel forwards or backwards, "
            "and use E and D to control the right wheel.\n\n");
    }
    else 
        printf("Press arrow keys to move manually, or enter to move automatically\n\n");
}

int robot_off_screen(struct Robot * robot){
    if(robot->x < 0 || robot-> y < 0){
        return 0;
    }
    if(robot->x > OVERALL_WINDOW_WIDTH || robot->y > OVERALL_WINDOW_HEIGHT){
        return 0;
    }
    return 1;
}

int checkRobotHitWall(struct Robot * robot, struct Wall * wall) {

    int overlap = checkOverlap(robot->x,robot->width,robot->y,robot->height,
                 wall->x,wall->width,wall->y, wall->height);

    return overlap;
}

/*
int checkRobotHitWalls(struct Robot * robot, struct Wall_collection * head) {
   struct Wall_collection *ptr = head;
   int hit = 0;

   while(ptr != NULL) {
      hit = (hit || checkRobotHitWall(robot, &ptr->wall));
      ptr = ptr->next;
   }
   return hit;

}*/

int checkRobotHitWalls(struct Robot * robot, SDL_Surface* surface, SDL_Renderer* renderer){
    double x1, y1, xDir, yDir;
    double x = robot->true_x, y = robot->true_y;
    x1 = x + matrixRotate(ROBOT_WIDTH/2, ROBOT_HEIGHT/2, robot->angle, 1);
    y1 = y + matrixRotate(ROBOT_WIDTH/2, ROBOT_HEIGHT/2, robot->angle, 0);
    //unit vector
    xDir = matrixRotate(-1, 0, robot->angle, 1);
    yDir = matrixRotate(-1, 0, robot->angle, 0);
    
    //SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < ROBOT_WIDTH; j++){
            //SDL_RenderDrawLine(renderer, round(x1), round(y1), round(x1 + xDir), round(y1 + yDir));
            if (isOpaque(surface, round(x1), round(y1))) return 1;
            x1 += xDir, y1 += yDir;
        }
        double temp = xDir;
        xDir = -yDir;
        yDir = temp;
    }
    
    return 0;
}



int checkRobotReachedEnd(struct Robot * robot, int x, int y, int width, int height){

    int overlap = checkOverlap(robot->x,robot->width,robot->y,robot->height,
                 x,width,y,height);

    return overlap;
}

void robotCrash(struct Robot * robot) {
    robot->currentSpeed = 0;
    if (!robot->crashed)
        printf("Ouchies!!!!!\n\nPress space to start again\n");
    robot->crashed = 1;
}

void robotSuccess(struct Robot * robot, int msec) {
    robot->currentSpeed = 0;
    if (!robot->crashed){
        printf("Success!!!!!\n\n");
        printf("Time taken %d seconds %d milliseconds \n", msec/1000, msec%1000);
        printf("Press space to start again\n");
    }
    robot->crashed = 1;
}

int checkRobotSensor(int x, int y, int sensorSensitivityLength, struct Wall * wall)  {
    //viewing_region of sensor is a square of 2 pixels * chosen length of sensitivity
    int overlap = checkOverlap(x,2,y,sensorSensitivityLength,
                 wall->x,wall->width,wall->y, wall->height);

    return overlap;
}

void checkRobotSensors(struct Robot * robot, SDL_Surface* surface, int* dest){
    double x = robot->true_x, y = robot->true_y;
    double x1 = x + matrixRotate(0, -(ROBOT_HEIGHT/2 - 1), robot->angle, 1);
    double y1 = y + matrixRotate(0, -(ROBOT_HEIGHT/2 - 1), robot->angle, 0);
    double x2 = x + matrixRotate(ROBOT_WIDTH/2, -(ROBOT_HEIGHT/2 - 1), robot->angle, 1);
    double y2 = y + matrixRotate(ROBOT_WIDTH/2, -(ROBOT_HEIGHT/2 - 1), robot->angle, 0);
    double x3 = x + matrixRotate(-(ROBOT_WIDTH/2 - 1), -(ROBOT_HEIGHT/2 - 1), robot->angle, 1);
    double y3 = y + matrixRotate(-(ROBOT_WIDTH/2 - 1), -(ROBOT_HEIGHT/2 - 1), robot->angle, 0);
    
    double xDir = matrixRotate(0, -floor(SENSOR_VISION/5), robot->angle, 1);
    double yDir = matrixRotate(0, -floor(SENSOR_VISION/5), robot->angle, 0);
    
    dest[0] = 5, dest[1] = 5, dest[2] = 5;
    
    int i;
    for (i = 0; i < 5; i++){
        //the front one
        
        if (isOpaque(surface, round(x1 + xDir), round(y1 + yDir))) break;
        dest[0]--;
        x1 += xDir, y1 += yDir;
    } if (i == 0) dest[0]--;
    for (i = 0; i < 5; i++){
        
        if (isOpaque(surface, round(x2 - yDir), round(y2 + xDir))) break;
        dest[1]--;
        x2 -= yDir, y2 += xDir;
    } if (i == 0) dest[1]--;
    for (i = 0; i < 5; i++){
        
        if (isOpaque(surface, round(x3 + yDir), round(y3 - xDir))) break;
        dest[2]--;
        x3 += yDir, y3 -= xDir;
    } if (i == 0) dest[2]--;
}

int checkRobotSensorFrontCentreAllWalls(struct Robot * robot, struct Wall_collection * head) {
    struct Wall_collection *ptr, *head_store;
    int i;
    double xDir, yDir;
    int robotCentreX, robotCentreY, xTL, yTL;
    int score, hit;

    int sensorSensitivityLength =  floor(SENSOR_VISION/5);

    head_store = head;
    robotCentreX = robot->x+ROBOT_WIDTH/2;
    robotCentreY = robot->y+ROBOT_HEIGHT/2;
    score = 0;

    for (i = 0; i < 5; i++)
    {
        ptr = head_store;
        //xDir = round(robotCentreX+(ROBOT_WIDTH/2-2)*cos((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*sin((robot->angle)*PI/180));
        //yDir = round(robotCentreY+(ROBOT_WIDTH/2-2)*sin((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*cos((robot->angle)*PI/180));
        xDir = round(robotCentreX-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*sin((robot->angle)*PI/180));
        yDir = round(robotCentreY+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*cos((robot->angle)*PI/180));
        xTL = (int) xDir;
        yTL = (int) yDir;
        hit = 0;

        while(ptr != NULL) {
            hit = (hit || checkRobotSensor(xTL, yTL, sensorSensitivityLength, &ptr->wall));
            ptr = ptr->next;
        }
        if (hit)
            score = i;
    }
    return score;
}

int checkRobotSensorLeftAllWalls(struct Robot * robot, struct Wall_collection * head) {
    struct Wall_collection *ptr, *head_store;
    int i;
    double xDir, yDir;
    int robotCentreX, robotCentreY, xTL, yTL;
    int score, hit;
    int sensorSensitivityLength;

    head_store = head;
    robotCentreX = robot->x+ROBOT_WIDTH/2;
    robotCentreY = robot->y+ROBOT_HEIGHT/2;
    score = 0;
    sensorSensitivityLength =  floor(SENSOR_VISION/5);

    for (i = 0; i < 5; i++)
    {
        ptr = head_store;
        //xDir = round(robotCentreX+(-ROBOT_WIDTH/2)*cos((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*sin((robot->angle)*PI/180));
        //yDir = round(robotCentreY+(-ROBOT_WIDTH/2)*sin((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*cos((robot->angle)*PI/180));
        xDir = round(robotCentreX+(+ROBOT_WIDTH/2)*cos((robot->angle-90)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*sin((robot->angle-90)*PI/180));
        yDir = round(robotCentreY+(+ROBOT_WIDTH/2)*sin((robot->angle-90)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*cos((robot->angle-90)*PI/180));
        xTL = (int) xDir;
        yTL = (int) yDir;
        hit = 0;

        while(ptr != NULL) {
            hit = (hit || checkRobotSensor(xTL, yTL, sensorSensitivityLength, &ptr->wall));
            ptr = ptr->next;
        }
        if (hit)
            score = i;
    }
    return score;
}

int checkRobotSensorRightAllWalls(struct Robot * robot, struct Wall_collection * head) {
    struct Wall_collection *ptr, *head_store;
    int i;
    double xDir, yDir;
    int robotCentreX, robotCentreY, xTL, yTL;
    int score, hit;
    int sensorSensitivityLength;

    head_store = head;
    robotCentreX = robot->x+ROBOT_WIDTH/2;
    robotCentreY = robot->y+ROBOT_HEIGHT/2;
    score = 0;
    sensorSensitivityLength =  floor(SENSOR_VISION/5);

    for (i = 0; i < 5; i++)
    {
        ptr = head_store;
        //xDir = round(robotCentreX+(-ROBOT_WIDTH/2)*cos((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*sin((robot->angle)*PI/180));
        //yDir = round(robotCentreY+(-ROBOT_WIDTH/2)*sin((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*cos((robot->angle)*PI/180));
        xDir = round(robotCentreX+(-ROBOT_WIDTH/2)*cos((robot->angle+90)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*sin((robot->angle+90)*PI/180));
        yDir = round(robotCentreY+(-ROBOT_WIDTH/2)*sin((robot->angle+90)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*cos((robot->angle+90)*PI/180));
        xTL = (int) xDir;
        yTL = (int) yDir;
        hit = 0;

        while(ptr != NULL) {
            hit = (hit || checkRobotSensor(xTL, yTL, sensorSensitivityLength, &ptr->wall));
            ptr = ptr->next;
        }
        if (hit)
            score = i;
    }
    return score;
}

void robotUpdate(struct SDL_Renderer * renderer, struct Robot * robot, SDL_Surface* surface){
    double x1, y1, x2, y2, x3, y3, xDir, yDir;
    double x = robot->true_x, y = robot->true_y;
    x1 = x + matrixRotate(ROBOT_WIDTH/2, ROBOT_HEIGHT/2, robot->angle, 1);
    y1 = y + matrixRotate(ROBOT_WIDTH/2, ROBOT_HEIGHT/2, robot->angle, 0);
    //unit vector
    xDir = matrixRotate(-20, 0, robot->angle, 1);
    yDir = matrixRotate(-20, 0, robot->angle, 0);
    
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    for (int i = 0; i < 4; i++){
        //printf("%d, %d, %d, %d\n", (int)x1, (int)y1, (int)(x1 + xDir), (int)(y1 + yDir));
        SDL_RenderDrawLine(renderer, round(x1), round(y1), round(x1 + xDir), round(y1 + yDir));
        x1 += xDir, y1 += yDir;
        double temp = xDir;
        xDir = -yDir;
        yDir = temp;
    }
    //draw the sensors
    
    double x10 = x + matrixRotate(0, -(ROBOT_HEIGHT/2 - 1), robot->angle, 1);
    double y10 = y + matrixRotate(0, -(ROBOT_HEIGHT/2 - 1), robot->angle, 0);
    double x20 = x + matrixRotate(ROBOT_WIDTH/2, -(ROBOT_HEIGHT/2 - 1), robot->angle, 1);
    double y20 = y + matrixRotate(ROBOT_WIDTH/2, -(ROBOT_HEIGHT/2 - 1), robot->angle, 0);
    double x30 = x + matrixRotate(-(ROBOT_WIDTH/2 - 1), -(ROBOT_HEIGHT/2 - 1), robot->angle, 1);
    double y30 = y + matrixRotate(-(ROBOT_WIDTH/2 - 1), -(ROBOT_HEIGHT/2 - 1), robot->angle, 0);
    x1 = x10, y1 = y10, x2 = x20, y2 = y20, x3 = x30, y3 = y30;
    
    xDir = matrixRotate(0, -floor(SENSOR_VISION/5), robot->angle, 1);
    yDir = matrixRotate(0, -floor(SENSOR_VISION/5), robot->angle, 0);
    
    for (int i = 0; i < 5; i++){
        SDL_SetRenderDrawColor(renderer, 80+(20*(i + 1)), 80+(20*(i + 1)), 80+(20*(i + 1)), 255);
        //the front one
        SDL_RenderDrawLine(renderer, round(x1), round(y1), round(x1 + xDir), round(y1 + yDir));
        //the right one
        SDL_RenderDrawLine(renderer, round(x2), round(y2), round(x2 - yDir), round(y2 + xDir));
        //the left one
        SDL_RenderDrawLine(renderer, round(x3), round(y3), round(x3 + yDir), round(y3 - xDir));
        x1 += xDir, y1 += yDir;
        x2 -= yDir, y2 += xDir;
        x3 += yDir, y3 -= xDir;
    }
    x1 = x10, y1 = y10, x2 = x20, y2 = y20, x3 = x30, y3 = y30;
    //draw angles of incidence
    xDir = matrixRotate(0, -1, robot->angle, 1);
    yDir = matrixRotate(0, -1, robot->angle, 0);
    //front one first
    double temp_angle, norm;
    temp_angle = fmod(robot->angle + 90, 360);
    for (int i = 0; i < SENSOR_VISION; i++){
        x1 += xDir, y1 += yDir;
        
        if (isOpaque(surface, round(x1), round(y1))){
            norm = normal(surface, x1, y1, temp_angle);
            drawAngleOfIncidence(renderer, x1, y1, temp_angle, norm, robot->display_tangent, robot->display_incidence);
            break;
        }
    }
    temp_angle = fmod(robot->angle + 180, 360);
    for (int i = 0; i < SENSOR_VISION; i++){
        x2 -= yDir, y2 += xDir;
        
        if (isOpaque(surface, round(x2), round(y2))){
            norm = normal(surface, x2, y2, temp_angle);
            drawAngleOfIncidence(renderer, x2, y2, temp_angle, norm, robot->display_tangent, robot->display_incidence);
            break;
        }
    }
    temp_angle = fmod(robot->angle + 0, 360);
    for (int i = 0; i < SENSOR_VISION; i++){
        x3 += yDir, y3 -= xDir;
        
        if (isOpaque(surface, round(x3), round(y3))){
            norm = normal(surface, x3, y3, temp_angle);
            drawAngleOfIncidence(renderer, x3, y3, temp_angle, norm, robot->display_tangent, robot->display_incidence);
            break;
        }
    }
}

void robotMotorMove(struct Robot * robot, int crashed) {
    if (crashed)
        robot->currentSpeed = 0, robot->currentSpeed2 = 0, robot->direction = 0, robot->direction2 = 0;
    else if (!robot->differential_mode){
        double x_offset, y_offset;
        
        
        switch(robot->direction){
            case UP :
                robot->currentSpeed += DEFAULT_SPEED_CHANGE;
                if (robot->currentSpeed > MAX_ROBOT_SPEED)
                    robot->currentSpeed = MAX_ROBOT_SPEED;
                break;
            case DOWN :
                robot->currentSpeed -= DEFAULT_SPEED_CHANGE;
                if (robot->currentSpeed < -MAX_ROBOT_SPEED)
                    robot->currentSpeed = -MAX_ROBOT_SPEED;
                break;
        }
        switch(robot->direction2){
            case LEFT :
                robot->angle = (robot->angle+360-DEFAULT_ANGLE_CHANGE)%360;
                break;
            case RIGHT :
                robot->angle = (robot->angle+DEFAULT_ANGLE_CHANGE)%360;
                break;
        }
        
        robot->direction = 0;
        robot->direction2 = 0;
        x_offset = (-robot->currentSpeed * sin(-robot->angle*PI/180));
        y_offset = (-robot->currentSpeed * cos(-robot->angle*PI/180));

        robot->true_x += x_offset;
        robot->true_y += y_offset;

        x_offset = round(robot->true_x);
        y_offset = round(robot->true_y);

        robot->x = (int) x_offset;
        robot->y = (int) y_offset;
    } else {
        switch(robot->direction){
            case UP :
                robot->currentSpeed += DEFAULT_SPEED_CHANGE;
                if (robot->currentSpeed > MAX_ROBOT_SPEED)
                    robot->currentSpeed = MAX_ROBOT_SPEED;
                break;
            case DOWN :
                robot->currentSpeed -= DEFAULT_SPEED_CHANGE;
                if (robot->currentSpeed < -MAX_ROBOT_SPEED)
                    robot->currentSpeed = -MAX_ROBOT_SPEED;
                break;
        }
        switch(robot->direction2){
            case UP :
                robot->currentSpeed2 += DEFAULT_SPEED_CHANGE;
                if (robot->currentSpeed2 > MAX_ROBOT_SPEED)
                    robot->currentSpeed2 = MAX_ROBOT_SPEED;
                break;
            case DOWN :
                robot->currentSpeed2 -= DEFAULT_SPEED_CHANGE;
                if (robot->currentSpeed2 < -MAX_ROBOT_SPEED)
                    robot->currentSpeed2 = -MAX_ROBOT_SPEED;
                break;
        }
        
        double radians_changed = (double)(robot->currentSpeed - robot->currentSpeed2) / ROBOT_WIDTH;
        double velocity = (double)(robot->currentSpeed + robot->currentSpeed2) / 2;
        double x_offset, y_offset;
        if (robot->currentSpeed == robot->currentSpeed2){
            //move straight
            radians_changed = 0;
            y_offset = robot->currentSpeed;
        } else {
            double R = ROBOT_WIDTH / 2 * (double)(robot->currentSpeed + robot->currentSpeed2)/(robot->currentSpeed - robot->currentSpeed2);
            //remember forwards is x
            y_offset = R * sin(radians_changed);
            x_offset = R - R * cos(radians_changed);
        }
        robot->true_x += matrixRotate(y_offset, x_offset, (robot->angle + 270) % 360, 1);
        robot->true_y += matrixRotate(y_offset, x_offset, (robot->angle + 270) % 360, 0);

        x_offset = round(robot->true_x);
        y_offset = round(robot->true_y);

        robot->direction = 0;
        robot->direction2 = 0;

        robot->x = (int) x_offset;
        robot->y = (int) y_offset;
        
        robot->angle = (robot->angle + (int)round(r2d(radians_changed))) % 360;
    }
}

int limit = 2;

int followingWall = 0;

int max_speed = 7;
double target_speed = 7;

int extra_turn = 0;

int mode = 0; //fsm time

int fallback_mode = 0;

int turnDir = 0;

int tightWall = 0;

int large_turns = 0;
int large_turn_cooldown = 0;

int f_sensor[5] = {0}, l_sensor[5] = {0}, r_sensor[5] = {0};
int sensor_idx = 0;

#define TURN_LEN (15)
int last_n_turns[TURN_LEN] = {0};
int turn_idx = 0;

int temp_count = 0;
int temp_result = 0;

int get_speed(int* src, int idx){
    if (idx < 0) idx += 5;
    idx = (idx + sensor_idx) % 5;
    return src[idx];
}

int get_turns(int val, int how_far_back){
    int idx = turn_idx;
    int result = 0;
    for (int i = 0; i < how_far_back; i++){
        idx = (idx + TURN_LEN - 1) % TURN_LEN;
        if (val == last_n_turns[idx]) result++;
    }
    return result;
}


void robotAutoMotorMove(struct Robot * robot, int f, int l, int r, int isActive) {
    if (!isActive) goto end;

    if (f && (mode == 2)){
        if ((get_speed(f_sensor, -1) != 0) || (robot->currentSpeed >= 6)){
            fallback_mode = mode;
            mode = 3;
            int lt = get_turns(LEFT, 6), rt = get_turns(RIGHT, 6);
            if (turnDir == LEFT) {if ((l > r) && (followingWall == 2) && (lt < 3) && (rt < 3)) turnDir = 0;}
            if (turnDir == RIGHT){if ((r > l) && (followingWall == 1) && (lt < 3) && (rt < 3)) turnDir = 0;}
            //printf("%d, %d, %d\n", lt, rt, turnDir);
        }
    }
    int finished = 0;
    while (!finished){
        switch (mode){
            case -3:
                //panning to the right value
                if (temp_count == temp_result + 1) {mode = 1; followingWall = 2; break;}
                robot->direction2 = RIGHT;
                temp_count++;
                finished = 1;
                break;
            case -2:
                //panning anticlockwise
                if (temp_count > temp_result) {mode = -3; temp_count = -temp_count; break;}
                if (temp_count < 0); //pan back first
                else {
                    if (r > 0) {
                        mode = 1;
                        followingWall = 1;
                        robot->direction2 = LEFT;
                        finished = 1;
                        break;
                    }
                }
                temp_count++;
                robot->direction2 = LEFT;
                if (robot->currentSpeed > 0) robot->direction = DOWN;
                finished = 1;
                break;
            case -1:
                //panning clockwise
                if (temp_count > 5) {temp_result = temp_count; temp_count = -temp_count; mode = -2; break;}
                if (l > 0) {temp_result = temp_count; temp_count = -temp_count; mode = -2; break;}
                robot->direction2 = RIGHT;
                if (robot->currentSpeed > 0) robot->direction = DOWN;
                temp_count++;
                finished = 1;
                break;
                
            case 0: //setup, sets followingWall
                if (followingWall) {mode = 1; break;}
                
                if (f || l || r) {
                    if (l || r){
                        followingWall = ((l > r) ? 2 : 1);
                        if (robot->currentSpeed > 0){
                            robot->direction2 = ((l > r) ? RIGHT : LEFT);
                            finished = 1; 
                        }
                        //break;
                    }
                    else {mode = -1; temp_count = 0; break;}
                }
                
                if (robot->currentSpeed < 5) robot->direction = UP;
                
                
                finished = 1;
                break;
            case 1:
                //determine if the wall is tight or not
                if ((l + r) > 5)
                    tightWall = 1;
                else tightWall = 0;
                mode = 2;
                temp_count = 0;
                break;
                
                
            case 2: //when wall following
                temp_count = (temp_count - 1 >= 0) ? temp_count - 1 : 0;
                
                large_turn_cooldown = (large_turn_cooldown - 1 >= 0) ? large_turn_cooldown - 1 : 0;
            
                if (followingWall == 1){
                    //if (l > r + 1) followingWall = 2;
                    if (l > 2) target_speed = fmax(5, target_speed - 1);
                    
                    if ((r > limit) && (l >= r)) {
                        tightWall = 1;
                        
                        
                    }
                    
                    int lt = get_turns(LEFT, 13), rt = get_turns(RIGHT, 13);
                        
                    if (((rt > 5) && (lt < 3)) && (l >= r)) followingWall = 2, large_turns = 0;
                    
                    if ((rt > 3) && (lt < 7) && (large_turn_cooldown == 0)){
                        large_turns++;

                        large_turn_cooldown = 20;
                        if (large_turns >= 2) large_turns = 0, followingWall = 2;
                    } 
                    
                    if (get_speed(r_sensor, -1) == 0);
                    else if ((get_speed(r_sensor, -2) == 0) && !tightWall);
                    else if (r <= 2 + tightWall) {
                        if (tightWall) turnDir = 0;
                        
                        if (get_turns(RIGHT, 4) < (tightWall ? 2 : 3))
                            robot->direction2 = RIGHT;

                        target_speed = fmax(4, target_speed - (tightWall ? 1 : 0.2));
                        r = get_speed(r_sensor, -1);
                    }
                    else if (r > 3) robot->direction2 = LEFT;// target_speed = fmax(1, target_speed - 1);
                    
                    if ((r == 3) && (get_speed(r_sensor, -1) == 3))
                        turnDir = 0;
                } else if (followingWall == 2) {
                    if (r > 2) target_speed = fmax(5, target_speed - 1);
                    
                    if ((l > limit) && (r >= l)) {
                        tightWall = 1; 
                        
                        
                    }
                    
                    int lt = get_turns(LEFT, 13), rt = get_turns(RIGHT, 13);
                        
                    if (((lt > 5) && (rt < 3)) && (r >= l)) followingWall = 1, large_turns = 0;
                    
                    if ((lt > 3) && (rt < 7) && (large_turn_cooldown == 0)){
                        large_turns++;

                        large_turn_cooldown = 20;
                        if (large_turns >= 2) large_turns = 0, followingWall = 1;
                    } 
                    
                    
                    if (get_speed(l_sensor, -1) == 0);
                    else if ((get_speed(l_sensor, -2) == 0) && !tightWall);
                    else if (l <= 2 + tightWall) {
                        if (tightWall) turnDir = 0;
                        
                        //prevent overturning
                        if (get_turns(LEFT, 4) < (tightWall ? 2 : 3))
                            robot->direction2 = LEFT;
                            
                        target_speed = fmax(4, target_speed - 1);
                        l = get_speed(l_sensor, -1);
                    }
                    else if (l > 3) robot->direction2 = RIGHT;// target_speed = fmax(1, target_speed - 1);
                    
                    if ((l == 3) && (get_speed(l_sensor, -1) == 3))
                        turnDir = 0;
                }
                
                if (robot->currentSpeed < target_speed) robot->direction = UP;
                else if (robot->currentSpeed > target_speed) robot->direction = DOWN;
                target_speed = fmin(max_speed, target_speed + 0.3);
                
                finished = 1;
                break;
            case 3: //detected wall in front
                temp_count = (temp_count - 1 >= 0) ? temp_count - 1 : 0;
            
                if (f == 0) {mode = 4; break;}
                target_speed = fmax(0, target_speed - (double)f / (tightWall ? 2.0 : 4.0));
                
                //if ((tightWall) && (target_speed < 2) && (temp_count == 0)) {mode = 6; break;}
                
                if (robot->currentSpeed > target_speed)
                    robot->direction = DOWN;
                if (turnDir == 0){
                    if (followingWall == 1){
                        /*
                        if (r == 0)
                            turnDir = RIGHT;
                        else turnDir = LEFT;
                        */
                        if (!r && !l && !get_speed(r_sensor, -1) && !get_speed(l_sensor, -1))
                            turnDir = LEFT;
                        else if (r == 0)
                            turnDir = RIGHT;
                        else turnDir = LEFT;
                    }
                    else if (followingWall == 2){
                        /*
                        if (l == 0)
                            turnDir = LEFT;
                        else turnDir = RIGHT;
                        */
                        if (!r && !l && !get_speed(r_sensor, -1) && !get_speed(l_sensor, -1))
                            turnDir = RIGHT;
                        else if (l == 0)
                            turnDir = LEFT;
                        else turnDir = RIGHT;
                    }
                }
                
                robot->direction2 = turnDir;
                finished = 1;
                break;
            case 4:
                if ((l < 3) && (r < 3) && (get_speed(l_sensor, 0) == 0) && (get_speed(r_sensor, 0))) {
                    robot->direction = UP;
                    mode = fallback_mode;
                    finished = 1;
                    target_speed = fmin(max_speed, target_speed + 0.1);
                    turnDir = 0;
                    break;
                }
                
                robot->direction2 = turnDir;
                //if (r >= 3) robot->direction2 = LEFT;
                //else if (l >= 3) robot->direction2 = RIGHT;
                finished = 1;
                mode = 5;
                
                break;
            case 5: //basically the same as 4
                
                
                //if (r >= 3) robot->direction2 = LEFT;
                //else if (l >= 3) robot->direction2 = RIGHT;
                if ((l < 3) && (r < 3) && (get_speed(l_sensor, 0) == 0) && (get_speed(r_sensor, 0)))
                    robot->direction = UP;
                else robot->direction2 = turnDir;
                mode = 1;
                finished = 1;
                target_speed = fmin(max_speed - tightWall, target_speed + 0.1);
                
                turnDir = 0;
                break;
            case 6:
                if (f == 0) {mode = 2; break;}
                if (temp_count > 5) {mode = 7; break;}
                if (robot->currentSpeed > 0) robot->direction = DOWN;
                if (followingWall == 1)
                    robot->direction2 = RIGHT;
                else if (followingWall == 2)
                    robot->direction2 = LEFT;
                finished = 1;
                temp_count++;
                break;
            case 7:
                if (f == 0) {mode = 2; break;}
                if (temp_count > 18) {mode = 2; break;}
                if (robot->currentSpeed > 0) robot->direction = DOWN;
                if (followingWall == 1)
                    robot->direction2 = LEFT;
                else if (followingWall == 2)
                    robot->direction2 = RIGHT;
                finished = 1;
                temp_count++;
                break;
        }
    }
    printf("front: %d, left: %d, right: %d\n", f, l, r);
    printf("mode: %d, following: %d, speed: %d, target: %.02f, turn: %d, turnlock: %d, tight: %d\n", mode, followingWall, robot->currentSpeed, target_speed, robot->direction2, turnDir, tightWall);
    finished = 0;
    
    end:
    f_sensor[sensor_idx] = f, l_sensor[sensor_idx] = l, r_sensor[sensor_idx] = r;
    sensor_idx = (sensor_idx + 1) % 5;
    last_n_turns[turn_idx] = robot->direction2;
    turn_idx = (turn_idx + 1) % TURN_LEN;
    
}

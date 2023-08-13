#include "stdio.h"
#include "stdlib.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL2_gfxPrimitives.h"

#include "time.h"

#include "formulas.h"
#include "wall.h"
#include "robot.h"

#define SIDEBAR_WIDTH (150)



int done = 0;


int main(int argc, char *argv[]) {
    SDL_Window *window;
    SDL_Renderer *renderer;
    
    SDL_Texture* wall_background;
    SDL_Surface* wall_surface = NULL;

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        return 1;
    }
    //the main window
    window = SDL_CreateWindow("Robot Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, OVERALL_WINDOW_WIDTH + SIDEBAR_WIDTH, OVERALL_WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    //the main renderer for the window
    renderer = SDL_CreateRenderer(window, -1, 0);

    int load_from_image = 0;

    if (argc == 2){
        load_from_image = 1;
        wall_surface = SDL_LoadBMP(argv[1]);
        if (!wall_surface){
            printf("Failed to load image %s: %s\n", argv[1], SDL_GetError());
            return 1;
        }

    } else {
        Uint32 rmask, gmask, bmask, amask;

    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
    #else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
    #endif
        
        wall_surface = SDL_CreateRGBSurface(0, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT, 32,
                                   rmask, gmask, bmask, amask);
    }

    struct Robot robot;
    struct Wall_collection *head = NULL;
    int front_centre_sensor, left_sensor, right_sensor=0;
    clock_t start_time, end_time;
    int msec;
    int crashed = 0;

    // SETUP MAZE
    // You can create your own maze here. line of code is adding a wall.
    // You describe position of top left corner of wall (x, y), then width and height going down/to right
    // Relative positions are used (OVERALL_WINDOW_WIDTH and OVERALL_WINDOW_HEIGHT)
    // But you can use absolute positions. 10 is used as the width, but you can change this.
    
    if (!load_from_image){
    
        //original
        ///*
        insertAndSetFirstWall(&head, 1,  OVERALL_WINDOW_WIDTH/2, OVERALL_WINDOW_HEIGHT/2, 10, OVERALL_WINDOW_HEIGHT/2);
        insertAndSetFirstWall(&head, 2,  OVERALL_WINDOW_WIDTH/2-100, OVERALL_WINDOW_HEIGHT/2+100, 10, OVERALL_WINDOW_HEIGHT/2-100);
        insertAndSetFirstWall(&head, 3,  OVERALL_WINDOW_WIDTH/2-250, OVERALL_WINDOW_HEIGHT/2+100, 150, 10);
        insertAndSetFirstWall(&head, 4,  OVERALL_WINDOW_WIDTH/2-150, OVERALL_WINDOW_HEIGHT/2, 150, 10);
        insertAndSetFirstWall(&head, 5,  OVERALL_WINDOW_WIDTH/2-250, OVERALL_WINDOW_HEIGHT/2-200, 10, 300);
        insertAndSetFirstWall(&head, 6,  OVERALL_WINDOW_WIDTH/2-150, OVERALL_WINDOW_HEIGHT/2-100, 10, 100);
        insertAndSetFirstWall(&head, 7,  OVERALL_WINDOW_WIDTH/2-250, OVERALL_WINDOW_HEIGHT/2-200, 450, 10);
        insertAndSetFirstWall(&head, 8,  OVERALL_WINDOW_WIDTH/2-150, OVERALL_WINDOW_HEIGHT/2-100, 250, 10);
        insertAndSetFirstWall(&head, 9,  OVERALL_WINDOW_WIDTH/2+200, OVERALL_WINDOW_HEIGHT/2-200, 10, 300);
        insertAndSetFirstWall(&head, 10,  OVERALL_WINDOW_WIDTH/2+100, OVERALL_WINDOW_HEIGHT/2-100, 10, 300);
        insertAndSetFirstWall(&head, 11,  OVERALL_WINDOW_WIDTH/2+100, OVERALL_WINDOW_HEIGHT/2+200, OVERALL_WINDOW_WIDTH/2-100, 10);
        insertAndSetFirstWall(&head, 12,  OVERALL_WINDOW_WIDTH/2+200, OVERALL_WINDOW_HEIGHT/2+100, OVERALL_WINDOW_WIDTH/2-100, 10);
        //*/
        
        //steep turn and narrow
        /*
        float aa, bb;
int name_index = 0;
int a = 325;
aa = 0.5;
bb = 1;
int c = 180;
int d = 1;
int e = a+100;
int f = c;
c += 50;
int m = 200;
for (int i = 0; i < m; i++){
    if (i < 150) {
        insertAndSetFirstWall(&head, name_index++,  a - i*aa , c + i*bb, 10, 10);
    }
    insertAndSetFirstWall(&head, name_index++,  e - i*aa , f + i*bb, 10, 10);
}
insertAndSetFirstWall(&head, name_index++, c-m*bb, a-150, 1000, 10);
insertAndSetFirstWall(&head, name_index++, c-m*bb, a-100, 305, 10);
insertAndSetFirstWall(&head, name_index++,  a - (m-50)*aa , c + (m-50)*bb, 10, 200);
insertAndSetFirstWall(&head, name_index++,  e - m*aa , f + m*bb, 10, 200);
        */
        
        //curved wall
        /*
        int i, a, b, c, d, e, f, g, h, k, l, m;
double j;
a = 325;
b = 40;
c = 10;
d = 3;
e = 90;
f = 225;
g = b;
h = c;
k = d;
l = e;
m = 500;
int name_index = 0;
for (i = 0; i < m; i++){
    j = i;
    insertAndSetFirstWall(&head, name_index++,
                            a + b*sin(c*j * M_PI/180),
                            (i * d)+e,
                            10, 10);
    insertAndSetFirstWall(&head, name_index++,
                            f + g*sin(h*j * M_PI/180),
                            (i * k)+l,
                            10, 10);
}
        */
        
        //long
        /*
        insertAndSetFirstWall(&head, 1,  OVERALL_WINDOW_WIDTH/1.50-100, 275, 10, OVERALL_WINDOW_HEIGHT);
insertAndSetFirstWall(&head, 2,  OVERALL_WINDOW_WIDTH/1.25-100, 350, 10, OVERALL_WINDOW_HEIGHT);
insertAndSetFirstWall(&head, 4,  OVERALL_WINDOW_WIDTH/2.25-100, 200, 10, OVERALL_WINDOW_HEIGHT);
insertAndSetFirstWall(&head, 5, 415, 350, 150, 10);
insertAndSetFirstWall(&head, 6, 260, 275, 220, 10);
insertAndSetFirstWall(&head, 7, 560, 60, 10, 300);
insertAndSetFirstWall(&head, 8, 50, 60, 10, 300);
insertAndSetFirstWall(&head, 9, 560, 60, 10, 300);
insertAndSetFirstWall(&head, 10, 475, 135, 10, 150);
insertAndSetFirstWall(&head, 11, 50, 60, 520, 10);
insertAndSetFirstWall(&head, 11, 125, 135, 350, 10);
insertAndSetFirstWall(&head, 10, 125, 135, 10, 150);
insertAndSetFirstWall(&head, 5, 60, 350, 125, 10);
insertAndSetFirstWall(&head, 5, 185, 200, 200, 10);
    
        */
    
        //complex
        /*
        int i, a, b, c, d, e, f, g, h, k, l, m,aa,bb;
double j;
a = 40;
aa = 0.5;
bb = 1;
b = 40;
c = 6;
d = 3;
e = 90;
f = 140;
g = b;
h = c;
k = d;
l = e;
m = 500;
int name_index = 0;
for (i = 0; i < m; i++){
    j = i;
    insertAndSetFirstWall(&head, name_index++,
                            a + b*sin(c*j * M_PI/180),
                            (i * d)+e,
                            10, 10);
    insertAndSetFirstWall(&head, name_index++,
                            f + g*sin(h*j * M_PI/180),
                            (i * k)+l,
                            10, 10);
}
insertAndSetFirstWall(&head, 1,  50, 15, 10, 85);
insertAndSetFirstWall(&head, 2,  50, 15, 200, 10);
insertAndSetFirstWall(&head, 2,  140, 85, 60, 10);
insertAndSetFirstWall(&head, 1,  200, 85, 10, 380);
insertAndSetFirstWall(&head, 1,  250, 15, 10, 245);
for (int i = 0; i < m; i++){
    if (i < 100) {
        insertAndSetFirstWall(&head, name_index++,  325 - i*0.75 , 350 - i*1, 8, 10);
        insertAndSetFirstWall(&head, name_index++,  325 - i*0.75 , 450 - i*1, 8, 10);
        insertAndSetFirstWall(&head, name_index++,  425 - i*0.75 , 350 - i*1, 8, 10);
    }
}
for (int i = 0; i < m; i++){
    if (i < 275) {
        insertAndSetFirstWall(&head, name_index++,  625 - i*0.75 , 450 - i*1, 8, 10);
    }
}
insertAndSetFirstWall(&head, 1,  250, 355, 10, 110);
insertAndSetFirstWall(&head, 2,  200, 465, 60, 10);
insertAndSetFirstWall(&head, 2,  325, 455, 300, 10);
insertAndSetFirstWall(&head, 2,  325, 355, 100, 10);
insertAndSetFirstWall(&head, 2,  275, 250, 80, 10);
insertAndSetFirstWall(&head, 2,  350, 175, 77, 10);
insertAndSetFirstWall(&head, 1,  270, 15, 10, 245);
insertAndSetFirstWall(&head, 1,  350, 105, 10, 80);
insertAndSetFirstWall(&head, 2,  270, 15, 400, 10);
insertAndSetFirstWall(&head, 2,  350, 100, 400, 10);
        */
        
        //basic 3
        /*
        insertAndSetFirstWall(&head, 2,  640-10-220, 400, 10, 80);
        insertAndSetFirstWall(&head, 2,  640-200-20, 400, 200, 10);
        insertAndSetFirstWall(&head, 2,  640-10-20, 50, 10, 350);
        insertAndSetFirstWall(&head, 2,  640-280-20, 50, 280, 10);
        insertAndSetFirstWall(&head, 2,  640-10-300, 50, 10, 100);
        insertAndSetFirstWall(&head, 2,  640-110-300, 150, 110, 10);
        insertAndSetFirstWall(&head, 2,  640-10-400, 50, 10, 100);
        insertAndSetFirstWall(&head, 2,  640-400-220, 50, 220, 10);
        insertAndSetFirstWall(&head, 2,  640-10-620, 50, 10, 290);
        insertAndSetFirstWall(&head, 2,  640-620-20, 340, 20, 10);


        insertAndSetFirstWall(&head, 1,  640-10-320, 300, 10, 180);
        insertAndSetFirstWall(&head, 2,  640-200-120, 300, 200, 10);
        insertAndSetFirstWall(&head, 2,  640-10-120, 150, 10, 150);
        insertAndSetFirstWall(&head, 2,  640-80-120, 150, 80, 10);
        insertAndSetFirstWall(&head, 2,  640-10-200, 150, 10, 100);
        insertAndSetFirstWall(&head, 2,  640-310-200, 250, 310, 10);
        insertAndSetFirstWall(&head, 2,  640-10-500, 150, 10, 100);
        insertAndSetFirstWall(&head, 2,  640-20-500, 150, 20, 10);
        insertAndSetFirstWall(&head, 2,  640-10-520, 150, 10, 290);
        insertAndSetFirstWall(&head, 2,  640-120-520, 440, 120, 10);
        */
        
        //complex 7
        /*
        int i;
    insertAndSetFirstWall(&head, 12,  640-10-120, 450, 10, 30);
    insertAndSetFirstWall(&head, 12,  640-10-220, 450, 10, 30);

    for (i = 0; i < 100; i++){
        insertAndSetFirstWall(&head, i,  640-10-(20 + i) , 350 + i, 10, 10); //1
        insertAndSetFirstWall(&head, i,  640-10-(20 +100 + i) , 350 + i, 10, 10); //1
    }
    insertAndSetFirstWall(&head, 12,  640-10-20, 280, 10, 70);
    insertAndSetFirstWall(&head, 12,  640-10-120, 280, 10, 70);

    for (i = 0; i < 180; i++){
        insertAndSetFirstWall(&head, i,  640-10-(20 +190 - i/2) , 100 + i, 10, 10); //1
    }
    for (i = 0; i < 105; i++){
        insertAndSetFirstWall(&head, i,  640-10-(20 +105/2 - i/2) , 175 + i, 10, 10); //1
    }


    insertAndSetFirstWall(&head, 2,  640-105/2-20, 175, 105/2, 10);
    insertAndSetFirstWall(&head, 2,  640-10-20, 20, 10, 155);
    insertAndSetFirstWall(&head, 2,  640-300-20, 20, 300, 10);
    insertAndSetFirstWall(&head, 2,  640-10-320, 20, 10, 60);
    insertAndSetFirstWall(&head, 2,  640-130-80, 100, 130, 10);
    insertAndSetFirstWall(&head, 2,  640-10-80, 80, 10, 20);
    insertAndSetFirstWall(&head, 2,  640-160-80, 80, 160, 10);


    double j;
    for (i = 0; i < 50; i++){
        j = i;
        insertAndSetFirstWall(&head, i+1,
                              // the most important bit is below.
                              // increase the 20 for a tighter bend
                              // descrease for a more meandering flow
                              640-10-(320 + 30*sin(10*j * M_PI/180)),
                              // increase the 5 for a spacier curve
                              (i * 5)+80,
                              10, 10);
    }
    for (i = 0; i < 75; i++){
        j = i;
        insertAndSetFirstWall(&head, i+1,
                              // the most important bit is below.
                              // increase the 20 for a tighter bend
                              // descrease for a more meandering flow
                              640-10-(240 + 30*sin(10*j * M_PI/180)),
                              // increase the 5 for a spacier curve
                              (i * 5)+80,
                              10, 10);
    }

    insertAndSetFirstWall(&head, 2,  640-105-345, 330, 105, 10);
    insertAndSetFirstWall(&head, 2,  640-10-450, 190, 10, 150);
    insertAndSetFirstWall(&head, 2,  640-70-380, 190, 70, 10);
    insertAndSetFirstWall(&head, 2,  640-10-380, 20, 10, 170);
    insertAndSetFirstWall(&head, 2,  640-260-380, 20, 260, 10);

    insertAndSetFirstWall(&head, 2,  640-345-255, 455, 345, 10);
    insertAndSetFirstWall(&head, 2,  640-10-600, 100, 10, 365);
    insertAndSetFirstWall(&head, 2,  640-70-530, 100, 70, 10);
    insertAndSetFirstWall(&head, 2,  640-10-530, 80, 10, 20);
    insertAndSetFirstWall(&head, 2,  640-110-530, 80, 110, 10);
        */
        
        drawWallsOntoSurface(wall_surface, head);
        
    }
    
    //apparently a texture blits faster so okay
    wall_background = SDL_CreateTextureFromSurface(renderer, wall_surface);
    
    SDL_Texture* past_background = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT);
    SDL_SetRenderTarget(renderer, past_background);
    SDL_SetTextureBlendMode(past_background, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderFillRect(renderer, NULL);
    SDL_SetRenderTarget(renderer, NULL);
    SDL_Rect footstep = {0, 0, 3, 3};
    
    
    SDL_Color rgba;
    Uint32 data = getpixel(wall_surface, OVERALL_WINDOW_WIDTH / 2 + 2, OVERALL_WINDOW_HEIGHT / 2 + 10);
    SDL_GetRGBA(data, wall_surface->format, &rgba.r, &rgba.g, &rgba.b, &rgba.a);
    printf("%d, %d, %d, %d\n", rgba.r, rgba.g, rgba.b, rgba.a);


    setup_robot(&robot);
    //updateAllWalls(head, renderer);
    SDL_Rect left_screen = {.x = 0, .y = 0, .w = OVERALL_WINDOW_WIDTH, .h = OVERALL_WINDOW_HEIGHT};
    SDL_Rect right_screen = {.x = OVERALL_WINDOW_WIDTH, .y = 0, .w = SIDEBAR_WIDTH, .h = OVERALL_WINDOW_HEIGHT};
    
    SDL_RenderCopy(renderer, wall_background, NULL, &left_screen);

    SDL_Event event;
    while(!done){
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderClear(renderer);

        //draw footsteps
        SDL_SetRenderTarget(renderer, past_background);
        footstep.x = robot.x - 1;
        footstep.y = robot.y - 1;
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 201, 130, 146, 150);
        SDL_RenderFillRect(renderer, &footstep);
        SDL_SetRenderTarget(renderer, NULL);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        
        if (robot.auto_mode) robot.differential_mode = 0;
        //Move robot based on user input commands/auto commands
        robotAutoMotorMove(&robot, front_centre_sensor, left_sensor, right_sensor, robot.auto_mode);
        if (!crashed)
            robotMotorMove(&robot, crashed);

        //Check if robot reaches endpoint. and check sensor values
        //if (checkRobotReachedEnd(&robot, 0, 340, 50, 100)){
        if (checkRobotReachedEnd(&robot, 0, 20, 50, 60)){
            end_time = clock();
            msec = (end_time-start_time) * 1000 / CLOCKS_PER_SEC;
            robotSuccess(&robot, msec);
        }
        /*
        else if(crashed == 1 || checkRobotHitWalls(&robot, head)){
            robotCrash(&robot);
            crashed = 1;
        }*/
        else if(crashed == 1 || checkRobotHitWalls(&robot, wall_surface, renderer)){
            robotCrash(&robot);
            crashed = 1;
        }
        //Otherwise compute sensor information
        else {
            int info[3] = {0};
            checkRobotSensors(&robot, wall_surface, info);
            front_centre_sensor = info[0];
            right_sensor = info[1];
            left_sensor = info[2];
            //front_centre_sensor = checkRobotSensorFrontCentreAllWalls(&robot, head);
            if (front_centre_sensor>0)
                //printf("Getting close on the centre. Score = %d\n", front_centre_sensor);

            //left_sensor = checkRobotSensorLeftAllWalls(&robot, head);
            if (left_sensor>0);
                //printf("Getting close on the left. Score = %d\n", left_sensor);

            //right_sensor = checkRobotSensorRightAllWalls(&robot, head);
            if (right_sensor>0);
                //printf("Getting close on the right. Score = %d\n", right_sensor);
            //printf("front: %d, left: %d, right: %d\n", front_centre_sensor, left_sensor, right_sensor);
        }
        SDL_RenderCopy(renderer, wall_background, NULL, &left_screen);
        
        SDL_RenderCopy(renderer, past_background, NULL, &left_screen);
        
        robotUpdate(renderer, &robot, wall_surface);
        //updateAllWalls(head, renderer);

        //sidebar
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &right_screen);
        //I'll just do it with images, too much work otherwise
        int w = OVERALL_WINDOW_WIDTH;
        //gradient time
        //255, 245, 68, 255
        //102, 211, 255, 255
        //the first bar
        /*
        int xOffset = w + 30, yOffset = 50, width = 10;
        for (int i = 0; i < 200; i++){
            SDL_SetRenderDrawColor(renderer, 255 + (double)(102 - 255)/200, 245 + (double)(211 - 245)/200, 68 + (double)(255 - 68)/200, 255);
            SDL_RenderDrawLine(renderer, xOffset, yOffset + i, xOffset + width, yOffset + i);
        }*/
        //green, then yellow, then red
        if (robot.differential_mode){
            SDL_Rect speed1 = {w + 45, 50, 10, 200};
            SDL_Rect speed2 = {w + 95, 50, 10, 200};
            SDL_SetRenderDrawColor(renderer, 184, 29, 19, 100);
            SDL_RenderFillRect(renderer, &speed1);
            SDL_RenderFillRect(renderer, &speed2);
            
            SDL_Rect yellow1 = {w + 45, 50 + 20, 10, 160};
            SDL_Rect yellow2 = {w + 95, 50 + 20, 10, 160};
            SDL_SetRenderDrawColor(renderer, 239, 183, 0, 100);
            SDL_RenderFillRect(renderer, &yellow1);
            SDL_RenderFillRect(renderer, &yellow2);
            
            SDL_Rect green1 = {w + 45, 50 + 60, 10, 80};
            SDL_Rect green2 = {w + 95, 50 + 60, 10, 80};
            SDL_SetRenderDrawColor(renderer, 0, 132, 80, 100);
            SDL_RenderFillRect(renderer, &green1);
            SDL_RenderFillRect(renderer, &green2);
            
            
            SDL_SetRenderDrawColor(renderer, 255, 245, 68, 255);
            SDL_RenderDrawRect(renderer, &speed1);
            SDL_RenderDrawRect(renderer, &speed2);
            SDL_Rect s1 = {w + 40, 50 + 98 - robot.currentSpeed * 4, 20, 5};
            SDL_Rect s2 = {w + 90, 50 + 98 - robot.currentSpeed2 * 4, 20, 5};
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &s1);
            SDL_RenderFillRect(renderer, &s2);
        } else {
            w += 25;
            SDL_Rect speed1 = {w + 45, 50, 10, 200};
            SDL_SetRenderDrawColor(renderer, 184, 29, 19, 100);
            SDL_RenderFillRect(renderer, &speed1);
            
            SDL_Rect yellow1 = {w + 45, 50 + 20, 10, 160};
            SDL_SetRenderDrawColor(renderer, 239, 183, 0, 100);
            SDL_RenderFillRect(renderer, &yellow1);
            
            SDL_Rect green1 = {w + 45, 50 + 60, 10, 80};
            SDL_SetRenderDrawColor(renderer, 0, 132, 80, 100);
            SDL_RenderFillRect(renderer, &green1);
            
            
            SDL_SetRenderDrawColor(renderer, 255, 245, 68, 255);
            SDL_RenderDrawRect(renderer, &speed1);
            SDL_Rect s1 = {w + 40, 50 + 98 - robot.currentSpeed * 4, 20, 5};
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &s1);
            w -= 25;
        }
        
        SDL_Rect compass = {w + 50, 300, 50, 50};
        SDL_RenderFillRect(renderer, &compass);
        //display the angles
        SDL_SetRenderDrawColor(renderer, 178, 12, 24, 255);
        double temp_angle = d2r((robot.angle + 90) % 360);
        double xDir = cos(temp_angle), yDir = sin(temp_angle);
        double xS = w + 50 + 25, yS = 300 + 25;
        for (int i = 0; i < 80; i++){
            int tempX = xDir * (5 - (double)i/16), tempY = yDir * (5 - (double)i/16);
            SDL_RenderDrawLine(renderer, round(xS + tempY), round(yS - tempX), round(xS - tempY), round(yS + tempX));
            xS += xDir/2, yS += yDir/2;
        }
        SDL_SetRenderDrawColor(renderer, 46, 64, 45, 255);
        temp_angle = d2r((robot.angle + 270) % 360);
        xDir = cos(temp_angle), yDir = sin(temp_angle);
        xS = w + 50 + 25, yS = 300 + 25;
        for (int i = 0; i < 80; i++){
            int tempX = xDir * (5 - (double)i/16), tempY = yDir * (5 - (double)i/16);
            SDL_RenderDrawLine(renderer, round(xS + tempY), round(yS - tempX), round(xS - tempY), round(yS + tempX));
            xS += xDir/2, yS += yDir/2;
        }

        // Check for user input
        SDL_RenderPresent(renderer);
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                done = 1;
            }
            const Uint8 *state = SDL_GetKeyboardState(NULL);
            if (!robot.differential_mode){
                if(state[SDL_SCANCODE_UP] && robot.direction != DOWN){
                    robot.direction = UP;
                }
                if(state[SDL_SCANCODE_DOWN] && robot.direction != UP){
                    robot.direction = DOWN;
                }
                if(state[SDL_SCANCODE_LEFT] && robot.direction2 != RIGHT){
                    robot.direction2 = LEFT;
                }
                if(state[SDL_SCANCODE_RIGHT] && robot.direction2 != LEFT){
                    robot.direction2 = RIGHT;
                }
                if (state[SDL_SCANCODE_C])
                    robot.differential_mode = 1, robot.auto_mode = 0, robot.currentSpeed2 = robot.currentSpeed;
            } else {
                if(state[SDL_SCANCODE_Q] && robot.direction != DOWN)
                    robot.direction = UP;
                if(state[SDL_SCANCODE_A] && robot.direction != UP)
                    robot.direction = DOWN;
                if(state[SDL_SCANCODE_E] && robot.direction2 != DOWN)
                    robot.direction2 = UP;
                if(state[SDL_SCANCODE_D] && robot.direction2 != UP)
                    robot.direction2 = DOWN;
                if (state[SDL_SCANCODE_V])
                    robot.differential_mode = 0;
            }
            if(state[SDL_SCANCODE_SPACE]){
                    setup_robot(&robot);
                    SDL_SetRenderTarget(renderer, past_background);
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                    SDL_RenderClear(renderer);
                    SDL_SetRenderTarget(renderer, NULL);
                    crashed = 0;
            }
            if(state[SDL_SCANCODE_RETURN]){
                robot.auto_mode = 1;
                start_time = clock();
            }
        }

        //SDL_Delay(10);
        SDL_Delay(120);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("DEAD\n");
}

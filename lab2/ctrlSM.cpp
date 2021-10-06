#define S_FUNCTION_NAME ctrlSM
#include "ttkernel.cpp"
#include "mex.h"

#include <time.h>
#include <random>
#include <cmath>

#define RAND_MIN -1000
#define RAND_MAX  1000

#define CTRL_TASK_PERIOD 60

#define PI 3.141592

// System Outputs
// Data structure used for the task data
#define CONS_ALPHA
#define CONS_THETA
#define CONS_VITESSE


// System Inputs
#define BATTERY_LEVEL 1
#define POS_X 2
#define POS_Y 3
#define POS_Z 4
#define ALPHA 5
#define THETA 6
#define VITESSE 7
#define DISTANCE 8

// Data structure used for the task data

struct TaskData {
	double battery_lvl;
    
    double curr_x;
    double curr_y;
    double curr_z;
    
    double dest_x; 
    double dest_y;
    
    double curr_alpha; 
    
    double curr_theta;
    double curr_vit;
    double distance;
    
    double d_alpha;
    double d_theta;
    double d_vit;
};

struct Mission {
    double x;
    double y;
}

Mission* get_new_mission(){
    Mission *m= new Mission();
    std::uniform_real_distribution<double> unif(RAND_MIN,RAND_MAX);
    std::default_random_engine re;
    m->x = unif(re);
    m->y = unif(re);
    return m
}

double navig_function(int segment, void* data) {
    TaskData *d = static_cast<TaskData*>(data);
    
    switch (segment) {
        case 1:
            // read inputs from operative system
            d->battery_lvl = ttAnalogIn(BATTERY_LEVEL);
            d->curr_x = ttAnalogIn(POS_X);
            d->curr_y= ttAnalogIn(POS_Y);
            d->curr_z= ttAnalogIn(POS_Z);
            d->curr_alpha= ttAnalogIn(ALPHA);
            d->curr_theta= ttAnalogIn(THETA);
            d->curr_vit= ttAnalogIn(VITESSE);
            d->distance= ttAnalogIn(DISTANCE);
            return 5;
        case 2:
            if(d->d_alpha != 0) // condition i was thinking of adding a boolean value to see whether or not we need to start a new mission
            {
                mexPrintf("Setting Alpha\n");
                d->curr_alpha += d->d_alpha;
                d->d_alpha = 0;
            }
            else if(true) // some condition
            {
                mexPrintf("Setting d_alpha\n");
                d->d_alpha = atan((dest_y- curr_y) / (dest_x- curr_x)); // if we want to make it deg => *180/PI
                if (dest_x- curr_x< 0) // gotta double check this one
                    d->d_alpha= - d->d_alpha;
            }
            return 1;
        default:
            // send ouputs to operative system
            //ttAnalogOut(VALVE_CTRL, d->open_valve);
            
            // We gotta AnalogOut the dAlpha
            
            return FINISHED;
    }
}


// Kernel init function    
void init(){
    // Allocate memory for the task and store pointer in UserData
    TaskData *data = new TaskData;
    ttSetUserData(data);
    memset( data, 0, sizeof(TaskData) );

    ttInitKernel(prioFP);

    mexPrintf("Simulation started\n");

    ttCreatePeriodicTask("nav", 0.0, CTRL_TASK_PERIOD,
                        navig_function, data);
    ttSetPriority(1, "nav");
}

// Kernel cleanup function
void cleanup() {
    // Free the allocated memory
    TaskData *d = static_cast<TaskData*>(ttGetUserData());
    delete d;

    mexPrintf("Simulation ended\n");
}

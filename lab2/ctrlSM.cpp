#define S_FUNCTION_NAME ctrlSM
#include "ttkernel.cpp"
#include "mex.h"

#include <time.h>
//#include <random>

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
    double curr_y
    double curr_z;
    double curr_alpha;
    double curr_theta;
    double curr_vit;
    double distance;
    double d_alpha;
    double d_theta;
    double d_vit; 
};

double navig_function(int segment, void* data) {
    TaskData *d = static_cast<TaskData*>(data);
    
    switch (segment) {
        case 1:
            // read inputs from operative system
            d->battery_lvl = ttAnalogIn(BATTERY_LEVEL);
            d->curr_x = ttAnalogIn(POS_X);
            return 5;
        case 2:
            if(d->tank_lvl <= LOW_LVL_THRESHOLD && d->open_valve)
            {
                mexPrintf("Closing Valve\n");
                d->open_valve = 0.0;
            }
            else if(d->tank_lvl >= HIGH_LVL_THRESHOLD && !d->open_valve)
            {
                mexPrintf("Opening Valve\n");
                d->open_valve = 1.0;
            }
            return 1;
        default:
            // send ouputs to operative system
            ttAnalogOut(VALVE_CTRL, d->open_valve);
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

    ttCreatePeriodicTask("task_ctrl", 0.0, CTRL_TASK_PERIOD,
                        ctrl_function, data);
    ttSetPriority(1, "task_ctrl");
}

// Kernel cleanup function
void cleanup() {
    // Free the allocated memory
    TaskData *d = static_cast<TaskData*>(ttGetUserData());
    delete d;

    mexPrintf("Simulation ended\n");
}

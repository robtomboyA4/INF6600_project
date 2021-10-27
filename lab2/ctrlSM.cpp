#define S_FUNCTION_NAME ctrlSM
#include "ttkernel.cpp"
#include "mex.h"

#include <time.h>
#include <cmath>
//#include <random>

#define CTRL_TASK_PERIOD 0.1
#define VIT_OPE 5

#define P_HIGH 0
#define P_LOW -1000
#define LAMBDA 1000

#define PI 3.141592

//System Outputs
#define CONS_ALPHA 1
#define CONS_THETA 2
#define CONS_VITESSE 3

//System Inputs
#define BATTERY_LEVEL 1
#define OBS_FAITES 2
#define POS_X 3
#define POS_Y 4
#define POS_Z 5
#define ALPHA 6
#define THETA 7
#define VITESSE 8
#define DISTANCE 9

//System Triggers
#define TRIGGER_ALARM_BAT 1
#define TRIGGER_DATA 2

// Data structure used for the task data

struct TaskData {
    double battery_lvl;
    double obs_faites;
    double curr_x;
    double curr_y;
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
                d->battery_lvl = ttAnalogIn(BATTERY_LEVEL);
                d->obs_faites = ttAnalogIn(OBS_FAITES);
                d->curr_x = ttAnalogIn(POS_X);
                d->curr_y= ttAnalogIn(POS_Y);
                d->curr_z= ttAnalogIn(POS_Z);
                d->curr_alpha= ttAnalogIn(ALPHA);
                d->curr_theta= ttAnalogIn(THETA);
                d->curr_vit= ttAnalogIn(VITESSE);
                d->distance= ttAnalogIn(DISTANCE);
                return 0.000009;
            case 2:
                if(d->curr_x != LAMBDA || d->curr_z != (P_LOW-P_HIGH))
                {
                    mexPrintf("Correct Alpha\n");
                    double alpha = atan2((P_LOW-P_HIGH - d->curr_z),LAMBDA - d->curr_x)*180/PI;
                    d->d_alpha = (alpha - d->curr_alpha);
                    //d->d_alpha = alpha;
                    d->d_theta = 0;
                    if(d->curr_vit != 5) {
                        d->d_vit = 5 - d->curr_vit;
                    }else {
                       d->d_vit = 0;
                    }
                } else
                {
                    d->d_alpha = 0;
                    d->d_theta = 0;
                    d->d_vit = -5;
                }
                return 0.000008;
            default:
                // send ouputs to operative system
                ttAnalogOut(CONS_ALPHA, d->d_alpha);
                ttAnalogOut(CONS_THETA, d->d_theta);
                ttAnalogOut(CONS_VITESSE, d->d_vit);
                
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
    
    ttCreatePeriodicTask("nav", 0.0, CTRL_TASK_PERIOD, navig_function, data);
    ttSetPriority(1, "nav");
    
}

// Kernel cleanup function
void cleanup() {
    // Free the allocated memory
    TaskData *d = static_cast<TaskData*>(ttGetUserData());
    delete d;
    
    mexPrintf("Simulation ended\n");
}

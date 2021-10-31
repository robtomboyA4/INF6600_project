#define S_FUNCTION_NAME ctrlSM
#include "ttkernel.cpp"
#include "mex.h"

#include <time.h>
#include <cmath>
//#include <random>

#define CTRL_TASK_PERIOD 0.1
#define OBS_TASK_PERIOD 5

#define VIT_OPE 5
#define MEMORY 128000

#define P_HIGH -10
#define P_LOW -1000
#define LAMBDA 1000

#define PI 3.141592

//System Outputs
#define CONS_ALPHA 1
#define CONS_THETA 2
#define CONS_VITESSE 3
#define TAKE_OBS 4
#define SEND_OBS 5

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
    int mode;
    int take_obs;
    int obs_faites;
    int send_obs;
    double battery_lvl;
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
    double x_dest;
    double y_dest;
    double z_dest;
};


/* NAVIGATION CONTROLLER*/
double navig_function(int segment, void* data) {
    TaskData *d = static_cast<TaskData*>(data);
    switch (segment) {
        case 1:
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
            if(d->curr_z == 0){
                ttCreateJob("mission");
                if (d->mode == 1){
                    double alpha = atan2((d->z_dest - d->curr_z), d->x_dest - d->curr_x)*180/PI;
                    double theta = atan2((d->y_dest - d->curr_y), d->x_dest - d->curr_x)*180/PI;
                    d->d_alpha = (alpha - d->curr_alpha);
                    d->d_theta = (theta - d->curr_theta);
                    if(d->curr_vit != VIT_OPE) {
                        d->d_vit = VIT_OPE - d->curr_vit;
                    }else {
                        d->d_vit = 0;
                    }
                    
                }else{
                    d->d_alpha = (0 - d->curr_alpha);
                    d->d_theta = 0;
                    d->d_vit = 0 - d->curr_vit;
                }
            }else if(d->z_dest == P_LOW && d->curr_z > d->z_dest && d->distance > 10) {
                double alpha = atan2((d->z_dest - d->curr_z), d->x_dest - d->curr_x)*180/PI;
                double theta = atan2((d->y_dest - d->curr_y), d->x_dest - d->curr_x)*180/PI;
                d->d_alpha = (alpha - d->curr_alpha);
                d->d_theta = (theta - d->curr_theta);
                if(d->curr_vit != VIT_OPE) {
                    d->d_vit = VIT_OPE - d->curr_vit;
                }else {
                    d->d_vit = 0;
                }
            }else if(d->z_dest == P_HIGH && d->curr_z <= d->z_dest){
                double alpha = atan2((d->z_dest - d->curr_z),d->x_dest - d->curr_x)*180/PI;
                double theta = atan2((d->y_dest - d->curr_y), d->x_dest - d->curr_x)*180/PI;
                d->d_alpha = (alpha - d->curr_alpha);
                d->d_theta = (theta - d->curr_theta);
                if(d->curr_vit != 5) {
                    d->d_vit = 5 - d->curr_vit;
                }else {
                    d->d_vit = 0;
                }
            }else if (d->z_dest == 0 && d->curr_z != d->z_dest){
                double alpha = 35;
                d->d_alpha = (alpha - d->curr_alpha);
                d->d_theta = 0;
                if(d->curr_vit != 5) {
                    d->d_vit = 5 - d->curr_vit;
                }else {
                    d->d_vit = 0;
                }
            }else {
                ttCreateJob("mission");
            }   
            return 0.000009;
        default:
            // send ouputs to operative system
            ttAnalogOut(CONS_ALPHA, d->d_alpha);
            ttAnalogOut(CONS_THETA, d->d_theta);
            ttAnalogOut(CONS_VITESSE, d->d_vit);
            return FINISHED;
    }
}

double mission_function(int segment, void* data) {
    TaskData *d = static_cast<TaskData*>(data);
    d->battery_lvl = ttAnalogIn(BATTERY_LEVEL);
    d->curr_x = ttAnalogIn(POS_X);
    d->curr_y= ttAnalogIn(POS_Y);
    d->curr_z= ttAnalogIn(POS_Z);
    switch(segment) {
        case 1:
            if (d->curr_z == 0) {
                if(d->battery_lvl > 5.0){
                    mexPrintf("Start diving to P_LOW\n");
                    d->mode = 1;
                    d->x_dest = d->curr_x + LAMBDA/2;
                    d->y_dest = d->curr_y + LAMBDA/2;
                    d->z_dest = P_LOW;
                }else{
                    mexPrintf("Call mothership\n");
                    d->mode = 0;
                    d->x_dest = d->curr_x;
                    d->y_dest = d->curr_y;
                    d->z_dest = d->curr_z;
                }
            } else if (d->curr_z != 0 && d->z_dest == P_HIGH) {
                if(d->battery_lvl > 5.0) {
                    mexPrintf("Diving\n");
                    d->mode = 1;
                    d->x_dest += LAMBDA/2;
                    d->y_dest += LAMBDA/2;
                    d->z_dest = P_LOW;
                }else{
                    mexPrintf("Go to the top\n");
                    d->mode = 0;
                    d->z_dest = 0;
                }
            } else if (d->curr_z != 0 && d->z_dest == P_LOW) {
                mexPrintf("Rising\n");
                d->mode = 1;
                d->x_dest += LAMBDA/2;
                d->y_dest -= LAMBDA/2;
                d->z_dest = P_HIGH;
            }
            return 0.000008;
        default:
            return FINISHED;
    }
}

double obs_function(int segment, void* data) {
    TaskData *d = static_cast<TaskData*>(data);
    switch(segment){
        case 1:
            d->obs_faites = ttAnalogIn(OBS_FAITES);
            return 0.000001;
        case 2:
            if (d->mode == 1){
                d->take_obs = 1;
            }else{
                d->take_obs = 0;
            }
            if(d->obs_faites == 1){
                mexPrintf("OBSERVATION DONE");
            }
            return 0.000002;
        default:
            ttAnalogOut(TAKE_OBS, d->take_obs);
            return FINISHED;
    }
}

// Kernel init function    
void init(){
    // Allocate memory for the task and store pointer in UserData
    TaskData *data = new TaskData;
    ttSetUserData(data);
    memset( data, 0, sizeof(TaskData) );
    double dest[10];
    dest[0] = LAMBDA/2;
    for(int i = 1; i<10; i++){
        dest[i] = dest[i-1] +LAMBDA/2;
    }
    double t = 0;
    ttInitKernel(prioFP);
    
    mexPrintf("Simulation started\n");
    ttCreateTask("mission", 0.00001, mission_function, data);
    ttSetPriority(1, "mission");
    
    //ttCreateTask("antenna", 0.00001, antenna_comm, data);
    //ttSetPriority(3);
    
    ttCreatePeriodicTask("nav", 0.0, CTRL_TASK_PERIOD, navig_function, data);
    ttSetPriority(2, "nav");
    
    
    ttCreatePeriodicTask("obs", 1.0, 5.0, obs_function, data);
    ttSetPriority(4, "obs");
}

// Kernel cleanup function
void cleanup() {
    // Free the allocated memory
    TaskData *d = static_cast<TaskData*>(ttGetUserData());
    delete d;
    
    mexPrintf("Simulation ended\n");
}

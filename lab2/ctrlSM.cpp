#define S_FUNCTION_NAME ctrlSM
#include "ttkernel.cpp"
#include "mex.h"

#include <time.h>
#include <cmath>

#define CTRL_TASK_PERIOD 0.1
#define OBS_TASK_PERIOD 5
#define VIT_OPE 5
#define ALPHA_MODE_0 35

#define P_HIGH -100
#define P_LOW -1000

#define LAMBDA 300
#define PI 3.141592
#define MEMORY 128000
#define OBS_SIZE 20

//System Outputs
#define CONS_ALPHA 1
#define CONS_THETA 2
#define CONS_VITESSE 3
#define TAKE_OBS 4
#define SEND_OBS 5
#define CHARGE_BAT 6

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

#define NUM_MISSIONS 2
#define BOUND_MIN -1000
#define BOUND_MAX 1000

#define NUM_OBS_PER_CYCLE sqrt( pow(LAMBDA, 2 ) + pow(2 * (P_HIGH - P_LOW), 2 )) / VIT_OPE / OBS_TASK_PERIOD

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
    
    //current mission id
    int mission_id;
    
    //current mission destination
    double x_dest; 
    double y_dest;
    
    // pour check si on a pas depasse la pos voulue
    double last_x; 
    double last_y;
    // next dest
    double next_z;
    
    double r;
    
    //
    bool dest_reached;
    
    int mode;
    
    int take_obs;
    int send_obs;
    int charge_bat;
    int data_sent;
    
    
    int memoire_restante;
};

struct Point{
    float x;
    float y;
};

Point missions[NUM_MISSIONS];

void generate_missions(){
    for(int i = 0; i< NUM_MISSIONS; i++)
    {
        missions[i].x= BOUND_MIN + (BOUND_MAX - BOUND_MIN) * ((float) rand() / RAND_MAX);
        missions[i].y= BOUND_MIN + (BOUND_MAX - BOUND_MIN) * ((float) rand() / RAND_MAX);
    }
} 

double obs_function(int segment, void* data) {
    TaskData *d = static_cast<TaskData*>(data);
    switch(segment){
        case 1:
            d->obs_faites = ttAnalogIn(OBS_FAITES);
            return 0.000001;
        case 2:
            if(d->obs_faites == 1)
                d->memoire_restante-= 1;
            else
                mexPrintf("Observation non effectuée\n");
            return 0.000001;
        default:
            ttAnalogOut(TAKE_OBS, d->take_obs);
            return FINISHED;
    }
}

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
                double r, next_r, next_alpha, next_theta, next_vit;
                if(d->mode == 0)
                {
                    if (d->curr_z == 0)
                    {
                        next_alpha= d->curr_alpha;
                        next_vit= 0;
                        if( d->curr_vit== 0 ) // check if the sousMarin stopped moving so we can call the mothership
                            ttCreateJob("mission");
                    }
                    else
                    {
                        next_alpha= ALPHA_MODE_0;
                        next_vit= VIT_OPE;
                    }
                    next_theta= d->curr_theta;
                }
                else{
                    // checking if the mission is over !
                    if ( (d->curr_x - d->x_dest)*(d->last_x - d->x_dest) <0 && 
                            (d->curr_y - d->y_dest)*(d->last_y - d->y_dest) <0 ){
                        mexPrintf("Mission finished!\n");
                        d->dest_reached= true;
                        ttCreateJob("mission");
                        
                        mexPrintf("d->curr_theta %f\n", d->curr_theta);
                        next_theta= d->curr_theta;
                    }
                    else
                    {
                        next_theta= atan2(d->y_dest - d->curr_y, d->x_dest - d->curr_x)*180/PI;
                        d->r -= sqrt( pow(d->last_x - d->curr_x, 2 ) + pow(d->last_y - d->curr_y, 2 ));
        //              if we reach the pos where we re supposed to change alpha
                        if((d->curr_z >= P_HIGH && d->next_z == P_HIGH) || ((d->curr_z <= P_LOW|| d->distance <= 10) && d->next_z == P_LOW )  )//    ) ||  (d->curr_x - d->next_x)*(d->last_x - d->next_x) <0
                        {
                            d->r = LAMBDA  / 2;
                            if (d->next_z == P_LOW) 
                            {
                                d->next_z= P_HIGH;
                            }
                            else // s’apprête à replonger
                            {
                                d->next_z= P_LOW;
                                ttCreateJob("mission");
                            }
                        }
                        next_alpha = d->r < 0 ? ( d->next_z== P_HIGH ? 90 : -90 ) : atan2(d->next_z - d->curr_z, d->r )*180/PI; // next_r 
                        next_vit= VIT_OPE;
                    }
                }
                //just before leaving
                d->last_x= d->curr_x;
                d->last_y= d->curr_y;
                d->d_theta = next_theta - d->curr_theta ;
                d->d_alpha = next_alpha - d->curr_alpha;
                d->d_vit = next_vit - d->curr_vit;
                return 0.000008;
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
    switch(segment) {
        case 1:
            d->battery_lvl = ttAnalogIn(BATTERY_LEVEL);
            d->curr_x = ttAnalogIn(POS_X);
            d->curr_y= ttAnalogIn(POS_Y);
            d->curr_z= ttAnalogIn(POS_Z);
            d->curr_vit= ttAnalogIn(VITESSE);
            return 0.000004;
        case 2:
            if ( d->curr_x == 0 && d->curr_y == 0 && d->curr_z == 0) // just started 
            {
                d->last_x= 0;
                d->last_y= 0;
                d->mission_id= 0;
                d->mode= 1;
                d->dest_reached= false;
                d->x_dest= missions[d->mission_id].x;
                d->y_dest= missions[d->mission_id].y;
                d->r= LAMBDA / 2;
                d->next_z= P_LOW;
                d->charge_bat = 0;
                d->data_sent = 0;
                d->memoire_restante= MEMORY/OBS_SIZE;
            }
            else if(d->dest_reached){ // new mission
                d->mission_id +=1 ;
                if (d->mission_id>=NUM_MISSIONS)
                {
                    d->mode = 0;
                    d->take_obs= 0;
                    d->send_obs= 1;
                }
                else 
                {
                    d->x_dest= missions[d->mission_id].x;
                    d->y_dest= missions[d->mission_id].y;
                    d->dest_reached= false;
                }
            }
            else if (d->charge_bat== 1 ) // if it s still charging
            {
                d->charge_bat=  d->battery_lvl >=100 ? 0 : 1 ;
            }
            else if (d->data_sent == 1 ) // if it sent its data
            {
                d->data_sent=  0 ;
                d->mode = 1;
                if (d->mission_id>=NUM_MISSIONS)
                    d->take_obs= 0; // stop taking obs 
                else
                    d->take_obs= 1; // start taking obs again
                d->send_obs= 0; // stop sending observations
            }
            else if (d->battery_lvl <= 5 || d->memoire_restante < NUM_OBS_PER_CYCLE ) // if it needs to be charged or memory is full
            {
                d->mode = 0;
                d->take_obs= 0;
                if(d->curr_vit == 0) // check if the SM stopped moving
                {
                    if(d->battery_lvl <= 5)
                        d->charge_bat= 1;
                    else
                        d->send_obs= 1;
                }
            }
            else //
            {
                d->mode = 1;
                d->take_obs= 1;
            }
            return 0.000001;
        default:
            ttAnalogOut(SEND_OBS, d->send_obs);
            ttAnalogOut(CHARGE_BAT, d->charge_bat);
            return FINISHED;
    }
}

double battery_full(int segment, void* data) {
    TaskData *d = static_cast<TaskData*>(data);
    switch (segment) {
        case 1:
            d->charge_bat = 0;
            ttCreateJob("mission");
            return 1;
        default:
            return FINISHED;
    }
}

double data_sent(int segment, void* data) {
    TaskData *d = static_cast<TaskData*>(data);
    switch (segment) {
        case 1:
            mexPrintf("trigger");
            d->data_sent = 1;
            ttCreateJob("mission");
            return 1;
        default:
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
    
    srand((unsigned) time(0));
    generate_missions();
    
    for(int i = 0; i< NUM_MISSIONS; i++)
    {
        mexPrintf("%f , %f\n", missions[i].x , missions[i].y);
    }
    
    mexPrintf("Simulation started\n");

    
    ttCreateTask("mission", 0.00001, mission_function, data);
    ttSetPriority(1, "mission");
    
    ttCreatePeriodicTask("nav", 0.0, CTRL_TASK_PERIOD, navig_function, data);
    ttSetPriority(2, "nav");
    
    ttCreatePeriodicTask("obs", 1.0, OBS_TASK_PERIOD, obs_function, data);
    ttSetPriority(3, "obs");
    
    // create a job to initialize vars
    ttCreateJob("mission");
    
    
    ttCreateHandler("battery_full", 1, battery_full, data);
    ttAttachTriggerHandler(TRIGGER_ALARM_BAT, "battery_full");
    
    ttCreateHandler("data_sent", 1, data_sent, data);
	ttAttachTriggerHandler(TRIGGER_DATA, "data_sent");
    
}

// Kernel cleanup function
void cleanup() {
    // Free the allocated memory
    TaskData *d = static_cast<TaskData*>(ttGetUserData());
    delete d;
    
    mexPrintf("Simulation ended\n");
}


// void print_data(TaskData &d)
// {
//     mexPrintf("next_theta %f, next_alpha %f\n", next_theta, next_alpha);
//     mexPrintf("x_dest %f, y_dest %f, , mission_id %d\n", d->x_dest , d->y_dest, d->mission_id );
//     mexPrintf("d->curr_x %f, d->curr_y %f, d->curr_z %f\n", d->curr_x, d->curr_y, d->curr_z);
//     mexPrintf("d->next_x %f, d->next_y %f, d->next_z %f\n",d->next_x, d->next_y, d->next_z);
//     mexPrintf("d->last_x %f, d->last_y %f\n", d->last_x,  d->last_y);
//     mexPrintf("d->distance %f\n", d->distance);
// }
                    

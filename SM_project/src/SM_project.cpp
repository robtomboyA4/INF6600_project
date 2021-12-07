#include <iostream>
#include <math.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/syspage.h>
#include <sys/neutrino.h>
#include "observationsModule.h"
#include "navigationModule.h"
#include "alimentationModule.h"

using namespace std;

#define LAMBDA 1000
#define P_HIGH -100
#define P_LOW -1000

#define ALPHA_MODE_0 35

MotionSystem sys;
AlimSystem alim;
PathMap pm;

typedef struct
{
	float x_dest;
	float y_dest;
	float z_dest;
	float r;
	float last_x;
	float last_y;
	float next_x;
	float next_y;
	float next_z;
	bool dest_reached;
}mission_datas;
mission_datas mission;

void *TobservationControl(void *arg)
{
	while(1)
	{
		if(sys.getMode() == 1)
		{
			pm.takeObservations({sys.getPos().x, sys.getPos().y, sys.getPos().z});
			alim.setDataTransmit(false);
		}else{
			if(sys.getPos().z >= 5 && alim.getDataTransmit() == false)
			{
				pm.transmitObservations();
				alim.setDataTransmit(true);
			}
		}
	}
}

/*
void *TantenneControl(void *arg)
{
	//TO DO
}*/

/*
void *Talimentation(void *arg)
{
	while(1)
	{
		alim.consommation(sys.getPos().vit, alim.getDataTransmit());
		printf("BATTERY = %f\n", alim.getBattery());
	}
	return(NULL);
}*/


void TmissionControl()
{
	if(sys.getPos().z == 0){
		if(alim.getBattery() > 5.0)
		{
			printf("Start diving to P_LOW\n");
			sys.setMode(1);
			mission.x_dest = LAMBDA/2;
			mission.y_dest = LAMBDA/2;
			mission.z_dest = P_LOW;
		}else{
			printf("Call mothership\n");
			sys.setMode(0);
			mission.x_dest = sys.getPos().x;
			mission.y_dest = sys.getPos().y;
			mission.z_dest = sys.getPos().z;
		}
	}else if(sys.getPos().z != 0 && mission.z_dest == P_HIGH){
		if(alim.getBattery() > 5.0){
			printf("Diving\n");
			sys.setMode(1);
			mission.x_dest += LAMBDA/2;
			mission.y_dest += LAMBDA/2;
			mission.z_dest = P_LOW;
		}else{
			printf("Go to the top\n");
			sys.setMode(0);
			mission.z_dest = 0;
		}
	}else if(sys.getPos().z != 0 && mission.z_dest == P_LOW){
		printf("Rising\n");
		sys.setMode(1);
		mission.x_dest += LAMBDA/2;
		mission.y_dest -= LAMBDA/2;
		mission.z_dest = P_HIGH;
	}
}

void *TnavigationControl(void *arg)
{
    float next_alpha, next_theta, next_vit, d_a, d_t, d_v;

    while(1)
    {
    	if(sys.getPos().z == 0){
    		TmissionControl();
    		if(sys.getMode() == 1){
    			next_alpha = atan2((mission.z_dest - sys.getPos().z), mission.x_dest - sys.getPos().x)*180/PI;
    			next_theta = atan2((mission.y_dest - sys.getPos().y), mission.x_dest - sys.getPos().x)*180/PI;
    			next_vit = VIT_OPE;
    			d_a = next_alpha - sys.getPos().alpha;
    			d_t = next_theta - sys.getPos().theta;
    			d_v = next_vit - sys.getPos().vit;
    		}else{
    			d_a = 0 - sys.getPos().alpha;
    			d_t = 0;
       			d_v = 0 - sys.getPos().vit;
    		}
    	}else if(mission.z_dest == P_LOW && sys.getPos().z > mission.z_dest && pm.getDistSeaFloor({sys.getPos().x, sys.getPos().y, sys.getPos().z})){
    		next_alpha = atan2((mission.z_dest - sys.getPos().z), mission.x_dest - sys.getPos().x)*180/PI;
    		next_theta = atan2((mission.y_dest - sys.getPos().y), mission.x_dest - sys.getPos().x)*180/PI;
    		next_vit = VIT_OPE;
    		d_a = next_alpha - sys.getPos().alpha;
     		d_t = next_theta - sys.getPos().theta;
     		d_v = next_vit - sys.getPos().vit;
    	}else if(mission.z_dest == P_HIGH && sys.getPos().z <= mission.z_dest){
    		next_alpha = atan2((mission.z_dest - sys.getPos().z), mission.x_dest - sys.getPos().x)*180/PI;
    		next_theta = atan2((mission.y_dest - sys.getPos().y), mission.x_dest - sys.getPos().x)*180/PI;
    		next_vit = VIT_OPE;
    		d_a = next_alpha - sys.getPos().alpha;
    		d_t = next_theta - sys.getPos().theta;
    		d_v = next_vit - sys.getPos().vit;
    	}else if(mission.z_dest == 0 && sys.getPos().z != mission.z_dest){
    		next_alpha = ALPHA_MODE_0;
    		next_vit = VIT_OPE;
    		d_a = next_alpha - sys.getPos().alpha;
    		d_t = 0;
    		d_v = next_vit - sys.getPos().vit;
    	}else{
    		TmissionControl();
    	}
    	sys.move(d_a, d_t, d_v);
    }
    sys.~MotionSystem();
	return (NULL);
}

void Q_init(void)
{
	pthread_t tid[2];
	pthread_attr_t attrib;
	struct sched_param mySchedParam;

	// Configuration des parametres des threads
	//setprio(0,20);
	pthread_attr_init(&attrib);
	pthread_attr_setinheritsched(&attrib, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attrib, SCHED_FIFO);

	// Creation des threads
	/*
	mySchedParam.sched_priority = 3;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if(pthread_create(&tid[0], &attrib, Talimentation, NULL) < 0)
		printf("taskSpawn Talimentation failed!\n");
	*/
	mySchedParam.sched_priority = 2;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if(pthread_create(&tid[1], &attrib, TnavigationControl, NULL) < 0)
		printf("taskSpawn TnaviationControl failed!\n");

	mySchedParam.sched_priority = 1;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if(pthread_create(&tid[2], &attrib, TobservationControl, NULL) < 0)
		printf("taskSpawn TobservationControl failed!\n");

	// attendre la fin de l'ensemble des threads
	for (int i = 0; i < 2; i++)
		pthread_join(tid[i], NULL);

	return;
}


int main()
{
	int numCpus;
	//obtenir le nombre de processeurs disponibles
	numCpus = _syspage_ptr->num_cpu;
	printf("Processeurs disponibles : %i\n\n", numCpus);



    PathMap pm;
    Q_init();
    //coord_t pos = {0,0,0};
    /*
    float x_dest = 100;
    float y_dest = 100;
    float z_dest = 500;
    printf("Move");
    while(sys.getPos().x < x_dest || sys.getPos().y < y_dest || sys.getPos().z < z_dest)
    {
    	float d_v;
    	float alpha = atan2((z_dest - sys.getPos().z), x_dest - sys.getPos().x)*180/PI;
   		float theta = atan2((y_dest - sys.getPos().y), x_dest - sys.getPos().x)*180/PI;
   		float d_a = alpha - sys.getPos().alpha;
   		float d_t = theta - sys.getPos().theta;
   		if(sys.getPos().vit != VIT_OPE)
   		{
   			d_v = VIT_OPE - sys.getPos().vit;
   		}else{
   			d_v = 0;
   		}
   		sys.move(d_a, d_t, d_v);
   		//printf("x = %f\n y = %f\n z = %f\n\n", sys.getPos().x, sys.getPos().y, sys.getPos().z);
    }*/

    /*
    for (int i = 0; i < 100; ++i)
    {
        pos.x = i*5;
        pos.y = i*5;
        pos.z = 100+i;
        pm.takeObservations(pos);
    }
    for (int i = 0; i < 100; ++i)
    {
        pos.x = 500 + i*5;
        pos.y = 500+ i*5;
        pos.z = 200-i;
        pm.takeObservations(pos);
    }

    for (int i = 0; i < 100; ++i)
    {
        pos.x = 1000 - i;
        pos.y = 1000 + i*5;
        pos.z = 100+ 2 * i;
        pm.takeObservations(pos);
    }
    for (int i = 0; i < 100; ++i)
    {
        pos.x = 900 - i;
        pos.y = 1500+ i*5;
        pos.z = 300-i;
        pm.takeObservations(pos);
    }

    pm.transmitObservations();
	*/
    return 0;
}

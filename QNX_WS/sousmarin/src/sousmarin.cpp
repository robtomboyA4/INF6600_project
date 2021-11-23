#include <iostream>
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

using namespace std;

// prototype fonction
int NbMsqQueue(mqd_t);

// Max de message dans un queue
#define MAX_NUM_MSG 50


typedef struct
{
	double x;
	double y;
	double z;
	double alpha;
	double theta;
	double vit;
}curr_navig;

typedef struct
{
	double x_dest;
	double y_dest;
}dest_navig;

/*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*/
/*
/*		Tache			: TnavigationControl()
/*
/*		Description		: Contrôle la tajectoire du Sous Marin
/*
/*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*/
void *TnavigationControl(void *arg)
{


	return(NULL);
}


/*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*/
/*
/*		Tache			: Q_init()
/*
/*		Description		: Cree et initialise les queues et les threads
/*
/*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*/
void Q_init(void)
{
	pthread_t tid[1];
	pthread_attr_t attrib;
	struct sched_param mySchedParam;
	struct mq_attr mqattr;
	int i;

	// Initialisation des semaphores


	// Creation des queues
	//memset(&mqattr, 0, sizeof(mqattr));
	//mqattr.mq_maxmsg = MAX_NUM_MSG;


	// Configuration des parametres des threads
	//setprio(0,20);
	pthread_attr_init(&attrib);
	pthread_attr_setinheritsched(&attrib, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attrib, SCHED_FIFO);

	// Creation des threads
	mySchedParam.sched_priority = 1;
	pthread_attr_setschedparam(&attrib, &mySchedParam);
	if(pthread_create(&tid[0], &attrib, TnavigationControl, NULL) < 0)
		printf("taskSpawn TnaviationControl failed!\n");


	// attendre la fin de l'ensemble des threads
	for (i = 0; i < 0; i++)
		pthread_join(tid[i], NULL);

	//Fermer les queues



	return;
}




int main(int argc, char const *argv[])
{
	int numCpus;
	//obtenir le nombre de processeurs disponibles
	numCpus = _syspage_ptr->num_cpu;

	printf("Processeurs disponibles : %i\n\n", numCpus);
	PathMap pm;
	coord_t pos = {0,0,0};
	curr_navig curr_pos = pos;
	// appel a la fonction Q_init pour creer les queues et le threads
	Q_init();
	return 1;

	/*
	PathMap pm;
	coord_t pos = {0,0,0};

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

	return 0;*/
}


/*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*/
/*
/*		Tache			: NbMsqQueue()
/*
/*		Description		: Retourne le nombre de messages présents dans
/* 							la queue passée en argument
/*
/*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*//*/
int NbMsqQueue(mqd_t QId)
{
	struct mq_attr mqstat ;
	mq_getattr( QId, &mqstat );
	return mqstat.mq_curmsgs ;
}

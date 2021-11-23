#include <iostream>
#include <cmath>
#include <random>
#include <cstring>

#include "navigationModule.h"

using namespace std;

MotionSystem::MotionSystem()
{
	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	pos.alpha = 0;
	pos.theta = 0;
	pos.vit = 0;
}

MotionSystem::~MotionSystem()
{
	delete pos;
}

void MotionSystem::move(consigne_navig cons)
{
	double alpha_err = (rand() % 201 - 100)/100;
	double theta_err = (rand() % 201 - 100)/100;
	double vit_err = (rand() % 201 - 100)/1000;

	pos.alpha = ((pos.alpha + cons.d_alpha) + alpha_err + 180) % 360 - 180;
	pos.theta = ((pos.alpha + cons.d_alpha) + alpha_err + 180) % 360 - 180;
	pos.vit = (pos.vit + cons.d_vit) + vit_err;

	pos.x = pos.vit * cos(pos.alpha*PI/180)*cos(pos.theta*PI/180);
	pos.y = pos.vit * cos(pos.alpha*PI/180)*sin(pos.theta*PI/180);
	pos.z = pos.vit * sin(pos.alpha*PI/180);
}

#include <iostream>
#include <math.h>
#include <random>
#include <random>
#include <cstring>

#include "navigationModule.h"

using namespace std;

float random_angle_err(int min, int max)
{
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<> distr(min, max);
	return distr(eng) - 3;
}

float random_vit_err(int min, int max)
{
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<> distr(min, max);
	return distr(eng) - 1;
}


MotionSystem::MotionSystem()
{
	mode = 0;
	pos.x = 0;
	pos.y = 0;
	pos.z = 0;
	pos.alpha = 0;
	pos.theta = 0;
	pos.vit = 0;
}

MotionSystem::~MotionSystem()
{
	delete &mode;
	delete &pos;
}

void MotionSystem::move(float d_a, float d_t, float d_v)
{

	float alpha_err = random_angle_err(ERR_MIN, ANGLE_ERR_MAX);
	float theta_err = random_angle_err(ERR_MIN, ANGLE_ERR_MAX);;
	float vit_err = random_vit_err(ERR_MIN, VIT_ERR_MAX);
	printf("prec_pos = %f	%f	%f\n", pos.x, pos.y, pos.z);
	pos.alpha = pos.alpha + d_a + alpha_err;
	pos.theta = pos.theta + d_t + theta_err;
	pos.vit = (pos.vit + d_v) + vit_err;

	pos.x = pos.x + pos.vit * cos(pos.alpha*PI/180)*cos(pos.theta*PI/180);
	pos.y = pos.y + pos.vit * cos(pos.alpha*PI/180)*sin(pos.theta*PI/180);
	pos.z = pos.z + pos.vit * sin(pos.alpha*PI/180);
	printf("after_pos = %f	%f	%f\n\n", pos.x, pos.y, pos.z);
}

void MotionSystem::setMode(bool m)
{
	mode = m;
}

bool MotionSystem::getMode()
{
	return mode;
}

curr_pos MotionSystem::getPos()
{
	return pos;
}

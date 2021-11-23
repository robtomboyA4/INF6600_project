#include <iostream>
#include <cmath>
#include <random>
#include <cstring>

#include "alimentationModule.h"

using namespace std;

AlimSystem::AlimSystem()
{
	bat_lvl = INIT_NIV_BAT;
}

AlimSystem::~AlimSystem()
{
	delete bat_lvl;
}

void AlimSystem::conso(double bat_lvl)
{
	double conso_moteur = pos.vit * 100/(BAT_AUTONOMY_S*VITESSE_MAX_MS);
	//double conso_transmit = transmit * 100/BAT_AUTONOMY_S;
	double conso_transmit = 0;
	bat_lvl -= conso_moteur - conso_transmit;
}

void AlimSystem::charge(double bat_lvl)
{
	bat_lvl += 100/(TIME_TO_CHARGE/SIMU_ACCEL);
}

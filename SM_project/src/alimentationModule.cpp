#include <iostream>
#include <cmath>
#include <random>
#include <cstring>

#include "alimentationModule.h"

using namespace std;

AlimSystem::AlimSystem()
{
	bat_lvl = INIT_NIV_BAT;
	data_transmit = false;
}

AlimSystem::~AlimSystem()
{
	delete &bat_lvl;
}

void AlimSystem::consommation(float vit_moteur, bool data_transmit)
{
	float conso_moteur = vit_moteur * 100/(BAT_AUTONOMY_S*VITESSE_MAX_MS);
	float conso_transmit = 0;
	if(data_transmit == 1){
		conso_transmit = 100/BAT_AUTONOMY_S;
	}
	bat_lvl -= conso_moteur - conso_transmit;
	if(bat_lvl < 0)
		bat_lvl = 0;
}

void AlimSystem::charge()
{
	bat_lvl += 100/(TIME_TO_CHARGE_S/SIMU_ACCEL);
	if(bat_lvl > 100)
		bat_lvl = 100;
}

float AlimSystem::getBattery()
{
	return bat_lvl;
}

void AlimSystem::setDataTransmit(bool a)
{
	data_transmit = a;
}

bool AlimSystem::getDataTransmit()
{
	return data_transmit;
}

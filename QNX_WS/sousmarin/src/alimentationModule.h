#ifndef ALIMENTATIONMODULE_H_
#define ALIMENTATIONMODULE_H_

#define SIMU_ACCEL 8000000
#define INIT_NIV_BAT 50
#define BAT_AUTONOMY_S 20*3600
#define VITESSE_MAX_MS 0.5
#define TIME_TO_CHARGE_S 50*60

class AlimSystem {
public:
	AlimSystem();
	~AlimSystem();

	void conso(double bat_lvl);
	void charge(double bat_lvl);

private:
	double bat_lvl;
};



#endif /* ALIMENTATIONMODULE_H_ */

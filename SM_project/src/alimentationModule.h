#ifndef ALIMENTATIONMODULE_H_
#define ALIMENTATIONMODULE_H_

#define SIMU_ACCEL 100
#define INIT_NIV_BAT 50
#define BAT_AUTONOMY_S 20*3600
#define VITESSE_MAX_MS 0.5
#define TIME_TO_CHARGE_S 50*60

class AlimSystem {
public:
	AlimSystem();
	~AlimSystem();

	void consommation(float vit_moteur, bool data_transmit);
	void charge();

	float getBattery();
	void setDataTransmit(bool a);
	bool getDataTransmit();

private:
	float bat_lvl;
	bool data_transmit;
};



#endif /* ALIMENTATIONMODULE_H_ */

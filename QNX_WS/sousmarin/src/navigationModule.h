#ifndef NAVIGATIONMODULE_H_
#define NAVIGATIONMODULE_H_

#define SIMU_ACCEL 8000000
#define PI 3.141592

typedef struct
{
	double d_alpha;
	double d_theta;
	double d_vit;
} consigne_navig;

typedef struct
{
	double x;
	double y;
	double z;
	double alpha;
	double theta;
	double vit;
}curr_navig;


class MotionSystem {
public:
	MotionSystem();
	~MotionSystem();

	void move(consigne_navig cons);

private:
	curr_navig pos;
};




#endif /* NAVIGATIONMODULE_H_ */

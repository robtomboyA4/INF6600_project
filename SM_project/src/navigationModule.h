#ifndef NAVIGATIONMODULE_H_
#define NAVIGATIONMODULE_H_

#define SIMU_ACCEL 100
#define PI 3.141592
#define VIT_OPE 5

constexpr int ERR_MIN = 0;
constexpr int ANGLE_ERR_MAX = 6;
constexpr int VIT_ERR_MAX = 2;

typedef struct
{
	float x;
	float y;
	float z;
	float alpha;
	float theta;
	float vit;
}curr_pos;


class MotionSystem {
public:
	MotionSystem();
	~MotionSystem();

	void move(float d_a, float d_t, float d_v);

	void setMode(bool m);
	bool getMode();

	curr_pos getPos();

private:
	bool mode;
	curr_pos pos;

};




#endif /* NAVIGATIONMODULE_H_ */

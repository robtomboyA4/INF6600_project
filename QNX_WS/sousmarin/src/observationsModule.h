#ifndef __GEN_MAP_H__
#define __GEN_MAP_H__

#define SIMU_ACCEL 8000000

#define IMG_W_PX 5000
#define IMG_H_PX 5000

#define PX_TO_M 1

#define IMG_W_M (PX_TO_M * IMG_W_PX)
#define IMG_H_M (PX_TO_M * IMG_H_PX)

#define BLANK_RGB {0,0,0}

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_t;

typedef struct
{
    double x;
    double y;
    double z;
} coord_t;


class PathMap {
    public:
        PathMap();
        ~PathMap();

        void takeObservations(coord_t position);
        uint32_t transmitObservations();
        uint32_t getDistSeaFloor(coord_t pos);

    private:
        rgb_t **img;

        uint32_t dumpId = 0;

        void setPx(int32_t coordX, int32_t coordY, int32_t coordZ, int32_t seaDepth);
		void drawDisc(int32_t coordX, int32_t coordY, int32_t coordZ);
		void dumpImage(const char* file_name);
};

#endif

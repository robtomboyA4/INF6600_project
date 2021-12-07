#include <iostream>
#include <cmath>
#include <cstring>

#include "observationsModule.h"
#include "navigationModule.h"

uint32_t mod(int32_t n, int32_t q)
{
    int r = n%q;
    if(r <0)
    {
        return r+q;
    }
    else 
    {
        return r;
    }
}

unsigned short lfsr = 0xBD43u;
unsigned bit;

unsigned myRand()
{
    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    return lfsr =  (lfsr >> 1) | (bit << 15);
}

PathMap::PathMap()
{
	this->img = new rgb_t*[IMG_H_PX];
	for(int i = 0; i < IMG_H_PX; ++i)
	{
		this->img[i] = new rgb_t[IMG_W_PX];
		for(int j = 0; j < IMG_W_PX; ++j)
		{
			this->img[i][j] = BLANK_RGB;
		}
	}
}

PathMap::~PathMap() 
{
    for(int i = 0; i < IMG_H_PX; ++i)
    {
        delete[] this->img[i];
    }
    delete[] this->img;
}

static const rgb_t getRGB(int32_t coordZ, int32_t seaDepth)
{
    uint32_t valZ, valSeaDepth;
    uint32_t val;
    if(coordZ < 0) valZ = (uint32_t) (-coordZ);
    else valZ = (uint32_t) (coordZ);
    if(seaDepth < 0) valSeaDepth = (uint32_t) (-seaDepth);
    else valSeaDepth = (uint32_t) (seaDepth);
    val = ((valSeaDepth & 0xFFF) << 12) | ((valZ & 0xFFF));
	return {(uint8_t) (val >> 16), (uint8_t) ((val >> 8) & 0xFF), (uint8_t) (val & 0xFF)};
}

static uint32_t getVal(rgb_t rgb)
{
    return (uint32_t) ((rgb.r << 16) | (rgb.g << 8) | (rgb.b));
}

static uint32_t getSeaDepth(rgb_t rgb)
{
    return (getVal(rgb) >> 12) & 0xFFF;
}

uint32_t PathMap::getDistSeaFloor(coord_t pos)
{
    int x,y;
    x = mod(pos.x,IMG_W_PX);
    y = mod(pos.y,IMG_H_PX);
    uint32_t seaDepth = getSeaDepth(this->img[x][y]);
    if (seaDepth == 0)
        return 1000;
    else
        return pos.z + getSeaDepth(this->img[x][y]);
}

void PathMap::setPx(int32_t coordX, int32_t coordY, int32_t coordZ, int32_t seaDepth)
{
	int x,y;
	x = mod(coordX,IMG_W_PX);
	y = mod(coordY,IMG_H_PX);

    // std::cout << "coordX= " << coordX << " X= " << x << " coordY= " << coordY << " Y= " << y << std::endl;

    this->img[x][y] = getRGB(coordZ, seaDepth);
}


void PathMap::drawDisc(int32_t coordX, int32_t coordY, int32_t coordZ)
{
    uint32_t seaDepth = 0;
    
    for (int i = -5; i <= 5 && seaDepth == 0; ++i)
    {
        for (int j = -5; j <= 5 && seaDepth == 0; ++j)
        {
            seaDepth = getSeaDepth(this->img[mod(coordX+i,IMG_W_PX)][mod(coordY+j,IMG_H_PX)]);
        }
    }
    if (seaDepth == 0)
        seaDepth = (1000 - myRand() % 200);

    if(seaDepth > 1000)
        seaDepth = 1000;
    else if (seaDepth < 800)
        seaDepth = 800;

    for(int i = -4; i <= 4; ++i)
    {
        for(int j = -4; j <= 4; ++j)
        {
            if(i==0 && j==0)
                setPx(coordX + i, coordY + j, coordZ, seaDepth + (rand() % 51) - 25);
            else
                setPx(coordX + i, coordY + j, 0xFFF, seaDepth + (rand() % 51) - 25);
        }
    }
}

void PathMap::dumpImage(const char* file_name)
{
    FILE *f;

    int w = IMG_W_PX;
    int h = IMG_H_PX;

    int x, y, r, g, b;

    unsigned char *img = NULL;
    int filesize = 54 + 3*w*h;  //w is your image width, h is image height, both int

    img = (unsigned char *)malloc(3*w*h);
    memset(img,0,3*w*h);

    for(int i=0; i<w; i++)
    {
        for(int j=0; j<h; j++)
        {
            x=i; y=(h-1)-j;
            rgb_t pixel = this->img[x][y];

            r = pixel.r;
            g = pixel.g;
            b = pixel.b;
            img[(x+y*w)*3+2] = (unsigned char)(r);
            img[(x+y*w)*3+1] = (unsigned char)(g);
            img[(x+y*w)*3+0] = (unsigned char)(b);
        }
    }

    unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
    unsigned char bmppad[3] = {0,0,0};

    bmpfileheader[ 2] = (unsigned char)(filesize    );
    bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
    bmpfileheader[ 4] = (unsigned char)(filesize>>16);
    bmpfileheader[ 5] = (unsigned char)(filesize>>24);

    bmpinfoheader[ 4] = (unsigned char)(       w    );
    bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
    bmpinfoheader[ 6] = (unsigned char)(       w>>16);
    bmpinfoheader[ 7] = (unsigned char)(       w>>24);
    bmpinfoheader[ 8] = (unsigned char)(       h    );
    bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
    bmpinfoheader[10] = (unsigned char)(       h>>16);
    bmpinfoheader[11] = (unsigned char)(       h>>24);

    f = fopen(file_name,"wb");
    fwrite(bmpfileheader,1,14,f);
    fwrite(bmpinfoheader,1,40,f);
    for(int i=0; i<h; i++)
    {
        fwrite(img+(w*(h-i-1)*3),3,w,f);
        fwrite(bmppad,1,(4-(w*3)%4)%4,f);
    }

    free(img);
    fclose(f);
}

void PathMap::takeObservations(coord_t position)
{
	int32_t x = mod((int32_t) position.x, IMG_W_PX);
	int32_t y = mod((int32_t) position.y, IMG_H_PX);

	for(int32_t loopId = 0; loopId < 8000000/SIMU_ACCEL; loopId++)
	{
		drawDisc(x,IMG_H_PX - y - 1, position.z);
	}
}

uint32_t PathMap::transmitObservations()
{
	char fileName[32];
	sprintf(fileName, "Transmission_%d.bmp", this->dumpId);

	this->dumpImage(fileName);

	this->dumpId++;

	for(uint64_t loopId = 0; loopId < 5000000000/SIMU_ACCEL; loopId++)
	{
		//transmitting
	}
	return this->dumpId;
}




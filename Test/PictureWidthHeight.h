#ifndef _PICTURE_WIDTH_HEIGHT
#define _PICTURE_WIDTH_HEIGHT
#include <ostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#define MAKEUS(a, b)	((unsigned short) ( ((unsigned short)(a))<<8 | ((unsigned short)(b)) ))
#define MAKEUI(a,b,c,d) ((unsigned int) ( ((unsigned int)(a)) << 24 | ((unsigned int)(b)) << 16 | ((unsigned int)(c)) << 8 | ((unsigned int)(d)) ))
#define M_DATA  0x00
#define M_SOF0  0xc0
#define M_DHT   0xc4
#define M_SOI	0xd8
#define M_EOI   0xd9
#define M_SOS   0xda
#define M_DQT	0xdb
#define M_DNL   0xdc
#define M_DRI   0xdd
#define M_APP0  0xe0
#define M_APPF	0xef
#define M_COM   0xfe
int GetPNGWidthHeight(const char* path, unsigned int* punWidth, unsigned int* punHeight);
int GetJPEGWidthHeight(const char* path, unsigned int *punWidth, unsigned int *punHeight);
int GetPicWidthHeight(const char* path, unsigned int *punWidth, unsigned int *punHeight);
int GetPNGWidthHeight(FILE *pfRead, unsigned int* punWidth, unsigned int* punHeight, unsigned int seekSet = 0);
#endif
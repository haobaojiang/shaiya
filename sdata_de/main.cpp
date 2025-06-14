/*
	ShaiyaSdataTool
	pushedx

	This program will allow you to encrypt and decrypt sdata files.

	Made using ShaiyaUS (version 113) client. The logic can be used for
	other versions or regions as long as the keys are updated to match
	the expected values.

	I am releasing my project as a "public domain" release. However, see the
	license for the SEED code that is used before using it in your own
	projects.
*/
#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include "SEED_KISA.h"
#include <windows.h>
#include <MyNetWork.h>


unsigned char ChecksumTable[] = 
{
	0x00, 0x00, 0x00, 0x00, 0x96, 0x30, 0x07, 0x77, 0x2C, 0x61, 0x0E, 0xEE, 0xBA, 0x51, 0x09, 0x99,
	0x19, 0xC4, 0x6D, 0x07, 0x8F, 0xF4, 0x6A, 0x70, 0x35, 0xA5, 0x63, 0xE9, 0xA3, 0x95, 0x64, 0x9E,
	0x32, 0x88, 0xDB, 0x0E, 0xA4, 0xB8, 0xDC, 0x79, 0x1E, 0xE9, 0xD5, 0xE0, 0x88, 0xD9, 0xD2, 0x97,
	0x2B, 0x4C, 0xB6, 0x09, 0xBD, 0x7C, 0xB1, 0x7E, 0x07, 0x2D, 0xB8, 0xE7, 0x91, 0x1D, 0xBF, 0x90,
	0x64, 0x10, 0xB7, 0x1D, 0xF2, 0x20, 0xB0, 0x6A, 0x48, 0x71, 0xB9, 0xF3, 0xDE, 0x41, 0xBE, 0x84,
	0x7D, 0xD4, 0xDA, 0x1A, 0xEB, 0xE4, 0xDD, 0x6D, 0x51, 0xB5, 0xD4, 0xF4, 0xC7, 0x85, 0xD3, 0x83,
	0x56, 0x98, 0x6C, 0x13, 0xC0, 0xA8, 0x6B, 0x64, 0x7A, 0xF9, 0x62, 0xFD, 0xEC, 0xC9, 0x65, 0x8A,
	0x4F, 0x5C, 0x01, 0x14, 0xD9, 0x6C, 0x06, 0x63, 0x63, 0x3D, 0x0F, 0xFA, 0xF5, 0x0D, 0x08, 0x8D,
	0xC8, 0x20, 0x6E, 0x3B, 0x5E, 0x10, 0x69, 0x4C, 0xE4, 0x41, 0x60, 0xD5, 0x72, 0x71, 0x67, 0xA2,
	0xD1, 0xE4, 0x03, 0x3C, 0x47, 0xD4, 0x04, 0x4B, 0xFD, 0x85, 0x0D, 0xD2, 0x6B, 0xB5, 0x0A, 0xA5,
	0xFA, 0xA8, 0xB5, 0x35, 0x6C, 0x98, 0xB2, 0x42, 0xD6, 0xC9, 0xBB, 0xDB, 0x40, 0xF9, 0xBC, 0xAC,
	0xE3, 0x6C, 0xD8, 0x32, 0x75, 0x5C, 0xDF, 0x45, 0xCF, 0x0D, 0xD6, 0xDC, 0x59, 0x3D, 0xD1, 0xAB,
	0xAC, 0x30, 0xD9, 0x26, 0x3A, 0x00, 0xDE, 0x51, 0x80, 0x51, 0xD7, 0xC8, 0x16, 0x61, 0xD0, 0xBF,
	0xB5, 0xF4, 0xB4, 0x21, 0x23, 0xC4, 0xB3, 0x56, 0x99, 0x95, 0xBA, 0xCF, 0x0F, 0xA5, 0xBD, 0xB8,
	0x9E, 0xB8, 0x02, 0x28, 0x08, 0x88, 0x05, 0x5F, 0xB2, 0xD9, 0x0C, 0xC6, 0x24, 0xE9, 0x0B, 0xB1,
	0x87, 0x7C, 0x6F, 0x2F, 0x11, 0x4C, 0x68, 0x58, 0xAB, 0x1D, 0x61, 0xC1, 0x3D, 0x2D, 0x66, 0xB6,
	0x90, 0x41, 0xDC, 0x76, 0x06, 0x71, 0xDB, 0x01, 0xBC, 0x20, 0xD2, 0x98, 0x2A, 0x10, 0xD5, 0xEF,
	0x89, 0x85, 0xB1, 0x71, 0x1F, 0xB5, 0xB6, 0x06, 0xA5, 0xE4, 0xBF, 0x9F, 0x33, 0xD4, 0xB8, 0xE8,
	0xA2, 0xC9, 0x07, 0x78, 0x34, 0xF9, 0x00, 0x0F, 0x8E, 0xA8, 0x09, 0x96, 0x18, 0x98, 0x0E, 0xE1,
	0xBB, 0x0D, 0x6A, 0x7F, 0x2D, 0x3D, 0x6D, 0x08, 0x97, 0x6C, 0x64, 0x91, 0x01, 0x5C, 0x63, 0xE6,
	0xF4, 0x51, 0x6B, 0x6B, 0x62, 0x61, 0x6C, 0x1C, 0xD8, 0x30, 0x65, 0x85, 0x4E, 0x00, 0x62, 0xF2,
	0xED, 0x95, 0x06, 0x6C, 0x7B, 0xA5, 0x01, 0x1B, 0xC1, 0xF4, 0x08, 0x82, 0x57, 0xC4, 0x0F, 0xF5,
	0xC6, 0xD9, 0xB0, 0x65, 0x50, 0xE9, 0xB7, 0x12, 0xEA, 0xB8, 0xBE, 0x8B, 0x7C, 0x88, 0xB9, 0xFC,
	0xDF, 0x1D, 0xDD, 0x62, 0x49, 0x2D, 0xDA, 0x15, 0xF3, 0x7C, 0xD3, 0x8C, 0x65, 0x4C, 0xD4, 0xFB,
	0x58, 0x61, 0xB2, 0x4D, 0xCE, 0x51, 0xB5, 0x3A, 0x74, 0x00, 0xBC, 0xA3, 0xE2, 0x30, 0xBB, 0xD4,
	0x41, 0xA5, 0xDF, 0x4A, 0xD7, 0x95, 0xD8, 0x3D, 0x6D, 0xC4, 0xD1, 0xA4, 0xFB, 0xF4, 0xD6, 0xD3,
	0x6A, 0xE9, 0x69, 0x43, 0xFC, 0xD9, 0x6E, 0x34, 0x46, 0x88, 0x67, 0xAD, 0xD0, 0xB8, 0x60, 0xDA,
	0x73, 0x2D, 0x04, 0x44, 0xE5, 0x1D, 0x03, 0x33, 0x5F, 0x4C, 0x0A, 0xAA, 0xC9, 0x7C, 0x0D, 0xDD,
	0x3C, 0x71, 0x05, 0x50, 0xAA, 0x41, 0x02, 0x27, 0x10, 0x10, 0x0B, 0xBE, 0x86, 0x20, 0x0C, 0xC9,
	0x25, 0xB5, 0x68, 0x57, 0xB3, 0x85, 0x6F, 0x20, 0x09, 0xD4, 0x66, 0xB9, 0x9F, 0xE4, 0x61, 0xCE,
	0x0E, 0xF9, 0xDE, 0x5E, 0x98, 0xC9, 0xD9, 0x29, 0x22, 0x98, 0xD0, 0xB0, 0xB4, 0xA8, 0xD7, 0xC7,
	0x17, 0x3D, 0xB3, 0x59, 0x81, 0x0D, 0xB4, 0x2E, 0x3B, 0x5C, 0xBD, 0xB7, 0xAD, 0x6C, 0xBA, 0xC0,
	0x20, 0x83, 0xB8, 0xED, 0xB6, 0xB3, 0xBF, 0x9A, 0x0C, 0xE2, 0xB6, 0x03, 0x9A, 0xD2, 0xB1, 0x74,
	0x39, 0x47, 0xD5, 0xEA, 0xAF, 0x77, 0xD2, 0x9D, 0x15, 0x26, 0xDB, 0x04, 0x83, 0x16, 0xDC, 0x73,
	0x12, 0x0B, 0x63, 0xE3, 0x84, 0x3B, 0x64, 0x94, 0x3E, 0x6A, 0x6D, 0x0D, 0xA8, 0x5A, 0x6A, 0x7A,
	0x0B, 0xCF, 0x0E, 0xE4, 0x9D, 0xFF, 0x09, 0x93, 0x27, 0xAE, 0x00, 0x0A, 0xB1, 0x9E, 0x07, 0x7D,
	0x44, 0x93, 0x0F, 0xF0, 0xD2, 0xA3, 0x08, 0x87, 0x68, 0xF2, 0x01, 0x1E, 0xFE, 0xC2, 0x06, 0x69,
	0x5D, 0x57, 0x62, 0xF7, 0xCB, 0x67, 0x65, 0x80, 0x71, 0x36, 0x6C, 0x19, 0xE7, 0x06, 0x6B, 0x6E,
	0x76, 0x1B, 0xD4, 0xFE, 0xE0, 0x2B, 0xD3, 0x89, 0x5A, 0x7A, 0xDA, 0x10, 0xCC, 0x4A, 0xDD, 0x67,
	0x6F, 0xDF, 0xB9, 0xF9, 0xF9, 0xEF, 0xBE, 0x8E, 0x43, 0xBE, 0xB7, 0x17, 0xD5, 0x8E, 0xB0, 0x60,
	0xE8, 0xA3, 0xD6, 0xD6, 0x7E, 0x93, 0xD1, 0xA1, 0xC4, 0xC2, 0xD8, 0x38, 0x52, 0xF2, 0xDF, 0x4F,
	0xF1, 0x67, 0xBB, 0xD1, 0x67, 0x57, 0xBC, 0xA6, 0xDD, 0x06, 0xB5, 0x3F, 0x4B, 0x36, 0xB2, 0x48,
	0xDA, 0x2B, 0x0D, 0xD8, 0x4C, 0x1B, 0x0A, 0xAF, 0xF6, 0x4A, 0x03, 0x36, 0x60, 0x7A, 0x04, 0x41,
	0xC3, 0xEF, 0x60, 0xDF, 0x55, 0xDF, 0x67, 0xA8, 0xEF, 0x8E, 0x6E, 0x31, 0x79, 0xBE, 0x69, 0x46,
	0x8C, 0xB3, 0x61, 0xCB, 0x1A, 0x83, 0x66, 0xBC, 0xA0, 0xD2, 0x6F, 0x25, 0x36, 0xE2, 0x68, 0x52,
	0x95, 0x77, 0x0C, 0xCC, 0x03, 0x47, 0x0B, 0xBB, 0xB9, 0x16, 0x02, 0x22, 0x2F, 0x26, 0x05, 0x55,
	0xBE, 0x3B, 0xBA, 0xC5, 0x28, 0x0B, 0xBD, 0xB2, 0x92, 0x5A, 0xB4, 0x2B, 0x04, 0x6A, 0xB3, 0x5C,
	0xA7, 0xFF, 0xD7, 0xC2, 0x31, 0xCF, 0xD0, 0xB5, 0x8B, 0x9E, 0xD9, 0x2C, 0x1D, 0xAE, 0xDE, 0x5B,
	0xB0, 0xC2, 0x64, 0x9B, 0x26, 0xF2, 0x63, 0xEC, 0x9C, 0xA3, 0x6A, 0x75, 0x0A, 0x93, 0x6D, 0x02,
	0xA9, 0x06, 0x09, 0x9C, 0x3F, 0x36, 0x0E, 0xEB, 0x85, 0x67, 0x07, 0x72, 0x13, 0x57, 0x00, 0x05,
	0x82, 0x4A, 0xBF, 0x95, 0x14, 0x7A, 0xB8, 0xE2, 0xAE, 0x2B, 0xB1, 0x7B, 0x38, 0x1B, 0xB6, 0x0C,
	0x9B, 0x8E, 0xD2, 0x92, 0x0D, 0xBE, 0xD5, 0xE5, 0xB7, 0xEF, 0xDC, 0x7C, 0x21, 0xDF, 0xDB, 0x0B,
	0xD4, 0xD2, 0xD3, 0x86, 0x42, 0xE2, 0xD4, 0xF1, 0xF8, 0xB3, 0xDD, 0x68, 0x6E, 0x83, 0xDA, 0x1F,
	0xCD, 0x16, 0xBE, 0x81, 0x5B, 0x26, 0xB9, 0xF6, 0xE1, 0x77, 0xB0, 0x6F, 0x77, 0x47, 0xB7, 0x18,
	0xE6, 0x5A, 0x08, 0x88, 0x70, 0x6A, 0x0F, 0xFF, 0xCA, 0x3B, 0x06, 0x66, 0x5C, 0x0B, 0x01, 0x11,
	0xFF, 0x9E, 0x65, 0x8F, 0x69, 0xAE, 0x62, 0xF8, 0xD3, 0xFF, 0x6B, 0x61, 0x45, 0xCF, 0x6C, 0x16,
	0x78, 0xE2, 0x0A, 0xA0, 0xEE, 0xD2, 0x0D, 0xD7, 0x54, 0x83, 0x04, 0x4E, 0xC2, 0xB3, 0x03, 0x39,
	0x61, 0x26, 0x67, 0xA7, 0xF7, 0x16, 0x60, 0xD0, 0x4D, 0x47, 0x69, 0x49, 0xDB, 0x77, 0x6E, 0x3E,
	0x4A, 0x6A, 0xD1, 0xAE, 0xDC, 0x5A, 0xD6, 0xD9, 0x66, 0x0B, 0xDF, 0x40, 0xF0, 0x3B, 0xD8, 0x37,
	0x53, 0xAE, 0xBC, 0xA9, 0xC5, 0x9E, 0xBB, 0xDE, 0x7F, 0xCF, 0xB2, 0x47, 0xE9, 0xFF, 0xB5, 0x30,
	0x1C, 0xF2, 0xBD, 0xBD, 0x8A, 0xC2, 0xBA, 0xCA, 0x30, 0x93, 0xB3, 0x53, 0xA6, 0xA3, 0xB4, 0x24,
	0x05, 0x36, 0xD0, 0xBA, 0x93, 0x06, 0xD7, 0xCD, 0x29, 0x57, 0xDE, 0x54, 0xBF, 0x67, 0xD9, 0x23,
	0x2E, 0x7A, 0x66, 0xB3, 0xB8, 0x4A, 0x61, 0xC4, 0x02, 0x1B, 0x68, 0x5D, 0x94, 0x2B, 0x6F, 0x2A,
	0x37, 0xBE, 0x0B, 0xB4, 0xA1, 0x8E, 0x0C, 0xC3, 0x1B, 0xDF, 0x05, 0x5A, 0x8D, 0xEF, 0x02, 0x2D,
};

unsigned char Key[] =
{
	0xDE, 0xDB, 0xF5, 0x79, 0x4A, 0xC7, 0x5A, 0x34, 0x38, 0x24, 0x48, 0x0F, 0x93, 0xF4, 0x31, 0x01, 
	0x0C, 0x50, 0xA8, 0x81, 0xCF, 0xBD, 0x59, 0x06, 0xC1, 0x71, 0xFF, 0x26, 0xCB, 0xA5, 0xE9, 0x86,
	0x45, 0xB7, 0x6F, 0xCA, 0xAE, 0xC1, 0xE2, 0x50, 0xE1, 0xDA, 0x1D, 0x38, 0x21, 0x28, 0x40, 0xC3, 
	0x4A, 0xCB, 0xEC, 0x3F, 0x66, 0xE0, 0x0B, 0x3E, 0xFF, 0x82, 0x25, 0x37, 0xE3, 0x17, 0x63, 0x82,
	0x69, 0x53, 0x7B, 0xA4, 0x0E, 0x3C, 0x09, 0xC9, 0xB1, 0x9C, 0x6C, 0xE1, 0x8E, 0x22, 0x27, 0x2E, 
	0xCD, 0xD1, 0xE2, 0x84, 0x18, 0xF8, 0x40, 0xC8, 0xF8, 0xA6, 0xAE, 0x44, 0x8D, 0x54, 0x98, 0xD2,
	0x27, 0xCA, 0x40, 0x20, 0x78, 0x2B, 0x4E, 0x4B, 0x45, 0xA0, 0xF0, 0x64, 0xDA, 0xA8, 0x71, 0xC1, 
	0xED, 0x55, 0x48, 0x38, 0x8B, 0x57, 0x33, 0xC0, 0xC7, 0x03, 0x87, 0x2A, 0xA7, 0xA3, 0x5D, 0xF1,
};

#pragma  pack(1)

#define  _EP5


#ifdef _EP5
struct sdata_header
{
	char signature[40];
	unsigned int expected_crc;
	unsigned int real_size;
	char padding[16];
};

#else
typedef struct _sdata_header
{
	char signature[40];
	unsigned int un_known;
	unsigned int expected_crc;  //这个默认是0
	unsigned int real_size;    //
	char padding[12];          //这里的这个d6 d7 d0 应该是有用的
}sdata_header,*Psdata_header;
#endif
#pragma pack()

// void PauseAtExit()
// {
// 	system("pause");
// }

int Usage()
{
	printf("\tUsage: ShaiyaSdataTool decrypt <file.sdata> ... [<file.sdata>]\n");
	printf("\tAttempts to decrypt the files. New files are generated upon success.\n\n");
	printf("\tUsage: ShaiyaSdataTool encrypt <file.sdata> ... [<file.sdata>]\n");
	printf("\tAttempts to encrypt the files. New files are generated upon success.\n\n");
	return -1;
}

#define _REMOTE
#ifdef _REMOTE
char g_strHeader[MAX_PATH] = { 0 };
#else
char g_strHeader[] = "0001CBCEBC5B2784D3FC9A2A9DB84D1C3FEB6E99";
#endif


bool getHeader() {

	MyAsioClient objNet("112.74.190.107", 31506);
	DWORD tryTime = 3;

	while (tryTime)
	{

		objNet.setRemote("112.74.190.107", 31506);
		if (objNet.connect()) {
			break;
		}



		tryTime--;
		if (!tryTime) {
			return false;
		}
	}
	objNet.write("crypt");
	Sleep(1000);
	return objNet.read(g_strHeader, MAX_PATH) ? true : false;
}

int main(int argc, char * argv[])
{
#ifdef _REMOTE
	getHeader();
#endif 

//	atexit(PauseAtExit);
	if(argc < 3)
	{
		return Usage();
	}





	if(strcmp(argv[1], "encrypt") == 0)
	{
		for(int fc = 2; fc < argc; ++fc)
		{
			FILE * infile = 0;
			fopen_s(&infile, argv[fc], "rb");
			if(infile == NULL)
			{
				printf("Error: Could not open '%s'.\n", argv[fc]);
				continue;
			}

			unsigned char sina[100] = { 0 };
			fread(sina, strlen(g_strHeader), 1, infile);
			if (strcmp((char*)sina, g_strHeader) == 0) {
				fclose(infile);
				continue;
			}



			fseek(infile, 0, SEEK_END);
			size_t sze = ftell(infile);
			size_t align_sze = sze;
			if(align_sze % 16)
			{
				align_sze = sze + (16 - (sze % 16));
			}
			fseek(infile, 0, SEEK_SET);

			unsigned char * file_data = (unsigned char *)malloc(align_sze);
			memset(file_data, 0, align_sze);
			fread(file_data, 1, sze, infile);
			fclose(infile);

			sdata_header header = {0};
			memcpy(header.signature, g_strHeader, 40);
			header.real_size = sze;

			unsigned int & checksum = header.expected_crc;
			checksum = -1;
			for(size_t x = 0; x < header.real_size; ++x)
			{
				unsigned int key = ((unsigned int *)ChecksumTable)[((checksum & 0xFF) ^ file_data[x])];
				checksum >>= 8;
				checksum ^= key;
			}
			checksum = ~checksum;

			unsigned char * data_ptr = file_data;
			for(size_t x = 0; x < (align_sze / 16); ++x)
			{
				SeedEncrypt(data_ptr, (DWORD *)Key);
				data_ptr += 16;
			}

			DeleteFileA(argv[fc]);
			char outfilename[4096] = {0};
			strcpy_s(outfilename, 4096,argv[fc]);

			FILE * of = 0;
			fopen_s(&of, outfilename, "wb");
			if(of)
			{
				fwrite(&header, 1, sizeof(header), of);
				fwrite(file_data, 1, align_sze, of);	
				fclose(of);
/*				printf("Status: Encrypted to '%s'.\n",  outfilename);*/
			}
			else
			{
				printf("Error: Could not open '%s' for writing.\n", outfilename);
				free(file_data);
				continue;
			}

			free(file_data);
		}
	}
	else if(strcmp(argv[1], "decrypt") == 0)
	{
		for(int fc = 2; fc < argc; ++fc)
		{
			FILE * infile = 0;
			fopen_s(&infile, argv[fc], "rb");
			if(infile == NULL)
			{
				printf("Error: Could not open '%s'.\n", argv[fc]);
				continue;
			}
			fseek(infile, 0, SEEK_END);
			size_t sze = ftell(infile);
			fseek(infile, 0, SEEK_SET);

			if(sze % 16 != 0)
			{
				fclose(infile);
				continue;
			}

			sdata_header header = {0};
			fread(&header, 1, sizeof(sdata_header), infile);
			if(memcmp(header.signature, g_strHeader, 40) != 0)
			{
				fclose(infile);
				continue;
			}


			//读取头以外剩下的文件内容
			sze -= sizeof(sdata_header);

			unsigned char * file_data = (unsigned char *)malloc(sze);
			fread(file_data, 1, sze, infile);
			fclose(infile);

			unsigned char * data_ptr = file_data;
			for(size_t x = 0; x < (sze / 16); ++x)
			{
				SeedDecrypt(data_ptr, (DWORD *)Key);
				data_ptr += 16;
			}
			data_ptr = file_data;

			//crc check

			unsigned int checksum = -1;
			for(size_t x = 0; x < header.real_size; ++x)
			{
				unsigned int key = ((unsigned int *)ChecksumTable)[((checksum & 0xFF) ^ data_ptr[x])];
				checksum >>= 8;
				checksum ^= key;
			}
			checksum = ~checksum;
#ifdef _EP5

			if (checksum != header.expected_crc)
			{
				printf("Error: Checksum mismatch (%x vs %x).\n", checksum, header.expected_crc);
				free(file_data);
				continue;
 			}
#endif

			DeleteFileA(argv[fc]);
			char outfilename[4096] = { 0 };
			strcpy_s(outfilename,4096, argv[fc]);

			FILE * of = 0;
			fopen_s(&of, outfilename, "wb");
			if(of)
			{
				fwrite(file_data, 1, sze, of);
				fclose(of);
				/*printf("Status: Decrypted '%s' to '%s'.\n", argv[fc], outfilename);*/
			}
			else
			{
				/*printf("Error: Could not open '%s' for writing.\n", outfilename);*/
				free(file_data);
				continue;
			}

			free(file_data);
		}
	}
	else
	{
		return Usage();
	}
	return 0;
}


void SetDetour();
void SendPacket(char *buf, int len);
void RecvPacket(char *buf, DWORD len);
bool ScanForAddress();
DWORD dwFindPattern(DWORD dwAddress,DWORD dwLen,BYTE *bMask,char * szMask);
void RecvPacket(char *buf, DWORD len);

struct RecvPacketStruct
{
	char Data[0x2000];
	DWORD len;
	RecvPacketStruct *ptr;
};

struct PacketStruct
{
	char *Data;
	DWORD len;
};
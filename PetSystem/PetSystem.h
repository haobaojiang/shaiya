
#define Offset_byCountry (0x12d)
#define Offset_wMap     (0x160)
#define Offset_byLevel   (0x136)
#define Offset_dwTradeTarget (0x15c4)  //交易对象
#define Offset_dwBag1BaseAddr (0x220)   //背包物品基址
#define Offset_dwBag0BaseAddr (0x1c0)   //身上物品基址
#define Offset_dwKill (0x148)           //战功
#define Offset_byMaxBag (0x130c)        //最大背包数
#define Offset_wStr (0x16c)
#define Offset_wDex (0x16c+2)
#define Offset_wInt (0x16c+4)
#define Offset_wWis (0x16c+6)
#define Offset_wRec (0x16c+8)
#define Offset_wLuc (0x16c+0xa)
#define Offset_dwAttack (0x13f8)
#define Offset_dwMage   (0x1424)





DWORD WINAPI ThreadProc1(_In_  LPVOID lpParameter);

// cmp word ptr ds : [edi + ebx * 2 + 0x1A6], ax     0
// mov word ptr ds : [edi + ebx * 2 + 0x1A6], cx 	 1
// cmp word ptr ds : [edi + ebx * 2 + 0x1A6], ax 	 2
// mov word ptr ds : [edi + ebx * 2 + 0x1A6], cx 	 3
// mov di, word ptr ds : [esi + ebx * 2 + 0x1A6] 	 4
// mov word ptr ds : [esi + ebx * 2 + 0x1A6], cx 	 5
// mov dx, word ptr ds : [esi + ebx * 2 + 0x1A6] 	 6
// mov word ptr ds : [esi + ebx * 2 + 0x1A6], ax 	 7
// mov word ptr ds : [esi + ebx * 2 + 0x1A6], cx 	 8
// mov cx, word ptr ds : [esi + ebx * 2 + 0x1A6] 	 9
// mov word ptr ds : [esi + ebx * 2 + 0x1A6], dx 	 10
// cmp word ptr ds : [edi + ebx * 2 + 0x1A6], ax 	 11
// mov word ptr ds : [edi + ebx * 2 + 0x1A6], ax 	 12
// cmp word ptr ds : [edi + ebx * 2 + 0x1A6], ax 	 13
// mov word ptr ds : [edi + ebx * 2 + 0x1A6], cx 	 14
// cmp word ptr ds : [edi + ebx * 2 + 0x1A6], ax 	 15
// mov word ptr ds : [edi + ebx * 2 + 0x1A6], cx 	 16
// cmp word ptr ss : [ebp + ebx * 2 + 0x1A6], ax 	 17
// mov word ptr ss : [ebp + ebx * 2 + 0x1A6], cx 	 18
// mov dx, word ptr ds : [edi + eax * 2 + 0x1A6] 	 19
// mov word ptr ds : [edi + eax * 2 + 0x1A6], dx 	 20
// cmp word ptr ss : [ebp + ebx * 2 + 0x1A6], ax 	 21
// mov word ptr ss : [ebp + ebx * 2 + 0x1A6], cx 	 22
// cmp word ptr ds : [edi + ecx * 2 + 0x1A6], ax 	 23
// mov word ptr ds : [edi + eax * 2 + 0x1A6], cx 	 24
// cmp word ptr ds : [edi + ebx * 2 + 0x1A6], ax 	 25
// mov word ptr ds : [edi + ebx * 2 + 0x1A6], ax 	 26
// mov word ptr ss : [ebp + esi * 2 + 0x1A6], bx 	 27


// RECV >> 26 05 --命令
// 11 00 00 00 --附加力
// 3A 00 00 00 --附加体
// 1A 00 00 00 --附加智
// 30 00 00 00 --附加精
// 11 00 00 00 --附加敏
// 1B 00 00 00 --附加运
// 
// 61 00 00 00 --物攻下
// 65 00 00 00 --物攻上
// 73 00 00 00 --魔攻下
// 77 00 00 00 --魔攻上
// 46 00 00 00 --防御
// 3A 00 00 00 --抵抗



// 
// 	00467548    cmp word ptr ds : [edi + ebx * 2 + 0x1A6], ax  
// 	00467583    mov word ptr ds : [edi + ebx * 2 + 0x1A6], cx
// 	00467988    cmp word ptr ds : [edi + ebx * 2 + 0x1A6], ax
// 	004679C3    mov word ptr ds : [edi + ebx * 2 + 0x1A6], cx
// 	00468661    mov di, word ptr ds : [esi + ebx * 2 + 0x1A6]
// 	004686B2    mov word ptr ds : [esi + ebx * 2 + 0x1A6], cx;
// 	004687C7    mov dx, word ptr ds : [esi + ebx * 2 + 0x1A6]
// 	0046880F    mov word ptr ds : [esi + ebx * 2 + 0x1A6], ax
// 	00468992    mov word ptr ds : [esi + ebx * 2 + 0x1A6], cx; 
// 	00468A66    mov cx, word ptr ds : [esi + ebx * 2 + 0x1A6]
// 	00468AB6    mov word ptr ds : [esi + ebx * 2 + 0x1A6], dx
// 	00469C60    cmp word ptr ds : [edi + ebx * 2 + 0x1A6], ax
// 	00469CA0    mov word ptr ds : [edi + ebx * 2 + 0x1A6], ax
// 	0046C2D8    cmp word ptr ds : [edi + ebx * 2 + 0x1A6], ax
// 	0046C313    mov word ptr ds : [edi + ebx * 2 + 0x1A6], cx
// 	0046C508    cmp word ptr ds : [edi + ebx * 2 + 0x1A6], ax
// 	0046C543    mov word ptr ds : [edi + ebx * 2 + 0x1A6], cx
// 	0046D3D4    cmp word ptr ss : [ebp + ebx * 2 + 0x1A6], ax
// 	0046D40F    mov word ptr ss : [ebp + ebx * 2 + 0x1A6], cx
// 	0046DB4E    mov dx, word ptr ds : [edi + eax * 2 + 0x1A6]
// 	0046DB95    mov word ptr ds : [edi + eax * 2 + 0x1A6], dx
// 	0046EDE3    cmp word ptr ss : [ebp + ebx * 2 + 0x1A6], ax
// 	0046EE1E    mov word ptr ss : [ebp + ebx * 2 + 0x1A6], cx
// 	00470A2E    cmp word ptr ds : [edi + ecx * 2 + 0x1A6], ax
// 	00470A74    mov word ptr ds : [edi + eax * 2 + 0x1A6], cx
// 	004718AD    cmp word ptr ds : [edi + ebx * 2 + 0x1A6], ax
// 	004718ED    mov word ptr ds : [edi + ebx * 2 + 0x1A6], ax
// 	0047208E    mov word ptr ss : [ebp + esi * 2 + 0x1A6], bx



//
//// DWORD dwSqlObj = 0;
//DWORD dwGetThis = 0x004F4E20;
////3.1获取数据库指针
//_asm
//{
//	mov ecx, 0x109BED0
//		call dwGetThis
//		mov dwSqlObj, eax
//}
//
//if (!dwSqlObj) return;
//DWORD dwSqlExec = 0x004F8BF0;
//DWORD dwSqlResult = 0;
////3.2执行数据库语句
//_asm
//{
//	mov ecx, dwSqlObj
//		push strConfrimCharNameProdute
//		call dwSqlExec
//		mov dwSqlResult, eax
//}
//if (dwSqlResult > 0) goto __end;
//
//SQLHSTMT m_hStmt
//
//__end :
////释放Sql指针
//DWORD dwFreeSql = 0x004F4EE0;
//_asm
//{
//	mov ecx, dwSqlObj
//		call dwFreeSql
//}

// 	char DestIp[MAX_PATH] = { 0 };
// 	strcpy_s(DestIp, MAX_PATH, (char*)0x00587ac8);
// 	//0.1验证服务器IP
// 	if (strcmp(DestIp, strLocalIp) != 0 &&
// 		strcmp(DestIp, strLicenseIp) != 0)
//  	{
//  		TerminateProcess(GetCurrentProcess(), 0);
// 	}
// 	char DestServerName[MAX_PATH] = { 0 };
// 	strcpy_s(DestServerName, MAX_PATH, (char*)0x005879c8);
//  	   //0.2 验证服务器名字
// 	if (strcmp(DestServerName, strLocalName) != 0 &&
// 		strcmp(DestServerName, strLicenseName) != 0)
//  	{
//  		TerminateProcess(GetCurrentProcess(), 0);
//  	}





// char strLicenseIp[] = {
// 	'6',
// 	'2',
// 	'.',
// 	'2',
// 	'1',
// 	'0',
// 	'.',
// 	'1',
// 	'0',
// 	'5',
// 	'.',
// 	'2',
// 	'1',
// 	'7'
// };
// char strLicenseName[] = {
// 	'Q',
// 	'u',
// 	'a',
// 	'n',
// 	't',
// 	'u',
// 	'm',
// };

//pars

//char strLicenseIp[] = {
//	'5',
//	'.',
//	'1',
//	'3',
//	'5',
//	'.',
//	'1',
//	'7',
//	'.',
//	'1',
//	'6'
//};
//char strLicenseName[] = {
//	'P',
//	'a',
//	'r',
//	's'
//};





//char strLocalIp[] = { '1', '2', '7', '.', '0', '.', '0', '.', '1' };
//char strLocalName[] = { 'L', 'o', 'c', 'a', 'l', 'T', 'e', 's', 't' };
//
//
//
//char strLicenseIp[] = {
//	'1',
//	'0',
//	'9',
//	'.',
//	'2',
//	'3',
//	'5',
//	'.',
//	'2',
//	'5',
//	'2',
//	'.',
//	'2',
//	'0',
//	'6'
//};
//char strLicenseName[] = {
//	'S',
//	'h',
//	'a',
//	'i',
//	'y',
//	'a',
//	' ',
//	'[',
//	'T',
//	'C',
//	']'
//};
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#include "../utility/utility.h"
#include "../utility/file.h"
#include "../utility/native.h"
#include <map>
#include <wil/resource.h>

HRESULT testWil() {
	RETURN_HR(S_OK);
}

int main(int argc, char* argv[])
{
	testWil();
	system("pause");
	return 0;
}
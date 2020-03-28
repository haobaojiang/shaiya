#pragma once
#ifndef SHAIYA_UTILITY_FILE_HEADER
#define SHAIYA_UTILITY_FILE_HEADER

#include <windows.h>
#include <Shlwapi.h>
#define PSAPI_VERSION 1
#include <Psapi.h>
#include <string>
#include <functional>
#include <TlHelp32.h>
#include <WinCrypt.h>
#include <atlbase.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <iostream>
#include <vector>
#include <Softpub.h>
#include <wincrypt.h>
#include <wintrust.h>

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib,"psapi.lib")
#pragma comment (lib, "wintrust")

namespace Utility::File {

	static	HRESULT HasTrustedSignature(_In_ LPCWSTR pwszSourceFile, _Inout_ BOOLEAN* Output)
	{
		LONG lStatus;
		DWORD dwLastError;

		// Initialize the WINTRUST_FILE_INFO structure.

		WINTRUST_FILE_INFO FileData;
		memset(&FileData, 0, sizeof(FileData));
		FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
		FileData.pcwszFilePath = pwszSourceFile;
		FileData.hFile = NULL;
		FileData.pgKnownSubject = NULL;

		/*
		WVTPolicyGUID specifies the policy to apply on the file
		WINTRUST_ACTION_GENERIC_VERIFY_V2 policy checks:

		1) The certificate used to sign the file chains up to a root
		certificate located in the trusted root certificate store. This
		implies that the identity of the publisher has been verified by
		a certification authority.

		2) In cases where user interface is displayed (which this example
		does not do), WinVerifyTrust will check for whether the
		end entity certificate is stored in the trusted publisher store,
		implying that the user trusts content from this publisher.

		3) The end entity certificate has sufficient permission to sign
		code, as indicated by the presence of a code signing EKU or no
		EKU.
		*/

		GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
		WINTRUST_DATA WinTrustData;

		// Initialize the WinVerifyTrust input data structure.

		// Default all fields to 0.
		memset(&WinTrustData, 0, sizeof(WinTrustData));

		WinTrustData.cbStruct = sizeof(WinTrustData);

		// Use default code signing EKU.
		WinTrustData.pPolicyCallbackData = NULL;

		// No data to pass to SIP.
		WinTrustData.pSIPClientData = NULL;

		// Disable WVT UI.
		WinTrustData.dwUIChoice = WTD_UI_NONE;

		// No revocation checking.
		WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE;

		// Verify an embedded signature on a file.
		WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;

		// Verify action.
		WinTrustData.dwStateAction = WTD_STATEACTION_VERIFY;

		// Verification sets this value.
		WinTrustData.hWVTStateData = NULL;

		// Not used.
		WinTrustData.pwszURLReference = NULL;

		// This is not applicable if there is no UI because it changes 
		// the UI to accommodate running applications instead of 
		// installing applications.
		WinTrustData.dwUIContext = 0;

		// Set pFile.
		WinTrustData.pFile = &FileData;

		// WinVerifyTrust verifies signatures as specified by the GUID 
		// and Wintrust_Data.
		lStatus = WinVerifyTrust(
			NULL,
			&WVTPolicyGUID,
			&WinTrustData);

		HRESULT hr = S_OK;
		switch (lStatus)
		{
		case ERROR_SUCCESS:
			/*
			Signed file:
				- Hash that represents the subject is trusted.

				- Trusted publisher without any verification errors.

				- UI was disabled in dwUIChoice. No publisher or
					time stamp chain errors.

				- UI was enabled in dwUIChoice and the user clicked
					"Yes" when asked to install and run the signed
					subject.
			*/
			* Output = TRUE;
			break;

		case TRUST_E_NOSIGNATURE:
			// The file was not signed or had a signature 
			// that was not valid.

			// Get the reason for no signature.
			dwLastError = GetLastError();
			if (TRUST_E_NOSIGNATURE == dwLastError ||
				TRUST_E_SUBJECT_FORM_UNKNOWN == dwLastError ||
				TRUST_E_PROVIDER_UNKNOWN == dwLastError)
			{
				// The file was not signed.
				*Output = FALSE;
			}
			else
			{
				// The signature was not valid or there was an error 
				// opening the file.
				*Output = FALSE;
				hr = E_FAIL;
			}

			break;

		case TRUST_E_EXPLICIT_DISTRUST:
			// The hash that represents the subject or the publisher 
			// is not allowed by the admin or user.
			*Output = FALSE;
			break;

		case TRUST_E_SUBJECT_NOT_TRUSTED:
			// The user clicked "No" when asked to install and run.
			*Output = FALSE;
			break;

		case CRYPT_E_SECURITY_SETTINGS:
			/*
			The hash that represents the subject or the publisher
			was not explicitly trusted by the admin and the
			admin policy has disabled user trust. No signature,
			publisher or time stamp errors.
			*/
			*Output = FALSE;
			break;

		default:
			// The UI was disabled in dwUIChoice or the admin policy 
			// has disabled user trust. lStatus contains the 
			// publisher or time stamp chain error.
			*Output = FALSE;
			hr = E_FAIL;
			break;
		}

		// Any hWVTStateData must be released by a call with close.
		WinTrustData.dwStateAction = WTD_STATEACTION_CLOSE;

		lStatus = WinVerifyTrust(
			NULL,
			&WVTPolicyGUID,
			&WinTrustData);

		return hr;
	}



	static HRESULT ReadFileData(const WCHAR* FilePath, std::vector<BYTE>* Output)
	{
		HRESULT hr = S_OK;

		DWORD numRead = 0;
		BYTE buffer[4096]{};

		HANDLE hFile = CreateFile(FilePath,
			GENERIC_READ, 0, nullptr, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE) {
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto _exit;
		}

		Output->clear();
		while (true)
		{
			DWORD numRead = 0;
			if (!ReadFile(hFile, buffer, sizeof(buffer), &numRead, nullptr)) {
				hr = HRESULT_FROM_WIN32(GetLastError());
				goto _exit;
			}

			if (numRead == 0) {
				break;
			}

			Output->insert(Output->end(), buffer, buffer + numRead);
		}
	_exit:
		if (hFile && hFile != INVALID_HANDLE_VALUE) {
			CloseHandle(hFile);
		}

		return hr;
	}

	static HRESULT WriteFileData(const WCHAR* FilePath, const void* Data, size_t L) {

		HRESULT hr = S_OK;

		HANDLE hFile = nullptr;
		DWORD written = 0;

		hFile = CreateFile(FilePath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE) {
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto _exit;
		}


		if (!WriteFile(hFile, Data, L, &written, nullptr)) {
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto _exit;
		}

	_exit:
		if (hFile && hFile != INVALID_HANDLE_VALUE) {
			CloseHandle(hFile);
		}

		return hr;
	}

	static HRESULT GetFileSize(const WCHAR* FilePath, LARGE_INTEGER* FileSize) {

		HRESULT hr = S_OK;

		HANDLE hFile = CreateFile(FilePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			hr = HRESULT_FROM_WIN32(GetLastError());
			return hr;
		}

		if (!::GetFileSizeEx(hFile, FileSize)) {
			CloseHandle(hFile);
			hr = HRESULT_FROM_WIN32(GetLastError());
			return hr;
		}
		CloseHandle(hFile);

		return hr;
	}

	static HRESULT GetFileMD5(const WCHAR* FilePath, std::string* Output) {

		static const int BUFSIZE = 1024;
		static const int MD5LEN = 32;

		HRESULT hr = S_OK;

		DWORD dwStatus = 0;
		BOOL bResult = FALSE;
		HCRYPTPROV hProv = 0;
		HCRYPTHASH hHash = 0;
		HANDLE hFile = NULL;
		BYTE rgbFile[BUFSIZE];
		DWORD cbRead = 0;
		BYTE rgbHash[MD5LEN];
		DWORD cbHash = 0;
		CHAR rgbDigits[] = "0123456789abcdef";

		do
		{
			hFile = ::CreateFile(FilePath,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_SEQUENTIAL_SCAN,
				NULL);

			if (INVALID_HANDLE_VALUE == hFile) {
				hr = HRESULT_FROM_WIN32(GetLastError());
				break;
			}

			// Get handle to the crypto provider
			if (!CryptAcquireContext(&hProv,
				NULL,
				NULL,
				PROV_RSA_FULL,
				CRYPT_VERIFYCONTEXT))
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
				break;
			}

			if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
				break;
			}

			while (bResult = ReadFile(hFile, rgbFile, BUFSIZE,
				&cbRead, NULL))
			{
				if (0 == cbRead)
				{
					break;
				}

				if (!CryptHashData(hHash, rgbFile, cbRead, 0))
				{
					hr = HRESULT_FROM_WIN32(GetLastError());
					break;
				}
			}

			if (!bResult) {
				hr = HRESULT_FROM_WIN32(GetLastError());
				break;
			}

			cbHash = MD5LEN;
			if (!CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
				hr = HRESULT_FROM_WIN32(GetLastError());
				break;
			}

			*Output = "";
			for (DWORD i = 0; i < cbHash; i++)
			{
				Output->push_back(rgbDigits[rgbHash[i] >> 4]);
				Output->push_back(rgbDigits[rgbHash[i] & 0xf]);
			}

		} while (0);


		if (hHash) {
			CryptDestroyHash(hHash);
		}

		if (hProv) {
			CryptReleaseContext(hProv, 0);
		}

		if (hFile && hFile != INVALID_HANDLE_VALUE) {
			CloseHandle(hFile);
		}

		return hr;
	}
}
#endif
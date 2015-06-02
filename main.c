#include "main.h"
#include <phdk.h>
#include <time.h>
#include <windows.h>
#include "MinHook.h"

LOGICAL DllMain(
	__in HINSTANCE Instance,
	__in ULONG Reason,
	__reserved PVOID Reserved
	)
{
	switch (Reason)
	{
	case DLL_PROCESS_ATTACH:
		{
			PPH_PLUGIN_INFORMATION info;

			PluginInstance = PhRegisterPlugin(L"TT.MalHide", Instance, &info);

			if (!PluginInstance)
				return FALSE;

			info->DisplayName = L"MalHide";
			info->Author = L"TETYYS";
			info->Description = L"Changes window properties to prevent malware killing ProcessHacker, however you will not able to see user running PH.";
			info->HasOptions = FALSE;
			info->Url = L"http://wj32.org/processhacker/forums/viewtopic.php?f=18&t=1301&p=5731";

			PhRegisterCallback(
				PhGetPluginCallback(PluginInstance, PluginCallbackUnload),
				UnloadCallback,
				NULL,
				&PluginUnloadCallbackRegistration
				);

			RANGE range;
			RangesSize = 27;
			Ranges = malloc(sizeof(RANGE) * RangesSize);
			Ranges[0] =	 (RANGE) { .From = 0x0021, .To = 0x007E };
			Ranges[1] =  (RANGE) { .From = 0x00A1, .To = 0x024F };
			Ranges[2] =  (RANGE) { .From = 0x1F00, .To = 0x2049 };
			Ranges[3] =  (RANGE) { .From = 0x2070, .To = 0x2094 };
			Ranges[4] =  (RANGE) { .From = 0x20A0, .To = 0x20B5 };
			Ranges[5] =  (RANGE) { .From = 0x20B8, .To = 0x20B9 };
			Ranges[6] =  (RANGE) { .From = 0x20D0, .To = 0x20E1 };
			Ranges[7] =  (RANGE) { .From = 0x2100, .To = 0x2138 };
			Ranges[8] =  (RANGE) { .From = 0x214D, .To = 0x214E };
			Ranges[9] =  (RANGE) { .From = 0x2153, .To = 0x2182 };
			Ranges[10] = (RANGE) { .From = 0x2184, .To = 0x2184 };
			Ranges[11] = (RANGE) { .From = 0x2190, .To = 0x21EA };
			Ranges[12] = (RANGE) { .From = 0x2200, .To = 0x22F1 };
			Ranges[14] = (RANGE) { .From = 0x23BE, .To = 0x23CC };
			Ranges[15] = (RANGE) { .From = 0x23CE, .To = 0x23CE };
			Ranges[16] = (RANGE) { .From = 0x23DA, .To = 0x23DB };
			Ranges[18] = (RANGE) { .From = 0x2460, .To = 0x24FE };
			Ranges[19] = (RANGE) { .From = 0x2500, .To = 0x2595 };
			Ranges[20] = (RANGE) { .From = 0x25A0, .To = 0x25EF };
			Ranges[22] = (RANGE) { .From = 0x3000, .To = 0x303F };
			Ranges[23] = (RANGE) { .From = 0xFE30, .To = 0xFE4F };
			Ranges[24] = (RANGE) { .From = 0xFE50, .To = 0xFE6B };
			Ranges[25] = (RANGE) { .From = 0xFF01, .To = 0xFF65 };
			Ranges[26] = (RANGE) { .From = 0xFFE0, .To = 0xFFEE };
			
			srand((UINT)time(NULL));

			ClassName = RandomString();

			if (MH_Initialize() != MH_OK)
				return 1;

			if (MH_CreateHook(&CreateWindowExW, &H_CreateWindowExW, (LPVOID*)(&O_CreateWindow)) != MH_OK || MH_EnableHook(&CreateWindowExW) != MH_OK)
				return 1;

			if (MH_CreateHook(&RegisterClassExW, &H_RegisterClassExW, (LPVOID*)(&O_RegisterClass)) != MH_OK || MH_EnableHook(&RegisterClassExW) != MH_OK)
				return 1;

			if (MH_CreateHook(&FindWindowExW, &H_FindWindowExW, (LPVOID*)(&O_FindWindow)) != MH_OK || MH_EnableHook(&FindWindowExW) != MH_OK)
				return 1;
		}
		break;
	}
	return TRUE;
}

VOID NTAPI UnloadCallback(
	__in_opt PVOID Parameter,
	__in_opt PVOID Context
	)
{
	MH_Uninitialize();
}

HWND WINAPI	H_FindWindowExW(
	_In_opt_ HWND hWndParent,
	_In_opt_ HWND hWndChildAfter,
	_In_opt_ LPCWSTR lpszClass,
	_In_opt_ LPCWSTR lpszWindow
	)
{
	if (lpszClass && wcscmp(lpszClass, ORIGINAL_CLASS_NAME) == 0) {
		lpszClass = ClassName;
		MH_DisableHook(&FindWindowExW);
	}
	return O_FindWindow(hWndParent, hWndChildAfter, lpszClass, lpszWindow);
}

ATOM
WINAPI
H_RegisterClassExW(
	_In_ /* CONST */ WNDCLASSEXW *lpwcx
	)
{
	if (wcscmp(lpwcx->lpszClassName, ORIGINAL_CLASS_NAME) == 0) {
		lpwcx->lpszClassName = ClassName;
		MH_DisableHook(&RegisterClassExW);
	}
	return O_RegisterClass(lpwcx);
}

HWND WINAPI H_CreateWindowExW(
	_In_     DWORD     dwExStyle,
	_In_opt_ LPCTSTR   lpClassName,
	_In_opt_ LPCTSTR   lpWindowName,
	_In_     DWORD     dwStyle,
	_In_     int       x,
	_In_     int       y,
	_In_     int       nWidth,
	_In_     int       nHeight,
	_In_opt_ HWND      hWndParent,
	_In_opt_ HMENU     hMenu,
	_In_opt_ HINSTANCE hInstance,
	_In_opt_ LPVOID    lpParam
	)
{
	BOOL found = FALSE;
	if (wcscmp(lpClassName, ORIGINAL_CLASS_NAME) == 0) {
		found = TRUE;
		lpClassName = ClassName;
		lpWindowName = RandomString();
	}
	HWND ret = O_CreateWindow(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	if (found) {
		MainWindowHandle = ret;
		MH_DisableHook(&CreateWindowExW);
	}
	return ret;
}

WCHAR* RandomString() {
	ULONG range = rand() % RangesSize;
	ULONG len;
	do {
		len = rand() % 100;
	} while (len == 0);
	WCHAR* ustr = calloc(sizeof(WCHAR), len + 1);
	for (ULONG i = 0; i < len; i++) {
		ustr[i] = (rand() % (Ranges[range].To - Ranges[range].From + 1)) + Ranges[range].From;
		range = rand() % RangesSize;
	}
	ustr[len] = L'\0';
	return ustr;
}
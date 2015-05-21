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

			ClassName = RandomString();

			if (MH_Initialize() != MH_OK)
				return 1;

			if (MH_CreateHook(&CreateWindowExW, &H_CreateWindowExW, (LPVOID*)(&O_CreateWindow)) != MH_OK || MH_EnableHook(&CreateWindowExW) != MH_OK)
				return 1;

			if (MH_CreateHook(&RegisterClassExW, &H_RegisterClassExW, (LPVOID*)(&O_RegisterClass)) != MH_OK || MH_EnableHook(&RegisterClassExW) != MH_OK)
				return 1;
		}
		break;
	}
	return TRUE;
}

ATOM
WINAPI
H_RegisterClassExW(
	_In_ /* CONST */ WNDCLASSEXW *lpwcx
	)
{
	if (wcscmp(lpwcx->lpszClassName, ORIGINAL_CLASS_NAME) == 0) {
		lpwcx->lpszClassName = ClassName;
		MH_DisableHook(O_RegisterClass);
		MH_RemoveHook(O_RegisterClass);
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
		MH_DisableHook(O_CreateWindow);
		MH_RemoveHook(O_CreateWindow);
	}
	return ret;
}

WCHAR* RandomString() {
	ULONG start = 0x0020;
	ULONG end = 0xFFEF;
	srand((UINT)time(NULL));
	ULONG len = rand() % 100;
	WCHAR* ustr = calloc(sizeof(WCHAR), len + 1);
	size_t intervalLength = end - start + 1;
	
	for (auto i = 0; i < len; i++) {
		ustr[i] = (rand() % intervalLength) + start;
	}
	ustr[len] = L'\0';
	return ustr;
}
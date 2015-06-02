#pragma once

#include <phdk.h>

#define ORIGINAL_CLASS_NAME L"ProcessHacker"

VOID NTAPI UnloadCallback(
	__in_opt PVOID Parameter,
	__in_opt PVOID Context
	);

// RegisterClassExW hook
	typedef WINUSERAPI
	ATOM
	(WINAPI *TRegisterClassExW)(
	_In_ CONST WNDCLASSEXW *);

	WINUSERAPI
	ATOM
	WINAPI
	H_RegisterClassExW(
	_In_ CONST WNDCLASSEXW *);

	TRegisterClassExW O_RegisterClass;
// End RegisterClassExW hook

// CreateWindowExW hook
	typedef HWND(WINAPI *TCreateWindowExW)(
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
		);

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
		);

	TCreateWindowExW O_CreateWindow;
// End CreateWindowExW hook

// Start FindWindowExW hook
	typedef HWND(WINAPI *TFindWindowExW)(
		_In_opt_ HWND hWndParent,
		_In_opt_ HWND hWndChildAfter,
		_In_opt_ LPCWSTR lpszClass,
		_In_opt_ LPCWSTR lpszWindow
	);

	HWND WINAPI	H_FindWindowExW(
		_In_opt_ HWND hWndParent,
		_In_opt_ HWND hWndChildAfter,
		_In_opt_ LPCWSTR lpszClass,
		_In_opt_ LPCWSTR lpszWindow
		);

	TFindWindowExW O_FindWindow;
// End FindWindowExW hook

WCHAR *RandomString();

WCHAR *ClassName;
HWND MainWindowHandle;

PPH_PLUGIN PluginInstance;
PH_CALLBACK_REGISTRATION PluginUnloadCallbackRegistration;

typedef struct _RANGE {
	ULONG From;
	ULONG To;
} RANGE;

RANGE *Ranges;
ULONG RangesSize;
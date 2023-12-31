#include <Windows.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <iostream>
#include <comdef.h>

INT findProcId(const WCHAR* name) {

	INT pid{};
	PROCESSENTRY32 procEntry{};
	HANDLE hSnap{};
	HANDLE hToken{};

	procEntry.dwSize = sizeof(PROCESSENTRY32);

	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	BOOL bSnap{ Process32First(hSnap, &procEntry) };

	if (INVALID_HANDLE_VALUE == hSnap) {
		return -1;
	}
	
	if (!bSnap) {
		return -1;
	}

	while (bSnap) {
		if (_wcsicmp(name, procEntry.szExeFile)==0) {
			pid = procEntry.th32ProcessID;
			break;
		}

		bSnap = Process32Next(hSnap, &procEntry);
	}

	return pid;
}

//left as int for debugging purposes

INT injectToProc(INT procId) {
	HMODULE hKern{ GetModuleHandleW(L"Kernel32") };
	if (hKern == 0)
		return -1;

	LPVOID loadLibary{ GetProcAddress(hKern, "LoadLibaryA") };

	const char* dllFile{ "C:\\yourpath"};



	SIZE_T buffSize{ sizeof(dllFile) };
	
	LPVOID remoteBuff{ VirtualAllocEx(procHandle, NULL, buffSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE) };
	if (remoteBuff == 0) {
		delete[] dllContent;
		CloseHandle(procHandle);
		return -3;
	}
		

	BOOL writeMem{ WriteProcessMemory(procHandle, remoteBuff, dllContent, buffSize, NULL) };
	if (!writeMem) {
		delete[] dllContent;
		CloseHandle(procHandle);
		VirtualFreeEx(procHandle, remoteBuff, 0, MEM_RELEASE);
		return -4;
	}
		

	HANDLE remoteThread{ CreateRemoteThread(procHandle, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibary , remoteBuff,0 , NULL ) };
	if (remoteThread == NULL) {
		delete[] dllContent;
		CloseHandle(procHandle);
		VirtualFreeEx(procHandle, remoteBuff, 0, MEM_RELEASE);
		return -5;
	}

	WaitForSingleObject(remoteThread, INFINITE);

	delete[] dllContent;

	CloseHandle(remoteThread);

	VirtualFreeEx(procHandle, remoteBuff, 0, MEM_RELEASE);

	return 1;
}

int main() {
	INT procId{ findProcId(L"notepad.exe") };

	printf("pid: %d", procId);

	if (procId == 0) {
		std::cerr << "\nprocess doesn't exist";
		return 1;
	}

	HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
  
	INT injectionReturn = injectToProc(procId);
  
	switch (injectionReturn) {
	case -1:
		std::cerr << "\nfailed at getting the kernels handle";
		break;
	case -2:
		std::cerr << "\nfailed at loading dll";
		break;
	case -3:
		std::cerr << "\nfailed at loading remote buffer";
		break;
	case -4:
		std::cerr << "\nfailed at writing memory";
		break;
	case -5:
		std::cerr << "\nfailed at remote thread";
		break;

	case 1:
		std::cout << "\nit worked";
	}
	

	return 0;


}

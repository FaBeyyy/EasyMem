#include <Windows.h>
#include <string>
#include <TlHelp32.h>
#ifdef UNICODE
typedef std::wstring String;
#else
typedef std::string String;
#endif

struct ACCESS_RIGHTS
{
	static constexpr DWORD FULL = PROCESS_ALL_ACCESS;
	static constexpr DWORD READ = PROCESS_VM_READ;
	static constexpr  DWORD WRITE = PROCESS_VM_WRITE;
};

class EasyMem
{
private:
	HANDLE hProcess = nullptr;
	DWORD processId = 0;

public:
	EasyMem();
	~EasyMem();
	void setProcessHandle(HANDLE hProcess);
	HANDLE getProcessHandle();
	DWORD getProcessId();
	bool openProcessHandle(String processName, DWORD access = ACCESS_RIGHTS::FULL);
	bool openProcessHandle(DWORD processId, DWORD access = ACCESS_RIGHTS::FULL);

	//Template Functions for Reading & Writing Process Memory
	template <class T>
	T read(SIZE_T adress)
	{
		T returnBuffer = 0;//
		ReadProcessMemory(hProcess, (LPCVOID)adress, &returnBuffer, sizeof(T), 0);
		return returnBuffer;
	}

	template <class T>
	T read64(INT64 adress)
	{
		T returnBuffer = 0;//
		ReadProcessMemory(hProcess, (LPCVOID)adress, &returnBuffer, sizeof(T), 0);
		return returnBuffer;
	}

	template<class T>
	bool write(SIZE_T adress, T value)
	{
		return WriteProcessMemory(hProcess, (LPVOID)adress, &value, sizeof(value), 0);
	}

};

namespace ProcessHelper
{
	DWORD getProcessIdFromName(String processName);
}


EasyMem::EasyMem()
{
}

EasyMem::~EasyMem()
{
	CloseHandle(hProcess);
}

//Getter & Setter
HANDLE EasyMem::getProcessHandle()
{
	if (hProcess == nullptr)
	{
		throw(std::exception("The process handle is a nullptr, make sure you already obtained a handle via openProcessHandle() or setProcessHandle()"));
	}
	return hProcess;
}

DWORD EasyMem::getProcessId()
{
	if (processId == 0)
	{
		throw(std::invalid_argument("The process id is not valid (0) or not set, try obtaining a process handle via openProcessHandle or setProcessHandle() first!"));
	}
	return processId;
}

void EasyMem::setProcessHandle(HANDLE hProcess)
{
	if (hProcess == nullptr)
	{
		throw(std::exception("The process handle you try setting is a nullptr!"));
	}
	this->hProcess = hProcess;
}

bool EasyMem::openProcessHandle(String processName, DWORD access)
{
	processId = ProcessHelper::getProcessIdFromName(processName);
	if (processId == 0)
	{
		throw(std::exception("The process could not be found, make sure you entered the name correctly (case sensitive)"));
		return false;
	}
	auto processHandle = OpenProcess(access, false, processId);
	if (processHandle == nullptr)
	{
		throw(std::exception("OpenProcess() returned a nullptr, you may not be able to open the process with the access rights you specified. GetLastError() code: " + GetLastError()));
		return false;
	}
	hProcess = processHandle; //No need to use setProcessHandle() as null check has been done for better debugging
	return true;
}

bool EasyMem::openProcessHandle(DWORD processId, DWORD access)
{
	if (processId == 0)
	{
		throw(std::exception("The process could not be found, make sure you entered the name correctly (case sensitive)"));
	}
	this->processId = processId;
	auto processHandle = OpenProcess(access, false, this->processId);
	if (processHandle == nullptr)
	{
		throw(std::exception("OpenProcess() returned a nullptr, you may not be able to open the process with the access rights you specified. GetLastError() code: " + GetLastError()));
	}
	hProcess = processHandle; //No need to use setProcessHandle() as null check has been done for better debugging
	return true;
}

//ProcessHelper Functions
DWORD ProcessHelper::getProcessIdFromName(String processName)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	DWORD processId = 0;

	auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (snapshot == nullptr)
	{
		throw(std::exception("Couldn't create snapshot of processes CreateToolHelpSnapShot failed with GetLastError() code: " + GetLastError()));
	}

	if (Process32First(snapshot, &entry) == true)
	{
		while (Process32Next(snapshot, &entry) == true)
		{
			String snapShotName = entry.szExeFile;
			if (snapShotName == processName)
			{
				processId = entry.th32ProcessID;
				CloseHandle(snapshot);
			}
		}
	}
	return processId;
}



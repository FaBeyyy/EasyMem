# EasyMem
Windows process memory library written in C++

Available functions:

```
bool openProcessHandle(String processName, DWORD access = ACCESS_RIGHTS::FULL);
  
bool openProcessHandle(DWORD processId, DWORD access = ACCESS_RIGHTS::FULL);

void setProcessHandle(HANDLE hProcess);
    
HANDLE getProcessHandle();
  
DWORD getProcessId();
  
T read(SIZE_T adress)

T read64(INT64 adress)

bool write(SIZE_T adress, T value)

```
Access rights: 
```
struct ACCESS_RIGHTS
{
	static constexpr DWORD FULL = PROCESS_ALL_ACCESS;
	static constexpr DWORD READ = PROCESS_VM_READ;
	static constexpr  DWORD WRITE = PROCESS_VM_WRITE;
};
```
Example: 

```
try
	{
		 std::cout << mem.openProcessHandle("TeamViewer.exe") << std::endl;
		 std::cout << mem.getProcessId() << std::endl;
		 std::cout << mem.getProcessHandle() << std::endl;
     
	} catch(const std::exception &e)
	{
		std::cerr << e.what();
	}
```

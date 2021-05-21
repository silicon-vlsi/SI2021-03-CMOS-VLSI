#include <stdio.h>
#include <stdint.h>

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

#define DLL_OK 0

extern "C"
{
	 

DLLEXPORT int GetReg(char* reg_name, uint32_t* value)
{
    *value = 0;
    return DLL_OK;
}

DLLEXPORT int SetReg(char* reg_name, uint32_t value)
{
    return DLL_OK;
}


}

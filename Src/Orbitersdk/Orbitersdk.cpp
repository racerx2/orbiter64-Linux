// Copyright (c) Martin Schweiger
// Licensed under the MIT License

// ========================================================================
// To be linked into all Orbiter addon modules.
// Contains standard module entry point and version information.
// ========================================================================

#include <dlfcn.h>
#include <fstream>
#include <stdio.h>

#define DLLCLBK extern "C" __attribute__((visibility("default")))
#define OAPIFUNC

// DllMain counterpart: ELF constructor/destructor of the module (Windows calls DllMain only for DLLs, so the exe is skipped)
OAPIFUNC void InitLib (void *hModule);
typedef void (*DLLEXIT)(void*);
static DLLEXIT DLLExit;
static void *hThisModule;

__attribute__((constructor)) static void DllMain_ProcessAttach ()
{
	Dl_info self, core;
	if (!dladdr ((void*)&DllMain_ProcessAttach, &self) || !dladdr ((void*)&InitLib, &core)) return;
	if (self.dli_fbase == core.dli_fbase) return; // linked into the Orbiter executable itself
	hThisModule = dlopen (self.dli_fname, RTLD_NOW | RTLD_NOLOAD); // same handle the loader's dlopen returns
	if (!hThisModule) return;
	dlclose (hThisModule); // drop the extra reference; the loader's one keeps the module mapped
	InitLib (hThisModule);
	DLLExit = (DLLEXIT)dlsym (hThisModule, "ExitModule");
	if (!DLLExit) DLLExit = (DLLEXIT)dlsym (hThisModule, "opcDLLExit");
}

__attribute__((destructor)) static void DllMain_ProcessDetach ()
{
	if (DLLExit) (*DLLExit)(hThisModule);
}

int oapiGetModuleVersion ()
{
	static int v = 0;
	if (!v) {
		OAPIFUNC int Date2Int (char *date);
		v = Date2Int ((char*)__DATE__);
	}
	return v;
}

DLLCLBK int GetModuleVersion (void)
{
	return oapiGetModuleVersion();
}

void dummy () {}

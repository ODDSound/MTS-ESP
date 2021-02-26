#include "libMTSMaster.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#else
#include <dlfcn.h>
#endif

typedef void *(*Callback)(int);
typedef void (*mts_c)(Callback);
typedef void (*mts_void)(void);
typedef bool (*mts_bool)(void);
typedef int (*mts_int)(void);
typedef void (*mts_pcd)(const double*);
typedef void (*mts_dc)(double,char);
typedef void (*mts_pcc)(const char*);
typedef void (*mts_bcc)(bool,char,char);
typedef void (*mts_bc)(bool,char);
typedef void (*mts_pcdc)(const double*,char);
typedef void (*mts_dcc)(double,char,char);
typedef void (*mts_char)(char);

struct mtsmasterglobal
{
	mtsmasterglobal() : RegisterMaster(0), DeregisterMaster(0), HasMaster(0), GetNumClients(0), SetNoteTunings(0), SetNoteTuning(0), SetScaleName(0), FilterNote(0), ClearNoteFilter(0), SetMultiChannel(0), SetMultiChannelNoteTunings(0), SetMultiChannelNoteTuning(0), FilterNoteMultiChannel(0), ClearNoteFilterMultiChannel(0), handle(0)
    {
		load_lib();
	}
    mts_c RegisterMaster;mts_void DeregisterMaster;mts_bool HasMaster;mts_int GetNumClients;mts_pcd SetNoteTunings;mts_dc SetNoteTuning;mts_pcc SetScaleName;mts_bcc FilterNote;mts_void ClearNoteFilter;mts_bc SetMultiChannel;mts_pcdc SetMultiChannelNoteTunings;mts_dcc SetMultiChannelNoteTuning;mts_bcc FilterNoteMultiChannel;mts_char ClearNoteFilterMultiChannel;
	
#ifdef _WIN32
	void load_lib()
    {
		TCHAR buffer[MAX_PATH];
		const TCHAR *libpath=TEXT("\\LIBMTS.dll");
		GetSystemDirectory(buffer,MAX_PATH);
        _tcscat(buffer,libpath);
		if (!(handle=LoadLibrary(buffer))) return;
        RegisterMaster              =(mts_c)    GetProcAddress(handle,"MTS_RegisterMaster");
        DeregisterMaster            =(mts_void) GetProcAddress(handle,"MTS_DeregisterMaster");
        HasMaster                   =(mts_bool) GetProcAddress(handle,"MTS_HasMaster");
        GetNumClients               =(mts_int)  GetProcAddress(handle,"MTS_GetNumClients");
        SetNoteTunings              =(mts_pcd)  GetProcAddress(handle,"MTS_SetNoteTunings");
        SetNoteTuning               =(mts_dc)   GetProcAddress(handle,"MTS_SetNoteTuning");
        SetScaleName                =(mts_pcc)  GetProcAddress(handle,"MTS_SetScaleName");
        FilterNote                  =(mts_bcc)  GetProcAddress(handle,"MTS_FilterNote");
        ClearNoteFilter             =(mts_void) GetProcAddress(handle,"MTS_ClearNoteFilter");
        SetMultiChannel             =(mts_bc)   GetProcAddress(handle,"MTS_SetMultiChannel");
        SetMultiChannelNoteTunings  =(mts_pcdc) GetProcAddress(handle,"MTS_SetMultiChannelNoteTunings");
        SetMultiChannelNoteTuning   =(mts_dcc)  GetProcAddress(handle,"MTS_SetMultiChannelNoteTuning");
        FilterNoteMultiChannel      =(mts_bcc)  GetProcAddress(handle,"MTS_FilterNoteMultiChannel");
        ClearNoteFilterMultiChannel =(mts_char) GetProcAddress(handle,"MTS_ClearNoteFilterMultiChannel");
	}
	~mtsmasterglobal() {if (handle) FreeLibrary(handle);}
	HINSTANCE handle;
#else
	void load_lib()
    {
        if (!(handle=dlopen("/Library/Application Support/MTS-ESP/libMTS.dylib",RTLD_NOW)) &&
            !(handle=dlopen("/usr/local/lib/libMTS.so",RTLD_NOW))) return;
        RegisterMaster              =(mts_c)    dlsym(handle,"MTS_RegisterMaster");
        DeregisterMaster            =(mts_void) dlsym(handle,"MTS_DeregisterMaster");
        HasMaster                   =(mts_bool) dlsym(handle,"MTS_HasMaster");
        GetNumClients               =(mts_int)  dlsym(handle,"MTS_GetNumClients");
        SetNoteTunings              =(mts_pcd)  dlsym(handle,"MTS_SetNoteTunings");
        SetNoteTuning               =(mts_dc)   dlsym(handle,"MTS_SetNoteTuning");
        SetScaleName                =(mts_pcc)  dlsym(handle,"MTS_SetScaleName");
        FilterNote                  =(mts_bcc)  dlsym(handle,"MTS_FilterNote");
        ClearNoteFilter             =(mts_void) dlsym(handle,"MTS_ClearNoteFilter");
		SetMultiChannel             =(mts_bc)   dlsym(handle,"MTS_SetMultiChannel");
        SetMultiChannelNoteTunings  =(mts_pcdc) dlsym(handle,"MTS_SetMultiChannelNoteTunings");
        SetMultiChannelNoteTuning   =(mts_dcc)  dlsym(handle,"MTS_SetMultiChannelNoteTuning");
        FilterNoteMultiChannel      =(mts_bcc)  dlsym(handle,"MTS_FilterNoteMultiChannel");
        ClearNoteFilterMultiChannel =(mts_char) dlsym(handle,"MTS_ClearNoteFilterMultiChannel");
	}
	~mtsmasterglobal() {if (handle) dlclose(handle);}
	void *handle;
#endif
};

static mtsmasterglobal global;

void MTS_RegisterMaster()                                                       {if (global.RegisterMaster) global.RegisterMaster(0);}
void MTS_DeregisterMaster()                                                     {if (global.DeregisterMaster) global.DeregisterMaster();}
bool MTS_HasMaster()                                                            {return global.HasMaster?global.HasMaster():false;}
int  MTS_GetNumClients()				                                        {return global.GetNumClients?global.GetNumClients():0;}
void MTS_SetNoteTunings(const double *freqs)                                    {if (global.SetNoteTunings) global.SetNoteTunings(freqs);}
void MTS_SetNoteTuning(double freq,char midinote)                               {if (global.SetNoteTuning) global.SetNoteTuning(freq,midinote);}
void MTS_SetScaleName(const char *name)                                         {if (global.SetScaleName) global.SetScaleName(name);}
void MTS_FilterNote(bool doFilter,char midinote,char midichannel)               {if (global.FilterNote) global.FilterNote(doFilter,midinote,midichannel);}
void MTS_ClearNoteFilter()                                                      {if (global.ClearNoteFilter) global.ClearNoteFilter();}
void MTS_SetMultiChannel(bool set,char midichannel)                             {if (global.SetMultiChannel) global.SetMultiChannel(set,midichannel);}
void MTS_SetMultiChannelNoteTunings(const double *freqs,char midichannel)       {if (global.SetMultiChannelNoteTunings) global.SetMultiChannelNoteTunings(freqs,midichannel);}
void MTS_SetMultiChannelNoteTuning(double freq,char midinote,char midichannel)  {if (global.SetMultiChannelNoteTuning) global.SetMultiChannelNoteTuning(freq,midinote,midichannel);}
void MTS_FilterNoteMultiChannel(bool doFilter,char midinote,char midichannel)   {if (global.FilterNoteMultiChannel) global.FilterNoteMultiChannel(doFilter,midinote,midichannel);}
void MTS_ClearNoteFilterMultiChannel(char midichannel)                          {if (global.ClearNoteFilterMultiChannel) global.ClearNoteFilterMultiChannel(midichannel);}


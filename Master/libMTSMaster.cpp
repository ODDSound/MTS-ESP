/*
Copyright (C) 2021 by ODDSound Ltd. info@oddsound.com

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.
*/

#include "libMTSMaster.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__TOS_WIN__) || defined(_MSC_VER)
#define MTS_ESP_WIN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
typedef HRESULT (WINAPI* SHGetKnownFolderPathFunc) (const GUID*,DWORD,HANDLE,PWSTR*);
typedef void (WINAPI* CoTaskMemFreeFunc) (LPVOID);
#else
#include <dlfcn.h>
#endif

typedef void (*mts_c)(void*);
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
	mtsmasterglobal() : RegisterMaster(0), DeregisterMaster(0), Reinitialize(0), HasMaster(0), HasIPC(0), GetNumClients(0), SetNoteTunings(0), SetNoteTuning(0), SetScaleName(0), FilterNote(0), ClearNoteFilter(0), SetMultiChannel(0), SetMultiChannelNoteTunings(0), SetMultiChannelNoteTuning(0), FilterNoteMultiChannel(0), ClearNoteFilterMultiChannel(0), handle(0)
    {
		load_lib();
	}
    mts_c RegisterMaster;mts_void DeregisterMaster,Reinitialize;mts_bool HasMaster,HasIPC;mts_int GetNumClients;mts_pcd SetNoteTunings;mts_dc SetNoteTuning;mts_pcc SetScaleName;mts_bcc FilterNote;mts_void ClearNoteFilter;mts_bc SetMultiChannel;mts_pcdc SetMultiChannelNoteTunings;mts_dcc SetMultiChannelNoteTuning;mts_bcc FilterNoteMultiChannel;mts_char ClearNoteFilterMultiChannel;
	
#ifdef MTS_ESP_WIN
	void load_lib()
    {
        SHGetKnownFolderPathFunc SHGetKnownFolderPath=0;
        CoTaskMemFreeFunc CoTaskMemFree=0;
        HMODULE shell32Module=GetModuleHandleW(L"Shell32.dll");
        HMODULE ole32Module=GetModuleHandleW(L"Ole32.dll");
        if (shell32Module) SHGetKnownFolderPath=(SHGetKnownFolderPathFunc)GetProcAddress(shell32Module,"SHGetKnownFolderPath");
        if (ole32Module) CoTaskMemFree=(CoTaskMemFreeFunc)GetProcAddress(ole32Module,"CoTaskMemFree");
        if (SHGetKnownFolderPath && CoTaskMemFree)
        {
            const GUID FOLDERID_ProgramFilesCommonGUID={0xF7F1ED05,0x9F6D,0x47A2,0xAA,0xAE,0x29,0xD3,0x17,0xC6,0xF0,0x66};
            PWSTR cf=NULL;
            if (SHGetKnownFolderPath(&FOLDERID_ProgramFilesCommonGUID,0,0,&cf)>=0)
            {
                WCHAR buffer[MAX_PATH];buffer[0]=L'\0';
                if (cf) wcsncpy(buffer,cf,MAX_PATH);
                CoTaskMemFree(cf);
                buffer[MAX_PATH-1]=L'\0';
                const WCHAR *libpath=L"\\MTS-ESP\\LIBMTS.dll";
                DWORD cfLen=wcslen(buffer);
                wcsncat(buffer,libpath,MAX_PATH-cfLen-1);
                if (!(handle=LoadLibraryW(buffer))) return;
            }
            else {CoTaskMemFree(cf);return;}
        }
        else return;
        RegisterMaster              =(mts_c)    GetProcAddress(handle,"MTS_RegisterMaster");
        DeregisterMaster            =(mts_void) GetProcAddress(handle,"MTS_DeregisterMaster");
        HasMaster                   =(mts_bool) GetProcAddress(handle,"MTS_HasMaster");
        HasIPC                      =(mts_bool) GetProcAddress(handle,"MTS_HasIPC");
        Reinitialize                =(mts_void) GetProcAddress(handle,"MTS_Reinitialize");
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
        HasIPC                      =(mts_bool) dlsym(handle,"MTS_HasIPC");
        Reinitialize                =(mts_void) dlsym(handle,"MTS_Reinitialize");
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
bool MTS_CanRegisterMaster()                                                    {return global.HasMaster?!global.HasMaster():true;}
bool MTS_HasIPC()                                                               {return global.HasIPC?global.HasIPC():false;}
void MTS_Reinitialize()                                                         {if (global.Reinitialize) global.Reinitialize();}
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


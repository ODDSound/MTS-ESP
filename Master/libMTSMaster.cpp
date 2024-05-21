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
typedef HRESULT (WINAPI* SHGetKnownFolderPathFunc) (const GUID*, DWORD, HANDLE, PWSTR*);
typedef void (WINAPI* CoTaskMemFreeFunc) (LPVOID);
#else
#include <dlfcn.h>
#endif

typedef void (*mts_void__pVoid)(void*);
typedef void (*mts_void__void)(void);
typedef bool (*mts_bool__void)(void);
typedef int (*mts_int__void)(void);
typedef void (*mts_void__pConstDouble)(const double*);
typedef void (*mts_void__double_char)(double, char);
typedef void (*mts_void__pConstChar)(const char*);
typedef void (*mts_void__bool_char_char)(bool, char, char);
typedef void (*mts_void__bool_char)(bool, char);
typedef void (*mts_void__pConstDouble_char)(const double*, char);
typedef void (*mts_void__double_char_char)(double, char, char);
typedef void (*mts_void__char)(char);

struct mtsmasterglobal
{
	mtsmasterglobal() 
    : RegisterMaster(0)
    , DeregisterMaster(0)
    , Reinitialize(0)
    , HasMaster(0)
    , HasIPC(0)
    , GetNumClients(0)
    , SetNoteTunings(0)
    , SetNoteTuning(0)
    , SetScaleName(0)
    , FilterNote(0)
    , ClearNoteFilter(0)
    , SetMultiChannel(0)
    , SetMultiChannelNoteTunings(0)
    , SetMultiChannelNoteTuning(0)
    , FilterNoteMultiChannel(0)
    , ClearNoteFilterMultiChannel(0)
    , handle(0)
    {
		load_lib();
	}
    
    mts_void__pVoid RegisterMaster;
    mts_void__void DeregisterMaster;
    mts_void__void Reinitialize;
    mts_bool__void HasMaster;
    mts_bool__void HasIPC;
    mts_int__void GetNumClients;
    mts_void__pConstDouble SetNoteTunings;
    mts_void__double_char SetNoteTuning;
    mts_void__pConstChar SetScaleName;
    mts_void__bool_char_char FilterNote;
    mts_void__void ClearNoteFilter;
    mts_void__bool_char SetMultiChannel;
    mts_void__pConstDouble_char SetMultiChannelNoteTunings;
    mts_void__double_char_char SetMultiChannelNoteTuning;
    mts_void__bool_char_char FilterNoteMultiChannel;
    mts_void__char ClearNoteFilterMultiChannel;
	
#ifdef MTS_ESP_WIN
	void load_lib()
    {
        SHGetKnownFolderPathFunc SHGetKnownFolderPath = 0;
        CoTaskMemFreeFunc CoTaskMemFree = 0;
        
        HMODULE shell32Module = GetModuleHandleW(L"Shell32.dll");
        HMODULE ole32Module = GetModuleHandleW(L"Ole32.dll");
        
        if (shell32Module)
            SHGetKnownFolderPath = (SHGetKnownFolderPathFunc)GetProcAddress(shell32Module, "SHGetKnownFolderPath");
       
        if (ole32Module)
            CoTaskMemFree = (CoTaskMemFreeFunc)GetProcAddress(ole32Module, "CoTaskMemFree");
        
        if (SHGetKnownFolderPath && CoTaskMemFree)
        {
            const GUID FOLDERID_ProgramFilesCommonGUID = {0xF7F1ED05, 0x9F6D, 0x47A2, 0xAA, 0xAE, 0x29, 0xD3, 0x17, 0xC6, 0xF0, 0x66};
            PWSTR cf = NULL;
            if (SHGetKnownFolderPath(&FOLDERID_ProgramFilesCommonGUID, 0, 0, &cf) >= 0)
            {
                WCHAR buffer[MAX_PATH];
                buffer[0] = L'\0';
                if (cf)
                    wcsncpy(buffer, cf, MAX_PATH);
                CoTaskMemFree(cf);
                buffer[MAX_PATH - 1] = L'\0';
                const WCHAR *libpath = L"\\MTS-ESP\\LIBMTS.dll";
                DWORD cfLen = wcslen(buffer);
                wcsncat(buffer, libpath, MAX_PATH - cfLen - 1);
                handle = LoadLibraryW(buffer);
                if (!handle)
                    return;
            }
            else 
            {
                CoTaskMemFree(cf);
                return;
            }
        }
        else 
        {
            return;
        }
        
        RegisterMaster              = (mts_void__pVoid)              GetProcAddress(handle, "MTS_RegisterMaster");
        DeregisterMaster            = (mts_void__void)               GetProcAddress(handle, "MTS_DeregisterMaster");
        HasMaster                   = (mts_bool__void)               GetProcAddress(handle, "MTS_HasMaster");
        HasIPC                      = (mts_bool__void)               GetProcAddress(handle, "MTS_HasIPC");
        Reinitialize                = (mts_void__void)               GetProcAddress(handle, "MTS_Reinitialize");
        GetNumClients               = (mts_int__void)                GetProcAddress(handle, "MTS_GetNumClients");
        SetNoteTunings              = (mts_void__pConstDouble)       GetProcAddress(handle, "MTS_SetNoteTunings");
        SetNoteTuning               = (mts_void__double_char)        GetProcAddress(handle, "MTS_SetNoteTuning");
        SetScaleName                = (mts_void__pConstChar)         GetProcAddress(handle, "MTS_SetScaleName");
        FilterNote                  = (mts_void__bool_char_char)     GetProcAddress(handle, "MTS_FilterNote");
        ClearNoteFilter             = (mts_void__void)               GetProcAddress(handle, "MTS_ClearNoteFilter");
        SetMultiChannel             = (mts_void__bool_char)          GetProcAddress(handle, "MTS_SetMultiChannel");
        SetMultiChannelNoteTunings  = (mts_void__pConstDouble_char)  GetProcAddress(handle, "MTS_SetMultiChannelNoteTunings");
        SetMultiChannelNoteTuning   = (mts_void__double_char_char)   GetProcAddress(handle, "MTS_SetMultiChannelNoteTuning");
        FilterNoteMultiChannel      = (mts_void__bool_char_char)     GetProcAddress(handle, "MTS_FilterNoteMultiChannel");
        ClearNoteFilterMultiChannel = (mts_void__char)               GetProcAddress(handle, "MTS_ClearNoteFilterMultiChannel");
	}
    
	~mtsmasterglobal()
    {
        if (handle)
            FreeLibrary(handle);
    }
    
	HINSTANCE handle;
#else
	void load_lib()
    {
        if (!(handle = dlopen("/Library/Application Support/MTS-ESP/libMTS.dylib", RTLD_NOW)) &&
            !(handle = dlopen("/usr/local/lib/libMTS.so", RTLD_NOW)))
        {
            return;
        }
        
        RegisterMaster              = (mts_void__pVoid)             dlsym(handle, "MTS_RegisterMaster");
        DeregisterMaster            = (mts_void__void)              dlsym(handle, "MTS_DeregisterMaster");
        HasMaster                   = (mts_bool__void)              dlsym(handle, "MTS_HasMaster");
        HasIPC                      = (mts_bool__void)              dlsym(handle, "MTS_HasIPC");
        Reinitialize                = (mts_void__void)              dlsym(handle, "MTS_Reinitialize");
        GetNumClients               = (mts_int__void)               dlsym(handle, "MTS_GetNumClients");
        SetNoteTunings              = (mts_void__pConstDouble)      dlsym(handle, "MTS_SetNoteTunings");
        SetNoteTuning               = (mts_void__double_char)       dlsym(handle, "MTS_SetNoteTuning");
        SetScaleName                = (mts_void__pConstChar)        dlsym(handle, "MTS_SetScaleName");
        FilterNote                  = (mts_void__bool_char_char)    dlsym(handle, "MTS_FilterNote");
        ClearNoteFilter             = (mts_void__void)              dlsym(handle, "MTS_ClearNoteFilter");
		SetMultiChannel             = (mts_void__bool_char)         dlsym(handle, "MTS_SetMultiChannel");
        SetMultiChannelNoteTunings  = (mts_void__pConstDouble_char) dlsym(handle, "MTS_SetMultiChannelNoteTunings");
        SetMultiChannelNoteTuning   = (mts_void__double_char_char)  dlsym(handle, "MTS_SetMultiChannelNoteTuning");
        FilterNoteMultiChannel      = (mts_void__bool_char_char)    dlsym(handle, "MTS_FilterNoteMultiChannel");
        ClearNoteFilterMultiChannel = (mts_void__char)              dlsym(handle, "MTS_ClearNoteFilterMultiChannel");
	}
    
	~mtsmasterglobal() 
    {
        if (handle)
            dlclose(handle);
    }
    
	void *handle;
#endif
};

static mtsmasterglobal global;

void MTS_RegisterMaster()                                                           {if (global.RegisterMaster) global.RegisterMaster(0);}
void MTS_DeregisterMaster()                                                         {if (global.DeregisterMaster) global.DeregisterMaster();}
bool MTS_CanRegisterMaster()                                                        {return global.HasMaster ? !global.HasMaster() : true;}
bool MTS_HasIPC()                                                                   {return global.HasIPC ? global.HasIPC() : false;}
void MTS_Reinitialize()                                                             {if (global.Reinitialize) global.Reinitialize();}
int  MTS_GetNumClients()				                                            {return global.GetNumClients ? global.GetNumClients() : 0;}
void MTS_SetNoteTunings(const double *freqs)                                        {if (global.SetNoteTunings) global.SetNoteTunings(freqs);}
void MTS_SetNoteTuning(double freq, char midinote)                                  {if (global.SetNoteTuning) global.SetNoteTuning(freq, midinote);}
void MTS_SetScaleName(const char *name)                                             {if (global.SetScaleName) global.SetScaleName(name);}
void MTS_FilterNote(bool doFilter, char midinote, char midichannel)                 {if (global.FilterNote) global.FilterNote(doFilter, midinote, midichannel);}
void MTS_ClearNoteFilter()                                                          {if (global.ClearNoteFilter) global.ClearNoteFilter();}
void MTS_SetMultiChannel(bool set, char midichannel)                                {if (global.SetMultiChannel) global.SetMultiChannel(set, midichannel);}
void MTS_SetMultiChannelNoteTunings(const double *freqs, char midichannel)          {if (global.SetMultiChannelNoteTunings) global.SetMultiChannelNoteTunings(freqs, midichannel);}
void MTS_SetMultiChannelNoteTuning(double freq, char midinote, char midichannel)    {if (global.SetMultiChannelNoteTuning) global.SetMultiChannelNoteTuning(freq, midinote, midichannel);}
void MTS_FilterNoteMultiChannel(bool doFilter, char midinote, char midichannel)     {if (global.FilterNoteMultiChannel) global.FilterNoteMultiChannel(doFilter, midinote, midichannel);}
void MTS_ClearNoteFilterMultiChannel(char midichannel)                              {if (global.ClearNoteFilterMultiChannel) global.ClearNoteFilterMultiChannel(midichannel);}

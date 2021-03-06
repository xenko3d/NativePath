﻿/*
Copyright (c) 2016 Giovanni Petrantoni

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

//
//  NativeDynamicLinking.c
//  NativePath
//
//  Created by Giovanni Petrantoni on 03/17/16.
//  Copyright © 2016 Giovanni Petrantoni. All rights reserved.
//

#ifdef _WIN32
   #define NATIVE_PATH_WIN
   //define something for Windows (32-bit and 64-bit, this part is common)
   #include <windows.h>
   #include <stdio.h>
   
   #if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
   // This code is for Win32 desktop apps
   #define NATIVE_PATH_WIN_DESKTOP
   #else
   // This code is for WinRT Windows Store apps
   #define NATIVE_PATH_WIN_APP
   #endif

   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #endif
#elif __APPLE__
    #include "TargetConditionals.h"
    #include <dlfcn.h>
    #include <stdio.h>
    #if TARGET_IPHONE_SIMULATOR
    #define NATIVE_PATH_IOS
         // iOS Simulator
    #elif TARGET_OS_IPHONE
    #define NATIVE_PATH_IOS
        // iOS device
    #elif TARGET_OS_MAC
        // Other kinds of Mac OS
    #define NATIVE_PATH_MACOS
    #else
    #   error "Unknown Apple platform"
    #endif
#elif __linux__
	#define NATIVE_PATH_LINUX
    // linux
	#include <stdlib.h>
	#include <stdio.h>
	#include <dlfcn.h>	
#elif __unix__ // all unices not caught above
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#elif defined(__ANDROID__)
	#define NATIVE_PATH_ANDROID
	#include <stdlib.h>
	#include <stdio.h>
	#include <dlfcn.h>
#else
#   error "Unknown compiler"
#endif

void* LoadDynamicLibrary(const char* libraryPath)
{
	char nameBuffer[2048];
		
#if defined(NATIVE_PATH_LINUX) || defined(NATIVE_PATH_ANDROID)
    if(libraryPath) 
    {
        void *lib = NULL;
        sprintf(nameBuffer, "%s", libraryPath);
        lib = dlopen(nameBuffer, RTLD_NOW);
        if (lib) return lib;

        sprintf(nameBuffer, "%s.so", libraryPath);
        return dlopen(nameBuffer, RTLD_NOW);
    }
    return dlopen(NULL, RTLD_NOW);
#endif

#ifdef NATIVE_PATH_WIN_DESKTOP
    if(libraryPath) 
    {
        sprintf_s(nameBuffer, 2048, "%s.dll", libraryPath);
    }
	return LoadLibraryA(libraryPath ? nameBuffer : NULL);
#endif

#ifdef NATIVE_PATH_WIN_APP
    if(libraryPath) 
    {
        sprintf_s(nameBuffer, 2048, "%s.dll", libraryPath);
        wchar_t wString[2048];
        MultiByteToWideChar(CP_ACP, 0, nameBuffer, -1, wString, 2048);
        return LoadPackagedLibrary(wString, 0);
    }
	return NULL;
#endif

#if defined(NATIVE_PATH_IOS) || defined(NATIVE_PATH_MACOS)
    if(libraryPath)
    {
        //dylib case
        sprintf(nameBuffer, "%s.dylib", libraryPath);
        void* lib = dlopen(nameBuffer, RTLD_NOW);
        if(lib) return lib;
        //framework case
        sprintf(nameBuffer, "%s", libraryPath);
        lib = dlopen(nameBuffer, RTLD_NOW);
        if(lib) return lib;
        else return NULL;
    }
    //return the executable itself
    return dlopen(NULL, RTLD_NOW);   
#endif
	
	return 0;
}

void FreeDynamicLibrary(void* handle)
{
#if defined(NATIVE_PATH_LINUX) || defined(NATIVE_PATH_ANDROID) || defined(NATIVE_PATH_IOS) || defined(NATIVE_PATH_MACOS)
	dlclose(handle);
#endif
	
#ifdef NATIVE_PATH_WIN
	FreeLibrary(handle);
#endif
}

void* GetSymbolAddress(void* handle, const char* symbolName)
{
#if defined(NATIVE_PATH_LINUX) || defined(NATIVE_PATH_ANDROID) || defined(NATIVE_PATH_IOS) || defined(NATIVE_PATH_MACOS)
    return dlsym(handle, symbolName);
#endif
	
#ifdef NATIVE_PATH_WIN
	return GetProcAddress(handle, symbolName);
#endif
	
	return 0;
}

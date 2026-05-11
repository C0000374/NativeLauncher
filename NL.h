//
// Copyright (C) C0000374
//

#ifndef __NL_h__
#define __NL_h__
#include <NTExp.h>

#pragma warning(push)
#pragma warning(disable:4005)

#define CDECL		__cdecl
#define STDCALL		__stdcall
#define NORETURN	__declspec(noreturn) VOID

VOID
STDCALL
NluInit(
    VOID
    );

NORETURN
CDECL
NlupFatalImpl(
    IN PCSTR File OPTIONAL,
    IN PCSTR Function OPTIONAL,
    IN UINT32 Line,
    IN PCWSTR Format,
    IN ...
    );

#define FATAL(Format, ...) NlupFatalImpl(__FILE__, __func__, __LINE__, Format, __VA_ARGS__)

PVOID
STDCALL
NluAllocateMemory(
    IN UINT32 Flags,
    IN UINT32 Size
    );

PVOID
STDCALL
NluReAllocateMemory(
    IN UINT32 Flags,
    IN PVOID Block,
    IN UINT32 Size
    );

VOID
STDCALL
NluFreeMemory(
    IN PVOID Block
    );

#pragma warning(pop)
#endif
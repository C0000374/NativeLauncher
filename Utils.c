//
// Copyright (C) C0000374
//

#include <NL.h>
#define _CRT_SECURE_NO_WARNINGS
#define _NO_CRT_STDIO_INLINE
#include <StdIO.h>

#define FE_BUFFER_SIZE  2048

HANDLE NlupProcessHeap;
PWSTR NlupFatalErrorFormatBuffer1;
PWSTR NlupFatalErrorFormatBuffer2;

VOID
STDCALL
NluInit(
    VOID
    )
{
    NlupProcessHeap = RtlProcessHeap();
    NlupFatalErrorFormatBuffer1 = RtlAllocateHeap(NlupProcessHeap, 0, FE_BUFFER_SIZE * sizeof(WCHAR));
    NlupFatalErrorFormatBuffer2 = RtlAllocateHeap(NlupProcessHeap, 0, FE_BUFFER_SIZE * sizeof(WCHAR));
}

NORETURN
CDECL
NlupFatalImpl(
    IN PCSTR File OPTIONAL,
    IN PCSTR Function OPTIONAL,
    IN UINT32 Line,
    IN PCWSTR Format,
    IN ...
    )
{
    va_list ArgList;

    va_start(ArgList, Format);
    _vsnwprintf(NlupFatalErrorFormatBuffer1, FE_BUFFER_SIZE, Format, ArgList);
    NlupFatalErrorFormatBuffer1[FE_BUFFER_SIZE - 1] = 0;

    if (File == NULL && Function == NULL) {

        _snwprintf(
            NlupFatalErrorFormatBuffer2,
            FE_BUFFER_SIZE,
            L"FATAL:\r\n%s",
            NlupFatalErrorFormatBuffer1
        );
    }
    else {

        if (File == NULL) File = "???.c";
        if (Function == NULL) Function = "???";

        _snwprintf(
            NlupFatalErrorFormatBuffer2,
            FE_BUFFER_SIZE,
            L"FATAL:\r\nФункция: WinCraft.exe!%S\r\nСтрока: %S!%u\r\n%s",
            Function,
            File,
            Line,
            NlupFatalErrorFormatBuffer1
        );
    }

    NlupFatalErrorFormatBuffer2[FE_BUFFER_SIZE - 1] = 0;

    MessageBoxW(
        GetFocus(),
        NlupFatalErrorFormatBuffer2,
        L"WinCraft",
        MB_OK | MB_ICONERROR
    );

    RtlExitUserProcess(STATUS_UNSUCCESSFUL);
}

PVOID
STDCALL
NluAllocateMemory(
    IN UINT32 Flags,
    IN UINT32 Size
    )
{
    PVOID Result;

    Result = RtlAllocateHeap(NlupProcessHeap, Flags, Size);
    if (Result == NULL) FATAL(L"Не удалось выделить 0x%08X байт в куче", Size);
    return Result;
}

PVOID
STDCALL
NluReAllocateMemory(
    IN UINT32 Flags,
    IN PVOID Block,
    IN UINT32 Size
    )
{
    PVOID Result;

    Result = RtlReAllocateHeap(NlupProcessHeap, Flags, Block, Size);
    if (Result == NULL) FATAL(L"Не удалось выделить 0x%08X байт в куче", Size);
    return Result;
}

VOID
STDCALL
NluFreeMemory(
    IN PVOID Block
    )
{
    RtlFreeHeap(NlupProcessHeap, 0, Block);
}
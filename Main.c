//
// Copyright (C) C0000374
//

#include <NL.h>

VOID
CDECL
WinMainCRTStartup(
    VOID
    )
{
    PWSTR* CmdLine;
    UINT32 ArgsCount;
    PWSTR Buffer;
    UINT32 u32Status;
    BOOLEAN bStatus;
    UNICODE_STRING FileName;
    UNICODE_STRING CommandLine;
    NTSTATUS Status;
    PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
    RTL_USER_PROCESS_INFORMATION ProcessInformation;

    NluInit();

    CmdLine = CommandLineToArgvW(
        NtCurrentPeb()->ProcessParameters->CommandLine.Buffer,
        &ArgsCount
    );
    if (ArgsCount < 3) {

        MessageBoxW(
            GetFocus(),
            L"Использование:\r\nNativeLauncher.exe \"<Program>\" \"<Parameters>\"",
            L"NativeLauncher",
            MB_OK | MB_ICONINFORMATION
        );
        LocalFree(CmdLine);
        RtlExitUserProcess(STATUS_UNSUCCESSFUL);
    }

    Buffer = NluAllocateMemory(0, 2048);
    u32Status = RtlGetFullPathName_U(CmdLine[1], 2048, Buffer, NULL);
    if (u32Status == 0) FATAL(L"Не удалось запустить программу!");

    bStatus = RtlDosPathNameToNtPathName_U(Buffer, &FileName, NULL, NULL);
    if (!bStatus) FATAL(L"Не удалось запустить программу!");
    NluFreeMemory(Buffer);

    RtlInitUnicodeString(&CommandLine, CmdLine[2]);
    Status = RtlCreateProcessParametersEx(
        &ProcessParameters,
        &FileName,
        NULL,
        NULL,
        &CommandLine,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        RTL_USER_PROC_PARAMS_NORMALIZED
    );
    if (!NT_SUCCESS(Status)) FATAL(L"Не удалось запустить программу!\r\nNTSTATUS=0x%08X", Status);

    ProcessInformation.Length = sizeof(RTL_USER_PROCESS_INFORMATION);
    Status = RtlCreateUserProcess(
        &FileName,
        0,
        ProcessParameters,
        NULL,
        NULL,
        NULL,
        FALSE,
        NULL,
        NULL,
        &ProcessInformation
    );
    if (!NT_SUCCESS(Status)) FATAL(L"Не удалось запустить программу!\r\nNTSTATUS=0x%08X", Status);

    LocalFree(CmdLine);
    RtlDestroyProcessParameters(ProcessParameters);
    RtlFreeUnicodeString(&FileName);

    Status = NtResumeThread(ProcessInformation.Thread, NULL);
    if (!NT_SUCCESS(Status)) FATAL(L"Не удалось запустить программу!\r\nNTSTATUS=0x%08X", Status);

    NtClose(ProcessInformation.Process);
    NtClose(ProcessInformation.Thread);
    RtlExitUserProcess(STATUS_SUCCESS);
}
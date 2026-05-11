#include <NTExp.h>

#pragma warning(disable:4005)

#define CDECL	__cdecl

VOID
CDECL
TnEntryPoint(
    VOID
    )
{
    HANDLE hHeap;
    PWSTR Buffer;
    UINT32 u32Status;
    BOOLEAN bStatus;
    UNICODE_STRING FileName;
    OBJECT_ATTRIBUTES ObjectAttributes;
    NTSTATUS Status;
    HANDLE hFile;
    IO_STATUS_BLOCK IoStatusBlock;

    hHeap = RtlProcessHeap();
    
    Buffer = RtlAllocateHeap(hHeap, 0, 2048);
    if (Buffer == NULL) {
        DbgPrint("RtlAllocateHeap error");
        RtlExitUserProcess(STATUS_UNSUCCESSFUL);
    }

    u32Status = RtlGetFullPathName_U(L"Test.txt", 2048, Buffer, NULL);
    if (u32Status == 0) {
        DbgPrint("RtlGetFullPathName_U error");
        RtlExitUserProcess(STATUS_UNSUCCESSFUL);
    }

    bStatus = RtlDosPathNameToNtPathName_U(Buffer, &FileName, NULL, NULL);
    if (!bStatus) {
        DbgPrint("RtlDosPathNameToNtPathName_U error");
        RtlExitUserProcess(STATUS_UNSUCCESSFUL);
    }

    RtlFreeHeap(hHeap, 0, Buffer);

    InitializeObjectAttributes(
        &ObjectAttributes,
        &FileName,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
    );

    Status = NtCreateFile(
        &hFile,
        FILE_GENERIC_WRITE,
        &ObjectAttributes,
        &IoStatusBlock,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        FILE_SUPERSEDE,
        FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
        NULL,
        0
    );
    if (!NT_SUCCESS(Status)) {
        DbgPrint("NtCreateFile error 0x%08X", Status);
        RtlExitUserProcess(STATUS_UNSUCCESSFUL);
    }

    RtlFreeUnicodeString(&FileName);

    Status = NtWriteFile(
        hFile,
        NULL,
        NULL,
        NULL,
        &IoStatusBlock,
        L"\xFEFFHello, World!",
        28,
        NULL,
        NULL
    );
    if (!NT_SUCCESS(Status)) {
        DbgPrint("NtWriteFile error 0x%08X", Status);
        RtlExitUserProcess(STATUS_UNSUCCESSFUL);
    }

    NtClose(hFile);
    RtlExitUserProcess(STATUS_SUCCESS);
}
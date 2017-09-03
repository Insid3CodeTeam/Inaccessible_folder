#include <windows.h>
#include <ntdll.h>

#ifdef _WIN64
char *captionMsg = "64-bit Application";
#else
char *captionMsg = "32-bit Application";
#endif

char *statusMsg = "FAILED!";

#define MAIN_FOLDER L"\\??\\C:\\Winmend~Folder~Hidden"

wchar_t *folders[] = {
    MAIN_FOLDER,
    MAIN_FOLDER L"\\..." ,
    MAIN_FOLDER L"\\...\\cn"
};

void Report(NTSTATUS NtStatus, char *msg, wchar_t *path) {
    char buffer[256] = {0};

    if (NtStatus == 0)
        statusMsg = "SUCCESS";

    sprintf(buffer,
            "Task:\t%s\nPath:\t%S\nStatus:\t0x%X (%s)",
            msg,
            path,
            NtStatus,
            statusMsg);

    if (NtStatus == 0)
        MessageBoxA(NULL,
                    buffer,
                    captionMsg,
                    MB_ICONINFORMATION);
    else
        MessageBoxA(NULL,
                    buffer,
                    captionMsg,
                    MB_ICONERROR);
}

int main() {

    NTSTATUS NtStatus;
    HANDLE hTarget;
    UNICODE_STRING ObjectName;
    OBJECT_ATTRIBUTES ObjectAttributes;
    IO_STATUS_BLOCK IoStatusBlock;

    for (int x = 0; x < 3; x++) {
        RtlInitUnicodeString(&ObjectName, folders[x]);
        InitializeObjectAttributes(&ObjectAttributes,
                                   &ObjectName,
                                   OBJ_CASE_INSENSITIVE,
                                   NULL,
                                   NULL);

        NtStatus = NtCreateFile(&hTarget,
                                FILE_READ_DATA | FILE_WRITE_DATA,
                                &ObjectAttributes,
                                &IoStatusBlock,
                                NULL,
                                FILE_ATTRIBUTE_HIDDEN,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                FILE_CREATE,
                                FILE_DIRECTORY_FILE,
                                NULL,
                                0);

        Report(NtStatus, "Creating folder...", folders[x]);
        NtClose(hTarget);
    }

    for (int x = 2; x >= 0; x--) {
        RtlInitUnicodeString(&ObjectName, folders[x]);

        InitializeObjectAttributes(&ObjectAttributes,
                                   &ObjectName,
                                   OBJ_CASE_INSENSITIVE,
                                   NULL,
                                   NULL);

        NtStatus = NtDeleteFile(&ObjectAttributes);
        Report(NtStatus, "Deleting folder...", folders[x]);
    }

    return 0;
}

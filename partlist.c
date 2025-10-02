#define __USE_MINGW_ANSI_STDIO 0
#include <stdio.h>
#include <windows.h>

#ifdef DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...) do {} while (0)
#endif

#define MAX_PARTITION_ENTRIES_NUM 128
#define MAX_PARTITION_NAME_LEN 36 /* as found in PARTITION_INFORMATION_GPT */
#define DRIVE_LAYOUT_INFORMATION_EX_SIZE sizeof(DRIVE_LAYOUT_INFORMATION_EX) + (MAX_PARTITION_ENTRIES_NUM - 1)*sizeof(PARTITION_INFORMATION_EX)

typedef struct partitionList {
    unsigned amount;
    unsigned disk;
    char partitionNames[MAX_PARTITION_ENTRIES_NUM][MAX_PARTITION_NAME_LEN];
} partitionList;

unsigned enumeratePartitions(partitionList **list)
{
    unsigned arrayLength = 0, i, j;
    DWORD dwTemp;
    partitionList *partitionLists = NULL;
    DRIVE_LAYOUT_INFORMATION_EX *pDriveLayoutInformationEx = malloc(DRIVE_LAYOUT_INFORMATION_EX_SIZE);
    if (!pDriveLayoutInformationEx) {
        LOG("failed to allocate memory for drive layout information ex\n");
        puts("malloc failed");
        return 0;
    }
    for (i = 0; i < 256; i++) {
        char diskPath[21];
        HANDLE hDisk;
        sprintf(diskPath, "\\\\.\\PhysicalDrive%u", (unsigned char)i);
        LOG("opening %s (disk %u)\n", diskPath, i);
        hDisk = CreateFileA(diskPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
        if (hDisk == INVALID_HANDLE_VALUE) {
            DWORD error = GetLastError();
            LOG("failed to open disk, code %lu\n", error);
            if (error == ERROR_FILE_NOT_FOUND) {
                LOG("ran out of disks, breaking\n");
                break;
            }
            free(pDriveLayoutInformationEx);
            printf("failed to open disk %s. error %lu", diskPath, error);
            return 0;
        }
        if (!DeviceIoControl(hDisk, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, pDriveLayoutInformationEx, DRIVE_LAYOUT_INFORMATION_EX_SIZE, &dwTemp, NULL)) {
            LOG("failed to read layout, code %lu\n", GetLastError());
            free(pDriveLayoutInformationEx);
            CloseHandle(hDisk);
            printf("failed to read layout of %s", diskPath);
            return 0;
        }
        CloseHandle(hDisk);
        if (pDriveLayoutInformationEx->PartitionStyle != PARTITION_STYLE_GPT) {
            LOG("not gpt\n");
            continue;
        }
        ++arrayLength;
        partitionLists = realloc(partitionLists, sizeof(*partitionLists)*arrayLength);
        if (!partitionLists) {
            LOG("failed to realloc partitionLists, arrayLength was %u\n", arrayLength);
            free(pDriveLayoutInformationEx);
            puts("realloc failed");
            return 0;
        }
        partitionLists[i].disk = i;
        partitionLists[i].amount = (unsigned)pDriveLayoutInformationEx->PartitionCount;
        LOG("%u partitions on disk %u\n", partitionLists[i].amount, partitionLists[i].disk);
        for (j = 0; j < partitionLists[i].amount; j++) {
            wchar_t *wPartitionName = pDriveLayoutInformationEx->PartitionEntry[j].Gpt.Name;
            WideCharToMultiByte(CP_ACP, 0, wPartitionName, MAX_PARTITION_NAME_LEN, partitionLists[i].partitionNames[j], MAX_PARTITION_NAME_LEN, NULL, NULL);
            LOG("partition %u is %s\n", j, partitionLists[i].partitionNames[j]);
        }
    }
    free(pDriveLayoutInformationEx);
    *list = partitionLists;
    LOG("done enumerating partitions\n");
    return arrayLength;
}

int main(void)
{
    unsigned diskAmount, i, j;
    partitionList *partitionLists;
    diskAmount = enumeratePartitions(&partitionLists);  
    if (diskAmount == 0) {
        puts("failed to enumerate partitions");
        system("pause");
        return 1;
    }
    for (i = 0; i < diskAmount; ++i) {
        if (i > 'z' - 'a') {
            printf("disks after sdz will not be displayed.\n");
            free(partitionLists);
            system("pause");
            return 0;
        }
        for (j = 0; j < partitionLists[i].amount; ++j) {
            printf("sd%c%u %s\n", i + 'a', j + 1,partitionLists[i].partitionNames[j]);
        }
    }
    system("pause");
    return 0;
}

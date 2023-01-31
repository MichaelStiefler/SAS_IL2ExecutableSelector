#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996 )

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "../global/version.h"
#include <strsafe.h>

#define OUT_H "resource_files.h"
#define OUT_CPP "Output/resource_files.cpp"

void initOutputFiles() {
    FILE* fout_h = fopen(OUT_H, "wb"); // Always using '\n'
    fprintf(
        fout_h,
        "#pragma once\n"
        "#include <cstddef>\n"
        "\n");
    fclose(fout_h);
    FILE* fout = fopen(OUT_CPP, "wb");
    fprintf(
        fout,
        "#include \"%s\"\n"
        "\n"
        "#if defined(_MSC_VER)\n"
        "    #define BIN2ARRAY_ALIGN __declspec(align(8))\n"
        "#else\n"
        "    #define BIN2ARRAY_ALIGN __attribute__((__aligned__(8)))\n"
        "#endif\n"
        "\n",
        OUT_H);
    fclose(fout);
}

int writeData(const char* name, size_t size, unsigned char* text, unsigned char* out_ptr) {
    //
    // Writing output .h & .cpp files
    //
    FILE* fout_h = fopen(OUT_H, "ab"); // Always using '\n'
    fprintf(
        fout_h,
        "extern const size_t %s_size;\n"
        "extern const unsigned char %s[];\n",
        name,
        name);
    fclose(fout_h);
    FILE* fout = fopen(OUT_CPP, "ab");
    fprintf(
        fout,
        "const size_t %s_size = %ldu;\n"
        "\n"
        "const unsigned char BIN2ARRAY_ALIGN %s[%s_size] = {\n",
        name,
        size,
        name,
        name);
    if (fwrite(text, out_ptr - text, 1, fout) != 1) {
        printf("Error writing output file!");
        free(text);
        fclose(fout);
        return 2;
    }
    fprintf(fout, "};\n");
    fclose(fout);
    return 0;
}

int readData(const char* name, size_t* size, unsigned char** textRef, unsigned char** out_ptrRef) {

    //
    // Working with the input file
    //
    FILE* fin = fopen(name, "rb");
    if (!fin) {
        printf("Error opening input file!\n");
        return 2;
    }
    fseek(fin, 0, SEEK_END);
    *size = (size_t)ftell(fin);
    fseek(fin, 0, SEEK_SET);
    printf("Input data size: %ld\n", (long)*size);
    // Loading all input data into memory at once
    unsigned char* data = (unsigned char*)malloc(*size);
    if (fread(data, *size, 1, fin) != 1) {
        printf("Failed to read input file!\n");
        free(data);
        fclose(fin);
        return 2;
    }
    fclose(fin);

    //
    // Converting input data to C code
    //
    unsigned char* text = (unsigned char*)malloc(*size * 5);
    unsigned char* out_ptr = text;
    unsigned char* in_ptr = data;
    size_t saltPos = 0;
    size_t saltLen = strlen(G_FILE_VERSION);

    for (size_t i = 1, col = 1; i <= *size; i++, in_ptr++) {
        unsigned char x = *in_ptr;
        unsigned char salt = G_FILE_VERSION[saltPos++];
        if (saltPos >= saltLen) saltPos = 0;
        x ^= salt;

        // Converting unsigned char to code - much faster than itoa/printf/and etc.
        // Using decimal code because it's much more compact than hex code.
        if (x >= 10) {
            if (x >= 100) {
                *out_ptr++ = '0' + ((x / 100u) % 10u);
            }
            *out_ptr++ = '0' + ((x / 10u) % 10u);
        }
        *out_ptr++ = '0' + (x % 10u);
        if (i != *size) {
            *out_ptr++ = ',';
        }
        if (col == 20) {
            *out_ptr++ = '\n';
            col = 1;
        }
        else
            col++;
    }
    free(data);
    // *out_ptr = 0; not necessary as we're using fwrite()
    *textRef = text;
    *out_ptrRef = out_ptr;
    return out_ptr - text;
}

int main(int argc, char* argv[])
{
    initOutputFiles();
    WIN32_FIND_DATAA ffd;
    //LARGE_INTEGER filesize;
    CHAR cDir[MAX_PATH];
    CHAR cName[MAX_PATH];
    //size_t length_of_arg;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;


    // Find the first file in the directory.
    hFind = FindFirstFileA("Input/*", &ffd);
    if (INVALID_HANDLE_VALUE == hFind)
    {
        printf("Error opening input file!\n");
        return 2;
    }

    do
    {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            printf("  %s   <DIR>\n", ffd.cFileName);
        }
        else
        {
            StringCchPrintfA(cDir, sizeof(cDir), "%s/%s", "Input/" , ffd.cFileName);
            ZeroMemory(cName, sizeof(cName));
            strncpy(cName, ffd.cFileName, strrchr(ffd.cFileName, '.') - ffd.cFileName);

            size_t size;
            unsigned char* text = NULL;
            unsigned char* out_ptr = NULL;
            int readRetVal = readData(cDir, &size, &text, &out_ptr);
            int writeRetVal = writeData(cName, size, text, out_ptr);

            HANDLE hFile = CreateFileA(cDir, GENERIC_READ, FILE_SHARE_READ,
                NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            printf("  %s   %ld bytes, readRetVal=%d, writeRetVal=%d\n", ffd.cFileName, GetFileSize(hFile, NULL), readRetVal, writeRetVal);
            CloseHandle(hFile);
        }
    } while (FindNextFileA(hFind, &ffd) != 0);

    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES)
    {
        printf("Error opening input file!\n");
        return 2;
    }

    FindClose(hFind);
    return 0;
}


/*
Copyright (C) 2011-2016 OldTimes Software

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef _WIN32
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "PL/platform_filesystem.h"

/*	File System	*/

PLresult _plInitIO(void) {
    return PL_RESULT_SUCCESS;
}

void _plShutdownIO(void) {

}

// Checks whether a file has been modified or not.
bool plIsFileModified(time_t oldtime, const char *path) {
    if (!oldtime) {
        _plSetErrorMessage("Invalid time, skipping check!\n");
        return false;
    }

    struct stat attributes;
    if (stat(path, &attributes) == -1) {
        _plSetErrorMessage("Failed to get file stats!\n");
        return false;
    }

    if (attributes.st_mtime > oldtime) {
        return true;
    }

    return false;
}

time_t plGetFileModifiedTime(const PLchar *path) {
    struct stat attributes;
    if (stat(path, &attributes) == -1) {
        _plSetErrorMessage("Failed to get modification time!\n");
        return 0;
    }
    return attributes.st_mtime;
}

// todo, move into platform_string and make safer
void plLowerCasePath(char *out) {
    for (int i = 0; out[i]; i++) {
        out[i] = (PLchar) tolower(out[i]);
    }
}

// Creates a folder at the given path.
bool plCreateDirectory(const char *path) {
#ifdef _WIN32
    if(CreateDirectory(path, NULL) || (GetLastError() == ERROR_ALREADY_EXISTS))
        return true;
    else if(GetLastError() == ERROR_PATH_NOT_FOUND)
        plSetError("Failed to find an intermediate directory! (%s)\n", path);
    else    // Assume it already exists.
        plSetError("Unknown error! (%s)\n", path);
#else
    {
        struct stat buffer;
        if (stat(path, &buffer) == -1) {
            if (mkdir(path, 0777) == 0)
                return true;

            switch (errno) {
                case EACCES:
                    _plSetErrorMessage("Failed to get permission! (%s)\n", path);
                case EROFS:
                    _plSetErrorMessage("File system is read only! (%s)\n", path);
                case ENAMETOOLONG:
                    _plSetErrorMessage("Path is too long! (%s)\n", path);
                default:
                    _plSetErrorMessage("Failed to create directory! (%s)\n", path);
            }
        } else
            // Path already exists, so this is fine.
            return true;
    }
#endif

    return false;
}

// Returns the extension for the file.
const char *plGetFileExtension(const char *in) {
    if (!plIsValidString(in)) {
        return "";
    }

    const PLchar *s = strrchr(in, '.');
    if(!s || s == in) {
        return "";
    }

    return s + 1;
}

// Strips the extension from the filename.
void plStripExtension(char *dest, const char *in) {
    if (!plIsValidString(in)) {
        *dest = 0;
        return;
    }

    const char *s = strrchr(in, '.');
    while (in < s) *dest++ = *in++;
    *dest = 0;
}

// Returns a pointer to the last component in the given filename. 
const char *plGetFileName(const char *path) {
    const char *lslash = strrchr(path, '/');
    if (lslash != NULL) {
        path = lslash + 1;
    }
    return path;
}

// Returns the name of the systems current user.
void plGetUserName(char *out) {
#ifdef _WIN32
    PLchar userstring[PL_MAX_USERNAME];

    // Set these AFTER we update active function.
    DWORD name = sizeof(userstring);
    if (GetUserName(userstring, &name) == 0)
        // If it fails, just set it to user.
        sprintf(userstring, "user");
#else   // Linux
    char *userstring = getenv("LOGNAME");
    if (userstring == NULL) {
        // If it fails, just set it to user.
        userstring = "user";
    }
#endif
    int i = 0, userlength = (int) strlen(userstring);
    while (i < userlength) {
        if (userstring[i] == ' ') {
            out[i] = '_';
        } else {
            out[i] = (char) tolower(userstring[i]);
        } i++;
    }

    //strncpy(out, cUser, sizeof(out));
}

/*	Scans the given directory.
	On each found file it calls the given function to handle the file.
*/
void plScanDirectory(const char *path, const char *extension, void (*Function)(const char *), bool recursive) {
    char filestring[PL_SYSTEM_MAX_PATH];
#ifdef _WIN32
        WIN32_FIND_DATA	finddata;
        HANDLE			find;

        snprintf(filestring, sizeof(filestring), "%s/*", path);

        find = FindFirstFile(filestring, &finddata);
        if (find == INVALID_HANDLE_VALUE) {
            plSetError("Failed to find an initial file!\n");
            return;
        }

        do {
            if(strcmp(finddata.cFileName, ".") == 0 || strcmp(finddata.cFileName, "..") == 0) {
                continue;
            }

            snprintf(filestring, sizeof(filestring), "%s/%s", path, finddata.cFileName);

            if(finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if(recursive) {
                    plScanDirectory(filestring, extension, Function, recursive);
                }
            }
            else{
                if(pl_strcasecmp(plGetFileExtension(finddata.cFileName), extension) == 0) {
                    Function(filestring);
                }
            }
        } while(FindNextFile(find, &finddata));

        FindClose(find);
#else
    DIR *directory = opendir(path);
    if (directory) {
        struct dirent *entry;
        while ((entry = readdir(directory))) {
            if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            snprintf(filestring, sizeof(filestring), "%s/%s", path, entry->d_name);

            struct stat st;
            if(stat(filestring, &st) == 0) {
                if(S_ISREG(st.st_mode)) {
                    if(pl_strcasecmp(plGetFileExtension(entry->d_name), extension) == 0) {
                        Function(filestring);
                    }
                }
                else if(S_ISDIR(st.st_mode) && recursive) {
                    plScanDirectory(filestring, extension, Function, recursive);
                }
            }
        }

        closedir(directory);
    }
#endif
}

const char *plGetWorkingDirectory(void) {
    static char out[PL_SYSTEM_MAX_PATH] = { '\0' };
    if (getcwd(out, PL_SYSTEM_MAX_PATH) == NULL) {
        switch (errno) { // todo, fix cases for Windows
            default: break;

            case EACCES:
                _plSetErrorMessage("Permission to read or search a component of the filename was denied!\n");
                break;
            case EFAULT:
                _plSetErrorMessage("buf points to a bad address!\n");
                break;
            case EINVAL:
                _plSetErrorMessage("The size argument is zero and buf is not a null pointer!\n");
                break;
            case ENOMEM:
                _plSetErrorMessage("Out of memory!\n");
                break;
            case ENOENT:
                _plSetErrorMessage("The current working directory has been unlinked!\n");
                break;
            case ERANGE:
                _plSetErrorMessage("The size argument is less than the length of the absolute pathname of the working directory, including the terminating null byte. \
						You need to allocate a bigger array and try again!\n");
                break;
        }
        return NULL;
    }
    return out;
}

void plSetWorkingDirectory(const char *path) {
    if(chdir(path) != 0) {
        switch(errno) { // todo, fix cases for Windows
            default: break;

            case EACCES:
                _plSetErrorMessage("Search permission is denied for any component of pathname!\n");
                break;
            case ELOOP:
                _plSetErrorMessage(
                        "A loop exists in the symbolic links encountered during resolution of the path argument!\n");
                break;
            case ENAMETOOLONG:
                _plSetErrorMessage("The length of the path argument exceeds PATH_MAX or a pathname component is longer than \
                NAME_MAX!\n");
                break;
            case ENOENT:
                _plSetErrorMessage(
                        "A component of path does not name an existing directory or path is an empty string!\n");
                break;
            case ENOTDIR:
                _plSetErrorMessage("A component of the pathname is not a directory!\n");
                break;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////
// FILE I/O

// Checks if a file exists or not.
bool plFileExists(const char *path) {
    struct stat buffer;
    return (bool) (stat(path, &buffer) == 0);
}

bool plPathExists(const char *path) {
    DIR *dir = opendir(path);
    if(dir) {
        closedir(dir);
        return true;
    }
    return false;
}

// todo, return plresult instead
bool plCopyFile(const char *path, const char *dest) {
    FILE *fold = fopen(path, "rb");
    if(!fold) {
        _plReportError(PL_RESULT_FILEREAD, "Failed to open %s!", path);
        return false;
    }

    fseek(fold, 0, SEEK_END);
    size_t file_size = (size_t)ftell(fold);
    fseek(fold, 0, SEEK_SET);

    uint8_t *data = calloc(file_size, 1);
    if(!data) {
        fclose(fold);

        _plReportError(PL_RESULT_MEMORYALLOC, "Failed to allocate buffer for %s, with size %d!", path, file_size);
        return false;
    }

    fread(data, 1, file_size, fold);

    FILE *out = fopen(dest, "wb");
    if(!out || (fwrite(data, 1, file_size, out) != file_size)) {
        fclose(fold);

        _plReportError(PL_RESULT_FILEREAD, "Failed to write %s!", dest);
        return false;
    }
    fclose(out);

    return true;
}

size_t plGetFileSize(const char *path) {
    _plResetError();

    struct stat buf;
    if(stat(path, &buf) != 0) {
        _plReportError(PL_RESULT_FILEREAD, plGetSystemErrorString());
        return 0;
    }

    return (size_t)buf.st_size;
}

int16_t plGetLittleShort(FILE *fin) {
    int b1 = fgetc(fin);
    int b2 = fgetc(fin);
    return (int16_t) (b1 + b2 * 256);
}

int32_t plGetLittleLong(FILE *fin) {
    int b1 = fgetc(fin);
    int b2 = fgetc(fin);
    int b3 = fgetc(fin);
    int b4 = fgetc(fin);
    return b1 + (b2 << 8) + (b3 << 16) + (b4 << 24);
}

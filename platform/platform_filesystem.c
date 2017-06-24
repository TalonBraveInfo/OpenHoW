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

#include "PL/platform_filesystem.h"

/*	File System	*/

PLresult _plInitIO(void) {
    plFunctionStart();

    return PL_RESULT_SUCCESS;
}

void _plShutdownIO(void) {
    plFunctionStart();
}

// Checks whether a file has been modified or not.
bool plIsFileModified(time_t oldtime, const char *path) {
    plFunctionStart();
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
    plFunctionEnd();
}

time_t plGetFileModifiedTime(const PLchar *path) {
    plFunctionStart();
    struct stat attributes;
    if (stat(path, &attributes) == -1) {
        _plSetErrorMessage("Failed to get modification time!\n");
        return 0;
    }
    return attributes.st_mtime;
    plFunctionEnd();
}

// todo, move into platform_string and make safer
void plLowerCasePath(char *out) {
    plFunctionStart();
    for (int i = 0; out[i]; i++) {
        out[i] = (PLchar) tolower(out[i]);
    }
    plFunctionEnd();
}

// Creates a folder at the given path.
bool plCreateDirectory(const char *path) {
    plFunctionStart();
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
            else {
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
    plFunctionStart();
    if (!plIsValidString(in)) {
        return "";
    }

    const PLchar *s = strrchr(in, '.');
    if(!s || s == in) {
        return "";
    }

    return s + 1;
    plFunctionEnd();
}

// Strips the extension from the filename.
void plStripExtension(char *dest, const char *in) {
    plFunctionStart();
    if (!plIsValidString(in)) {
        *dest = 0;
        return;
    }

    const char *s = strrchr(in, '.');
    while (in < s) *dest++ = *in++;
    *dest = 0;
    plFunctionEnd();
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
    plFunctionStart();
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
    plFunctionEnd();
}

/*	Scans the given directory.
	On each found file it calls the given function to handle the file.
*/
void plScanDirectory(const char *path, const char *extension, void(*Function)(const char *filepath)) {
    plFunctionStart();

    char filestring[PL_SYSTEM_MAX_PATH];
#ifdef _WIN32
    {
        WIN32_FIND_DATA	finddata;
        HANDLE			find;

        sprintf(filestring, "%s/*%s", path, extension);

        find = FindFirstFile(filestring, &finddata);
        if (find == INVALID_HANDLE_VALUE)
        {
            plSetError("Failed to find an initial file!\n");
            return;
        }

        do
        {
            // Pass the entire dir + filename.
            sprintf(filestring, "%s/%s", path, finddata.cFileName);
            Function(filestring);
        } while(FindNextFile(find, &finddata));
    }
#else
    DIR *directory = opendir(path);
    if (directory) {
        struct dirent *entry;
        while ((entry = readdir(directory))) {
            if (strcasestr(entry->d_name, extension)) {
                sprintf(filestring, "%s/%s", path, entry->d_name);
                Function(filestring);
            }
        }

        closedir(directory);
    }
#endif
    plFunctionEnd();
}

void plGetWorkingDirectory(PLchar *out) {
    plFunctionStart();
    if (!getcwd(out, PL_SYSTEM_MAX_PATH)) {
        switch (errno) {
            default:
                break;

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
        return;
    }
    strcat(out, "\\");
}

void plSetWorkingDirectory(const char *path) {
    plFunctionStart();

    if(chdir(path) != 0) {
        switch(errno) {
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

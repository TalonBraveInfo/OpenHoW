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

#include "platform_library.h"

/*	Library management	*/

PL_FARPROC plFindLibraryFunction(PL_INSTANCE instance, const PLchar *function) {
    plFunctionStart();

    if (instance) {
        PL_FARPROC address;

#ifdef _WIN32
        address = GetProcAddress(instance, function);
#else   // Linux
        address = dlsym(instance, function);
#endif
        if (address)
            return address;
    }

    return (NULL);
}

// Frees library instance.
PLvoid _plFreeLibrary(PL_INSTANCE instance) {
    plFunctionStart();
#ifdef _WIN32
    FreeLibrary(instance);
#else   // Linux
    dlclose(instance);
#endif
    plFunctionEnd();
}

PLvoid plUnloadLibrary(PL_INSTANCE instance) {
    plFunctionStart();
    if (instance) {
        _plFreeLibrary(instance);

        // Set the instance to null.
        instance = NULL;
    }
}

/*	Function to allow direct loading of an external module.
*/
PL_INSTANCE plLoadLibrary(const PLchar *path) {
    plFunctionStart();

    PLchar newpath[PL_SYSTEM_MAX_PATH];
    sprintf(newpath, "%s"PL_SYSTEM_LIBRARY_EXTENSION, path);

    PL_INSTANCE instance =
#ifdef _WIN32
    LoadLibrary(newpath);
#else
    dlopen(newpath, RTLD_NOW);
#endif
    if (!instance) {
        plSetError("Failed to load module! (%s)\n%s\n", newpath, plGetSystemError());
        return NULL;
    }

    return instance;
}

/*	Generic interface to allow loading of an external module.
*/
PLvoid *plLoadLibraryInterface(PL_INSTANCE instance, const PLchar *path, const PLchar *entry, PLvoid *handle) {
    instance = plLoadLibrary(path);
    if (!instance)
        return NULL;

    PLvoid *(*EntryFunction)(PLvoid *) = plFindLibraryFunction(instance, entry);
    if (!EntryFunction) {
        plSetError("Failed to find entry function! (%s)\n", entry);
        return NULL;
    }

    return (EntryFunction(handle));
}

// file:   SystemCore.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.

#include "SystemCore.h"
#include <fcntl.h>
#include <signal.h>
#include <dirent.h>
#include <fstream>

pid_t system_async(const char * command, int * infp, int * outfp)
{
    int p_stdin[2];
    int p_stdout[2];
    pid_t pid;

    if (pipe(p_stdin) == -1)
        return -1;

    if (pipe(p_stdout) == -1) {
        close(p_stdin[0]);
        close(p_stdin[1]);
        return -1;
    }

    pid = fork();

    if (pid < 0) {
        close(p_stdin[0]);
        close(p_stdin[1]);
        close(p_stdout[0]);
        close(p_stdout[1]);
        return pid;
    } else if (pid == 0) {
        close(p_stdin[1]);
        dup2(p_stdin[0], 0);
        close(p_stdout[0]);
        dup2(p_stdout[1], 1);
        dup2(::open("/dev/null", O_RDONLY), 2);
        /// Close all other descriptors for the safety sake.
        for (int i = 3; i < 4096; ++i)
            ::close(i);

        setsid();
        execl("/bin/sh", "sh", "-c", command, NULL);
        _exit(1);
    }

    close(p_stdin[0]);
    close(p_stdout[1]);

    if (infp == NULL) {
        close(p_stdin[1]);
    } else {
        *infp = p_stdin[1];
    }

    if (outfp == NULL) {
        close(p_stdout[0]);
    } else {
        *outfp = p_stdout[0];
    }

    return pid;
}

int pidof( String proc_name)
{
    int pid = -1;

    DIR *dp = opendir( "/proc");
    if ( dp != NULL ) {
        struct dirent *dirp;
        while (  (pid < 0) && (dirp = readdir( dp)) ) {
            int id = atoi( dirp->d_name);
            if ( id <= 0 ) continue;
            string path = string( "/proc/") + dirp->d_name + "/cmdline";
            ifstream file( path.c_str());
            string line;
            getline( file, line);
            if ( line.empty() ) continue;
            size_t pos = line.find( '\0');
            if ( pos != string::npos )
                line = line.substr( 0, pos);
            pos = line.rfind( '/');
            if ( pos != string::npos )
                line = line.substr( pos + 1);
            if ( proc_name == line )
                pid = id;
        }
    }
    closedir( dp);
    return pid;
}

String make_linux_path( String path)
{
    String p;
    for ( int i = 0; i < path.length(); i++ ) {
        if ( path[i] == ' ' ) p += '\\';
        p += path[i];
    }
    return p;
}

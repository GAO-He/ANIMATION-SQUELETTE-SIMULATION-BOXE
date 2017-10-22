/*****************************************************************************
File: udp.h

Informatique Graphique
Master d'informatique
Christian Jacquemin, Universite Paris-Sud & LIMSI-CNRS

Copyright (C) 2014 University Paris-Sud
This file is provided without support, instruction, or implied
warranty of any kind.  University Paris 11 makes no guarantee of its
fitness for a particular purpose and is not liable under any
circumstances for any damages or loss whatsoever arising from the use
or inability to use this file or items derived from it.
******************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#else
//#define socklen_t int
#include <winsock2.h>
#include <Ws2tcpip.h>
//#include <wspiapi.h>
#endif

extern int SocketToLocalServer;

// INITIALIZATION
void initUDP( void );
// MESSAGE PROCESSING
void processUDPMessages( void );

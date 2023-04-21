#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <stdio.h>
#include <iostream>
#include <zmq.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <assert.h>
#include <map>

using namespace std;

#define CLIENT_PREFIX "tcp://localhost:"
#define SERVER_PREFIX "tcp://*:"

#define BASE_PORT 4000
#define STR_LEN 64

#define REQUEST_TIMEOUT 2000
typedef enum
{
    EXIT = 0,
    CREATE,
    REMOVE,
    EXEC,
    PRINT,
    PING,
    DEFAULT
} command_type;

string convert_adr_client(unsigned short port);
string convert_adr_server(unsigned short port);

const char *int_to_str(unsigned a);

command_type get_command();
string unitread();

#endif
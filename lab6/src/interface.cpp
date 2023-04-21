#include "interface.h"

string unitread() // считывание по словам через пробел
{
    string result = "";
    char cur;
    while ((cur = getchar()) != ' ')
    {
        if (cur == '\0' || cur == '\n')
        {
            break;
        }
        result += cur;
    }
    return result;
}

command_type get_command()
{
    string cmd = unitread();
    if (strcmp(cmd.c_str(), "print") == 0)
    {
        return PRINT;
    }
    if (strcmp(cmd.c_str(), "create") == 0)
    {
        return CREATE;
    }
    if (strcmp(cmd.c_str(), "exec") == 0)
    {
        return EXEC;
    }
    if (strcmp(cmd.c_str(), "exit") == 0)
    {
        return EXIT;
    }
    if (strcmp(cmd.c_str(), "remove") == 0)
    {
        return REMOVE;
    }
    if (strcmp(cmd.c_str(), "ping") == 0)
    {
        return PING;
    }
    return DEFAULT;
}

string convert_adr_client(unsigned short port)  // получает адрес из порта для клиента
{
    string port_string = int_to_str(port);
    string name = CLIENT_PREFIX + port_string;
    return name;
}

string convert_adr_server(unsigned short port)  // получает адрес из порта для сервера
{
    string port_string = int_to_str(port);
    string name = SERVER_PREFIX + port_string;
    return name;
}

const char *int_to_str(unsigned a)
{
    int num = a, i = 0;
    if (a == 0)
        return "0";
    while (num > 0)
    {
        num /= 10;
        i++;
    }
    char *result = (char *)calloc(sizeof(char), i + 1);
    while (i >= 1)
    {
        result[--i] = a % 10 + '0';
        a /= 10;
    }

    return result;
}

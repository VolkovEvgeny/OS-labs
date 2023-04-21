#include "config.h"

using namespace std;

#define CLIENT_ROOT_ID 15

int main(int argc, char const *argv[])
{
    std::cout << "Using client root " << CLIENT_ROOT_ID << " as default" << endl;
    std::cout << "\nWrite:\n command [arg1] ... [argn]\n";
    std::cout << "\ncreate [id] to create calculation node\n";
    std::cout << "\nremove [id] to remove calculation node\n";
    std::cout << "\nexec [id] [key:string] [value:int] to add [key:value] to map on calculation node\n";
    std::cout << "\nexec [id] [key:string] to show the value in map on calculation node\n";
    std::cout << "\nping [id] to show node reachible\n";
    std::cout << "\nprint to show NTree topology\n\n";
    Tree *system;   // топология лежит на клиенте
    system = createTree(CLIENT_ROOT_ID);
    void *context = zmq_ctx_new(); // create new context
    if (context == NULL)
    {
        throw runtime_error("Error: Can't initialize context");
    }

    void *socket_left = NULL;   // для отправки запроса на адрес левого (правого) узла
    void *socket_right = NULL;
    int ex = 0;
    while (true)
    {
        command_type cur_command = get_command(); // считываем команду из ввода
        string child_id_str;
        int child_id;           // id нового узла
        string remove_id_str;
        int remove_id;
        int exec_id;
        int ping_id;
        int save;
        string ping_id_str;
        string exec_id_str;
        int value;
        string key;
        char *reply = (char *)calloc(sizeof(char), 64);
        switch (cur_command)
        {
        case PRINT:
            printTree(system, 0);   
            break;
        case CREATE:
            child_id_str = unitread(); // считывание след слова до пробела
            child_id = atoi(child_id_str.c_str()); // получаем число из строки
            if (child_id <= 0)
            {
                cout << "Error: invalid id" << endl;
                break;
            }
            if (existNode(system, child_id))
            {
                cout << "Error: already exists" << endl;
                break;
            }
            system = createNode(system, child_id);
            if (child_id > CLIENT_ROOT_ID)
            {
                if (socket_right == NULL)   // если нет правого потомка
                {
                    int fork_pid = fork();  // создаем новый процесс
                    if (fork_pid == -1)
                    {
                        throw runtime_error("Error: fork problem occured");
                        break;
                    }
                    if (fork_pid == 0)
                    {
                        create_server_node(child_id); // запускаем из дочерний процесса программу ./server 
                    }

                    socket_right = zmq_socket(context, ZMQ_REQ); // create new request socket сперва отправляет, потом получает

                    cout << "OK: " << fork_pid << endl;
                    int opt = 0;
                    int rc = zmq_setsockopt(socket_right, ZMQ_LINGER, &opt, sizeof(opt)); // установка периода ожидания для сокета
                    assert(rc == 0);                                                      // пишет сообщение об ошибке
                    if (socket_right == NULL)
                    {
                        throw runtime_error("Error: socket not created");
                    }
                    rc = zmq_connect(socket_right, convert_adr_client(BASE_PORT + child_id).c_str()); // connect soket wieh endpoint - адресом ребенка
                    assert(rc == 0);
                    break;
                }
            }
            if (child_id < CLIENT_ROOT_ID)
            {
                if (socket_left == NULL)
                {
                    int fork_pid = fork();
                    if (fork_pid == -1)
                    {
                        throw runtime_error("Error: fork problem occured");
                        break;
                    }
                    if (fork_pid == 0)
                    {
                        create_server_node(child_id);
                    }

                    socket_left = zmq_socket(context, ZMQ_REQ);

                    cout << "OK: " << fork_pid << endl;
                    int opt = 0;
                    int rc = zmq_setsockopt(socket_left, ZMQ_LINGER, &opt, sizeof(opt));
                    assert(rc == 0);
                    if (socket_left == NULL)
                    {
                        throw runtime_error("Error: socket not created");
                    }
                    rc = zmq_connect(socket_left, convert_adr_client(BASE_PORT + child_id).c_str());
                    assert(rc == 0);
                    break;
                }
            }
            if (child_id > CLIENT_ROOT_ID)
            {
                if (socket_right != NULL)
                {
                    int replied = 0;
                    send_create(socket_right, child_id);
                    if (availible_receive(socket_right))
                    {
                        reply = receive(socket_right);
                        if (strcmp("", reply) != 0)
                        {
                            replied = 1;
                            cout << reply << endl;
                        }
                    }
                    if (replied == 0)
                    {
                        cout << "Error: node " << child_id << " unavailible" << endl;
                    }
                    break;
                }
            }
            if (child_id < CLIENT_ROOT_ID)
            {
                if (socket_left != NULL)
                {
                    int replied = 0;
                    send_create(socket_left, child_id);
                    if (availible_receive(socket_left))
                    {
                        reply = receive(socket_left);
                        if (strcmp("", reply) != 0)
                        {
                            replied = 1;
                            cout << reply << endl;
                        }
                    }
                    if (replied == 0)
                    {
                        cout << "Error: node " << child_id << " unavailible" << endl;
                    }
                    break;
                }
            }
            break;

        case REMOVE:
            remove_id_str = unitread(); // считывание id которое мы хотим удалить
            remove_id = atoi(remove_id_str.c_str());
            if (remove_id <= 0)
            {
                cout << "Error: invalid id" << endl;
                break;
            }
            if (!existNode(system, remove_id))
            {
                cout << "Error: Not found" << endl;
                break;
            }
            if (CLIENT_ROOT_ID == remove_id)
            {
                cout << "Error: can't delete manager root" << endl;
                break;
            }
            system = deleteNode(system, remove_id); // удаляем id из топологии
            if (remove_id > CLIENT_ROOT_ID)
            {
                int replied = 0;
                send_remove(socket_right, remove_id);
                if (availible_receive(socket_right))
                {
                    reply = receive(socket_right);
                    if (strcmp("", reply) != 0)
                    {
                        replied = 1;
                        cout << reply << endl;
                    }
                }
                if (replied == 0)
                {
                    cout << "Error: node " << child_id << " unavailible" << endl;
                }
                break;
            }
            else if (remove_id < CLIENT_ROOT_ID)
            {
                int replied = 0;
                send_remove(socket_left, remove_id);
                if (availible_receive(socket_left))
                {
                    reply = receive(socket_left);
                    if (strcmp("", reply) != 0)
                    {
                        replied = 1;
                        cout << reply << endl;
                    }
                }
                if (replied == 0)
                {
                    cout << "Error: node " << child_id << " unavailible" << endl;
                }
                break;
            }
            break;
        case EXEC:
        {
            exec_id_str = unitread();
            exec_id = atoi(exec_id_str.c_str());
            char *send_key = (char *)calloc(1, sizeof(char));
            char str_value[64];
            char c;
            int check = 1;
            int cnt_args = 1;
            int it = 0;
            int cur_size = 1;
            while ((c = getchar()))
            { // считываем ключ значение
                if (c == '\n')
                {
                    if (cnt_args == 1)
                    {
                        check = 0;
                    }
                    break;
                }
                if (c == ' ')
                {
                    cnt_args++;
                    it = 0;
                    continue;
                }
                if (cnt_args == 1)
                {
                    send_key[it] = c;
                    cur_size++;
                    send_key = (char *)realloc(send_key, cur_size * sizeof(char));
                }
                else
                {
                    str_value[it] = c;
                }
                it++;
            }

            if (check == 1)
            {
                value = stoi(str_value);
                save = 1;
            }
            else
            {
                value = 0;
                save = 0;
            }

            if (exec_id <= 0)
            {
                cout << "Error: invalid id" << endl;
                break;
            }
            if (!existNode(system, exec_id))
            {
                cout << "Error: Not found" << endl;
                break;
            }
            if (CLIENT_ROOT_ID == exec_id)
            {
                cout << "Error: it is a manager root" << endl;
                break;
            }
            if (exec_id > CLIENT_ROOT_ID)
            {
                int replied = 0;
                send_exec(socket_right, exec_id, send_key, value, save);
                if (availible_receive(socket_right))
                {
                    reply = receive(socket_right);
                    if (strcmp("", reply) != 0)
                    {
                        replied = 1;
                        cout << reply << endl;
                    }
                }
                if (replied == 0)
                {
                    cout << "Error: node " << exec_id << " unavailible" << endl;
                }
                break;
            }
            else if (exec_id < CLIENT_ROOT_ID)
            {
                int replied = 0;
                send_exec(socket_left, exec_id, send_key, value, save);
                if (availible_receive(socket_left))
                {
                    reply = receive(socket_left);
                    if (strcmp("", reply) != 0)
                    {
                        replied = 1;
                        cout << reply << endl;
                    }
                }
                if (replied == 0)
                {
                    cout << "Error: node " << exec_id << " unavailible" << endl;
                }
                break;
            }
            break;
        }
        case PING:
        {
            ping_id_str = unitread();
            ping_id = atoi(ping_id_str.c_str());
            if (ping_id <= 0)
            {
                cout << "Error: invalid id" << endl;
                break;
            }
            if (!existNode(system, ping_id))
            {
                cout << "Error: Not found" << endl;
                break;
            }
            if (CLIENT_ROOT_ID == ping_id)
            {
                cout << "Error: it is a manager root" << endl;
                break;
            }
            if (ping_id > CLIENT_ROOT_ID)
            {
                int replied = 0;
                send_ping(socket_right, ping_id);
                if (availible_receive(socket_right))
                {
                    reply = receive(socket_right);
                    if (strcmp("", reply) != 0)
                    {
                        replied = 1;
                        cout << reply << endl;
                    }
                }
                if (replied == 0)
                {
                    cout << "OK: 0" << endl;
                }
                break;
            }
            else if (ping_id < CLIENT_ROOT_ID)
            {
                int replied = 0;
                send_ping(socket_left, ping_id);
                if (availible_receive(socket_left))
                {
                    reply = receive(socket_left);
                    if (strcmp("", reply) != 0)
                    {
                        replied = 1;
                        cout << reply << endl;
                    }
                }
                if (replied == 0)
                {
                    cout << "OK: 0" << endl;
                }
                break;
            }
            break;
        }

        case EXIT:
            if (socket_right != NULL)
            {
                send_exit(socket_right);
            }
            if (socket_left != NULL)
            {
                send_exit(socket_left);
            }
            ex = 1;
            break;
        }
        if (ex == 1)
        {
            break;
        }
        free(reply);
    }
    zmq_close(socket_right);
    zmq_close(socket_left);
    zmq_ctx_destroy(context);
}
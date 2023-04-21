#include "config.h"

void create_server_node(int id)
{
    const char *arg = SERVER_PATH;
    const char *arg0 = int_to_str(id);
    execl(arg, arg0, NULL);
}

void send_create(void *socket, int id)
{
    command_type cmd = CREATE;
    zmq_msg_t command;
    zmq_msg_init_size(&command, sizeof(cmd));          // выделяем ресурсы для хранения сообщения
    memcpy(zmq_msg_data(&command), &cmd, sizeof(cmd)); // копируем содержимое одной области памяти в другую (записываем CREATE в сообщение)
    zmq_msg_send(&command, socket, ZMQ_SNDMORE);       // отправляем на сокет. ZMQ_SNDMORE - Указывает, что отправляемое сообщение состоит из нескольких частей и что за ним должны следовать дополнительные части сообщения. 
    zmq_msg_close(&command);                           // очищаем ресурсы для хранения сообщения

    zmq_msg_t id_msg;                                  // передаем вторую часть сообщения
    zmq_msg_init_size(&id_msg, sizeof(id));
    memcpy(zmq_msg_data(&id_msg), &id, sizeof(id));
    // указатель на содержимое сообщения
    zmq_msg_send(&id_msg, socket, 0);
    zmq_msg_close(&id_msg);
}

void send_remove(void *socket, int id)
{
    command_type cmd = REMOVE;
    zmq_msg_t command;
    zmq_msg_init_size(&command, sizeof(cmd));
    memcpy(zmq_msg_data(&command), &cmd, sizeof(cmd));
    zmq_msg_send(&command, socket, ZMQ_SNDMORE);
    zmq_msg_close(&command);

    zmq_msg_t id_msg;
    zmq_msg_init_size(&id_msg, sizeof(id));
    memcpy(zmq_msg_data(&id_msg), &id, sizeof(id));
    zmq_msg_send(&id_msg, socket, 0);
    zmq_msg_close(&id_msg);
}

void send_exec(void *socket, int id, char *key, int value, int save)
{
    command_type cmd = EXEC;
    zmq_msg_t command;
    zmq_msg_init_size(&command, sizeof(cmd));
    memcpy(zmq_msg_data(&command), &cmd, sizeof(cmd));
    zmq_msg_send(&command, socket, ZMQ_SNDMORE);
    zmq_msg_close(&command);

    zmq_msg_t id_msg;
    zmq_msg_init_size(&id_msg, sizeof(id));
    memcpy(zmq_msg_data(&id_msg), &id, sizeof(id));
    zmq_msg_send(&id_msg, socket, ZMQ_SNDMORE);
    zmq_msg_close(&id_msg);

    if (save == 0)
    { // есть только ключ
        const char *send_key = (char *)malloc(sizeof(key));
        send_key = key;
        zmq_msg_t key_msg;
        zmq_msg_init_size(&key_msg, sizeof(send_key));
        memcpy(zmq_msg_data(&key_msg), send_key, sizeof(send_key));
        zmq_msg_send(&key_msg, socket, 0);
        zmq_msg_close(&key_msg);
        return;
    }

    const char *send_key = (char *)malloc(sizeof(key));
    send_key = key;
    zmq_msg_t key_msg;
    zmq_msg_init_size(&key_msg, sizeof(send_key));
    memcpy(zmq_msg_data(&key_msg), send_key, sizeof(send_key));
    zmq_msg_send(&key_msg, socket, ZMQ_SNDMORE);
    zmq_msg_close(&key_msg);

    if (save == 1)
    {
        zmq_msg_t value_msg;
        zmq_msg_init_size(&value_msg, sizeof(value));
        memcpy(zmq_msg_data(&value_msg), &value, sizeof(value));
        zmq_msg_send(&value_msg, socket, 0);
        zmq_msg_close(&value_msg);
    }
}

// проверяем сокет на наличие новых сообщений
bool availible_receive(void *socket)
{ 
    zmq_pollitem_t items[1] = {{socket, 0, ZMQ_POLLIN, 0}};
    int rc = zmq_poll(items, 1, REQUEST_TIMEOUT);
    assert(rc != -1);
    if (items[0].revents & ZMQ_POLLIN)
        return true;
    return false;
}

char *receive(void *socket)
{
    zmq_msg_t reply;
    zmq_msg_init(&reply); // инициализирует объект пустого сообщения
    zmq_msg_recv(&reply, socket, 0);
    size_t result_size = zmq_msg_size(&reply);

    char *result = (char *)calloc(sizeof(char), result_size + 1);
    memcpy(result, zmq_msg_data(&reply), result_size);
    zmq_msg_close(&reply);

    return result;
}

void send_exit(void *socket)
{
    command_type cmd = EXIT;
    zmq_msg_t command_msg;
    zmq_msg_init_size(&command_msg, sizeof(cmd));
    memcpy(zmq_msg_data(&command_msg), &cmd, sizeof(cmd));
    zmq_msg_send(&command_msg, socket, 0);
    zmq_msg_close(&command_msg);
}

void send_ping(void *socket, int id)
{
    command_type cmd = PING;
    zmq_msg_t command;
    zmq_msg_init_size(&command, sizeof(cmd));
    memcpy(zmq_msg_data(&command), &cmd, sizeof(cmd));
    zmq_msg_send(&command, socket, ZMQ_SNDMORE);
    zmq_msg_close(&command);

    zmq_msg_t id_msg;
    zmq_msg_init_size(&id_msg, sizeof(id));
    memcpy(zmq_msg_data(&id_msg), &id, sizeof(id));
    zmq_msg_send(&id_msg, socket, 0);
    zmq_msg_close(&id_msg);
}

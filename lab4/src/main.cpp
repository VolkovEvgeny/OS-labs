#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>

using namespace std;

int main(int argc, char const *argv[])
{
    string current_str;
    int child_tag;
    string child1, child2;
    cout << "Enter the name for first child file: ";
    cin >> child1;
    cout << "Enter the name for second child file: ";
    cin >> child2;

    shm_unlink("1.back"); // удаляем старые объекты разделяемой памяти из директории /dev/shm
    shm_unlink("2.back");
    sem_unlink("_sem1"); // также удаляем старые семафоры
    sem_unlink("_sem2");
    // создаем семафоры по названию, даем им права на доступ, присваиваем им значение 1
    sem_t *sem1 = sem_open("_sem1", O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH, 1);
    sem_t *sem2 = sem_open("_sem2", O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH, 1);
    int state = 0;        // переменная для получения значения семафора
    pid_t f_id1 = fork(); // создаем 1 процесс
    if (f_id1 == -1)
    {
        cout << "Fork error with code -1 returned in the parent, no child_1 process is created" << endl;
        exit(EXIT_FAILURE);
    }
    else if (f_id1 == 0) // в 1 процессе
    {
        sem_close(sem1); // закрываем семафор 1 в основном процессе
        string child = child1;
        // передаем в 1 дочерний процесс название объекта разделяемой памяти, семафора и файла
        execlp("./child", child.c_str(), "1.back", "_sem1", NULL); 
        perror("Execlp error");
        return 0;
    }
    else
    {
        pid_t f_id2 = fork();
        if (f_id2 == -1)
        {
            cout << "Fork error with code -1 returned in the parent, no child_2 process is created" << endl;
            exit(EXIT_FAILURE);
        }
        else if (f_id2 == 0)
        {
            sem_close(sem2);
            string child = child2;
            execlp("./child", child.c_str(), "2.back", "_sem2", NULL);
            perror("Execlp error");
            return 0;
        }

        else
        {
            while (getline(std::cin, current_str))
            {
                int s_size = current_str.size() + 1;    // получаем размер строки
                char *buffer = (char *)current_str.c_str(); // получаем строку
                if (current_str.size() <= 10)
                {
                    // открываем объект разделяемой памяти
                    int fd = shm_open("1.back", O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
                    ftruncate(fd, s_size); // устанавливаем размер файла на размер строки
                    // отображаем данные из файла в оперативную память
                    char *mapped = (char *)mmap(NULL, s_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
                    memset(mapped, '\0', s_size); // заполняем объект '\0'
                    sprintf(mapped, "%s", buffer);  // пишем туда строку
                    munmap(mapped, s_size); // удаляем отображение
                    close(fd); // закрываем файл
                    sem_wait(sem1); // уменьшаем семафор на 1
                }
                else
                {
                    int fd = shm_open("2.back", O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
                    ftruncate(fd, s_size);
                    char *mapped = (char *)mmap(NULL, s_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
                    memset(mapped, '\0', s_size);
                    sprintf(mapped, "%s", buffer);
                    munmap(mapped, s_size);
                    close(fd);
                    sem_wait(sem2);
                }
            }
        }
    }
    sem_close(sem1);
    sem_close(sem2);
    return 0;
}

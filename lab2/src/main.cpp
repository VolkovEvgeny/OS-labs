#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

int main(int argc, char const *argv[])
{
    string current_str; // строка, которую мы будем передавать в дочерний процесс через pipe
    string child1, child2;  // объявляем и вводим названия для двух файлов
    cout << "Enter the name for first child file: ";
    cin >> child1;
    cout << "Enter the name for second child file: ";
    cin >> child2;
    int fd1[2]; // файловые дескрипторы для pipe, связанного с первым дочерним процессом
    int fd2[2]; // файловые дескрипторы для pipe, связанного с вторым дочерним процессом

    if (pipe(fd1) == -1)    // создаем pipe для 1-го процесса
    {
        cout << "Pipe error occured" << endl;
        exit(EXIT_FAILURE);
    }
    if (pipe(fd2) == -1)    // создаем pipe для 2-го процесса
    {
        cout << "Pipe error occured" << endl;
        exit(EXIT_FAILURE);
    }

    pid_t f_id1 = fork();   // создаем первый дочерний процесс
    
    if (f_id1 == -1)    // если ошибка, выходим из программы
    {
        cout << "Fork error with code -1 returned in the parent, no child_1 process is created" << endl;
        exit(EXIT_FAILURE);
    }
    else if (f_id1 == 0)    // если f_id1 == 0, то мы находимся в первом дочернем процессе
    {
        // передаем первый дочерний процесс на исполнение другой программе, скомпилированной из child.cpp
        // на вход передаем файловые дескрипторы pipe1 и название первого файла
        execlp("./child", to_string(fd1[0]).c_str(), to_string(fd1[1]).c_str(), child1.c_str(), NULL);  
        perror("Execlp error"); // в случае если execlp не сработал, выводим ошибку
        return 0;   // завершаем работу дочернего процесса в этой программе
    } 
    else {  // если f_id1 != 0, то мы находимся в основном процессе (f_id1 равен id 1 дочернего процесса)
        pid_t f_id2 = fork();   // создаем второй дочерний процесс
        if (f_id2 == -1)
        {
            cout << "Fork error with code -1 returned in the parent, no child_2 process is created" << endl;
            exit(EXIT_FAILURE);
        }
        else if (f_id2 == 0)
        {
            // передаем второй дочерний процесс на исполнение другой программе, скомпилированной из child.cpp
            // на вход передаем файловые дескрипторы pipe2 и название второго файла
            execlp("./child", to_string(fd2[0]).c_str(), to_string(fd2[1]).c_str(), child2.c_str(), NULL);
            perror("Execlp error"); // в случае если execlp не сработал, выводим ошибку
            return 0;
        } 
        
        else    // основной процесс
        {
            while (getline(std::cin, current_str))  // считываем строку из терминала до '\n'
            {
                int s_size = current_str.size() + 1;   // получаем размер строки
                if (current_str.size() <= 10)   // отправляем в первый процесс
                {
                    write(fd1[1], &s_size, sizeof(int));    // отправляем размер строки в pipe
                    write(fd1[1], current_str.c_str(), s_size); // отправляем саму строку в pipe
                }
                else    // во второй процесс
                {
                    write(fd2[1], &s_size, sizeof(int));
                    write(fd2[1], current_str.c_str(), s_size);
                }
            }
        }
    }
    close(fd2[1]);  // закрываем файлы pipe-ов
    close(fd1[1]);
    close(fd2[0]);
    close(fd1[0]);
    return 0;
}

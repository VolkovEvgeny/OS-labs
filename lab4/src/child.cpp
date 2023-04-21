#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>
#include <fstream>
#include <set>

using namespace std;

int main(int argc, char const *argv[])
{
    char *semFile = (char *) argv[2];
    sem_t *sem = sem_open(semFile,  O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH, 0);
    std::string vovels = "aoueiy";
    std::set<char> volSet(vovels.begin(), vovels.end());
    string filename = argv[0];
    fstream cur_file;
    cur_file.open(filename, fstream::in | fstream::out | fstream::app);
    char *backfile = (char *) argv[1];
    int state = 1; // переменная для получения значения семафора
    while (1)
    {
        sem_getvalue(sem, &state); // получаем значение семафора в переменную state
        if (state == 0) {   // ждем пока state != 0
            int fd = shm_open(backfile, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
            struct stat statBuf; 
            fstat(fd, &statBuf); // получаем данные о файле
            int size_of_str = statBuf.st_size; // получаем размер файла из структуры
            ftruncate(fd, size_of_str); // устанавливаем размер файла 
            char *mapped = (char *) mmap(NULL, size_of_str, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); // отображаем файл в память
            std::string allocated = mapped; // исходная строка
            string result_str;
            for (int i = 0; i < size_of_str; i++) {
                if (volSet.find(std::tolower(allocated[i])) == volSet.cend()) {
                    result_str.push_back(allocated[i]);
                }
            }
            cur_file << result_str << endl;
            close(fd);  
            munmap(mapped, size_of_str);
            sem_post(sem);
        }
    }
    sem_close(sem);
    return 0;
}
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <set>

using namespace std;

int main(int argc, char const *argv[])
{
    std::string vovels = "aoueiy";  // гласные буквы
    std::set<char> volSet(vovels.begin(), vovels.end());    // создаем множество всех гласных букв
    string filename = argv[2];  // получили название файла 
    int fd[2];  // создаем дескрипторы для pipe в дочернем процессе
    fd[0] = stoi(argv[0]);  // отсюда читаем
    fd[1] = stoi(argv[1]);  // сюда пишем
    fstream cur_file;   // создаем файловый поток чтобы открыть txt файл
    cur_file.open(filename, fstream::in | fstream::out | fstream::app); // открываем файл на чтение и запись
    while (true)
    {
        int size_of_str;
        read(fd[0], &size_of_str, sizeof(int)); // получаем размер строки
        char str_array[size_of_str];
        read(fd[0], &str_array, sizeof(char) * size_of_str);    // получаем исходную строку
        string result_str; // объ
        // алгоритм удаления гласных букв
        for (int i = 0; i < size_of_str; i++) {
            if (volSet.find(std::tolower(str_array[i])) == volSet.cend()) {
                result_str.push_back(str_array[i]); // в result_str только гласные
            }
        }
        cur_file << result_str << endl; // пишем result_str в файл
    }
    return 0;
}

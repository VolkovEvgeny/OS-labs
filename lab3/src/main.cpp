#include <iostream>
#include <pthread.h>
#include <vector>
#include <ctime>

using matrix = std::vector<std::vector<double>>;

struct PthreadData
{
    matrix &pixel_matr;         // матрица 
    matrix &copy_pixel_matr;    // ее копия (результирующая матрица)
    matrix &convolution_matr;   // матрица свертки
    std::pair<int, int> current_pixel;  //  пиксель (скорее всего i, j элемента матрицы)
    int pixel_matr_height;        // размеры матрицы
    int pixel_matr_width;
    int convolution_matr_side;    // размер матрицы свертки
    PthreadData(matrix &pixel_matr, matrix &copy_pixel_matr, matrix &convolution_matr, std::pair<int, int> &current_pixel); // констуктор
    void calculate();             // функция подсчета свертки
};

PthreadData::PthreadData(matrix &pixel_matr, matrix &copy_pixel_matr, matrix &convolution_matr, std::pair<int, int> &current_pixel)
    : pixel_matr(pixel_matr), copy_pixel_matr(copy_pixel_matr), convolution_matr(convolution_matr), current_pixel(current_pixel),
      pixel_matr_height(pixel_matr.size()), pixel_matr_width(pixel_matr[0].size()),
      convolution_matr_side(convolution_matr.size()) 
{
}

void PthreadData::calculate()   // обрабатывает 1 элемент
{
    double sum(0);  // сумма
    std::pair<int, int> cur = std::pair<int, int>(current_pixel.first - convolution_matr_side / 2,
                                                  current_pixel.second - convolution_matr_side / 2); // -1, -1
    // идем переменными cur_conv_y и cur_conv_x по матрице свертки, cur_pixel_y а cur_pixel_y и cur_pixel_x по исходной матрице (не полностью)
    for (int cur_conv_y = 0, cur_pixel_y = cur.first; cur_conv_y < convolution_matr_side; cur_conv_y++, cur_pixel_y++)
    {
        for (int cur_conv_x = 0, cur_pixel_x = cur.second; cur_conv_x < convolution_matr_side; cur_conv_x++, cur_pixel_x++)
        {
            int koef_x = 0, koef_y = 0;
            if (cur_pixel_x < 0)
            {
                koef_x = -cur_pixel_x;
            }
            else if (cur_pixel_x > (pixel_matr_width - 1))
            {
                koef_x = -cur_pixel_x + pixel_matr_width - 1;
            }
            if (cur_pixel_y < 0)
            {
                koef_y = -cur_pixel_y;
            }
            else if (cur_pixel_y > (pixel_matr_height - 1))
            {
                koef_y = -cur_pixel_y + pixel_matr_height - 1;
            }
            sum += pixel_matr[cur_pixel_y + koef_y][cur_pixel_x + koef_x] * convolution_matr[cur_conv_y][cur_conv_x];
        }
    }
    copy_pixel_matr[current_pixel.first][current_pixel.second] = sum;
}

void *thread_func(void *data)       // функция потока (рутина), которая накладывает фильтр на матрицу
{
    static_cast<PthreadData *>(data)->calculate(); // static_cast преобразует void * в класс
    return NULL;
}

void print_matr(matrix &m)
{
    for (int i = 0; i < m.size(); i++)
    {
        for (int j = 0; j < m[0].size(); j++)
        {
            std::cout << m[i][j] << '\t';
        }
        std::cout << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    int start_time = clock(); // начало замера времени
    int num_of_threads;       // количество потоков
    int pixel_matr_height, pixel_matr_width; // размеры матрицы
    int convolution_matr_side = 3;           // размеры матрицы свертки (размера 3 на 3, по условию)

    std::cout << "Enter the height and width of matrix\n";
    std::cin >> pixel_matr_height >> pixel_matr_width;

    matrix pixel_matr(pixel_matr_height, std::vector<double>(pixel_matr_width, 0)); // создаем матрицу нужных размеров
    matrix copy_pixel_matr = pixel_matr;                                            // копия матрицы 

    std::cout << "Enter the matrix\n";
    for (int i = 0; i < pixel_matr_height; i++)                                     // заполняем матрицу
    {
        for (int j = 0; j < pixel_matr_width; j++)
        {
            std::cin >> pixel_matr[i][j];
        }
    }

    matrix convolution_matr(convolution_matr_side, std::vector<double>(convolution_matr_side, 0)); // создаем матрицу свертки 3 на 3

    std::cout << "Enter the convolution matrix size of 3x3\n";
    for (int i = 0; i < convolution_matr_side; i++)                                                // заполняем матрицу свертки
    {
        for (int j = 0; j < convolution_matr_side; j++)
        {
            std::cin >> convolution_matr[i][j];
        }
    }

    std::cout << "Enter number of threads\n";
    std::cin >> num_of_threads;

    std::pair<int, int> init_current_pixel(std::pair<int, int>(0, 0));                  //  пиксель (скорее всего i, j элемента матрицы)
    PthreadData init_unit(pixel_matr, copy_pixel_matr, convolution_matr, init_current_pixel);   // создаем объект класса, применяющего алгоритм свертки к матрице
    std::vector<pthread_t> threads(num_of_threads);                                     // вектор потоков                               
    std::vector<PthreadData> pthreaddata(num_of_threads, init_unit);                    // вектор классов для работы над матрицами для каждого потока

    int &&num_of_pixels = pixel_matr_height * pixel_matr_width;                         // количество элементов матрицы
    if (num_of_threads > num_of_pixels)
    {
        num_of_threads = num_of_pixels;
    }

    int num_of_execution;

    std::cout << "Enter k\n";
    std::cin >> num_of_execution;                                                       // количество применений алгоритма

    for (int i = 0; i < num_of_execution; i++)
    {
        int pixels_done = 0;
        while (pixels_done < num_of_pixels)                                             // для каждого элемента применяем алгоритм
        {
            int threads_to_open = std::min(num_of_threads, num_of_pixels - pixels_done); // количество необходимых потоков для обработки num_of_pixels элементов
            for (int j = 0; j < threads_to_open; j++)                                   // за одну итерацию можем параллельно обработать threads_to_open элементов
            {
                int &&current_pixel = (pixels_done + 1) - 1;                            // вычисляем текущий элемент
                // присваиваем текущему элементу класса j-го потока координаты current_pixel в матрице
                pthreaddata[j].current_pixel = std::pair<int, int>(current_pixel / pixel_matr_width, current_pixel % pixel_matr_width); 
                ++pixels_done;
                pthread_create(&(threads[j]), NULL, thread_func, &(pthreaddata[j])); // создает поток threads[j], который будет работать с аргументами pthreaddata[j]
            }
            for (int j = 0; j < threads_to_open; j++)
            {
                pthread_join(threads[j], NULL); // ждет поток и вывод потока
            }
        }
        std::swap(pixel_matr, copy_pixel_matr); // присваиваем результат исходной матрице
        pixels_done = 0;
    }
    std::cout << "Result matrix:\n";
    print_matr(pixel_matr);
    int end_time = clock(); // время в конце
    std::cout << "Time:\n";
    std::cout << end_time - start_time << std::endl;
}

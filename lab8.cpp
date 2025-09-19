#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <mutex>
#include <algorithm>
#include <fstream>
#include <string>
#include <cmath>
#include <iomanip>

using namespace std;
using namespace chrono;

// Класс для умножения матриц
class MatrixMultiplier {
private:
    mutex mtx;
    
public:
    // Обычное последовательное умножение матриц
    vector<vector<double>> multiplySequential(const vector<vector<double>>& A, 
                                            const vector<vector<double>>& B) {
        int n = A.size();
        int m = B[0].size();
        int p = B.size();
        
        vector<vector<double>> C(n, vector<double>(m, 0.0));
        
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                for (int k = 0; k < p; k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
        
        return C;
    }
    
    // Параллельное умножение матриц (по строкам)
    vector<vector<double>> multiplyParallel(const vector<vector<double>>& A, const vector<vector<double>>& B, int num_threads = thread::hardware_concurrency()) {
        int n = A.size();
        int m = B[0].size();
        int p = B.size();
        
        vector<vector<double>> C(n, vector<double>(m, 0.0));
        vector<thread> threads;
        
        // Функция для работы потока
        auto thread_func = [&](int start_row, int end_row) {
            for (int i = start_row; i < end_row; i++) {
                for (int j = 0; j < m; j++) {
                    double sum = 0.0;
                    for (int k = 0; k < p; k++) {
                        sum += A[i][k] * B[k][j];
                    }
                    C[i][j] = sum;
                }
            }
        };
        
        // Распределяем строки между потоками
        int rows_per_thread = n / num_threads;
        int extra_rows = n % num_threads;
        int current_row = 0;
        
        for (int i = 0; i < num_threads; i++) {
            int rows_to_process = rows_per_thread + (i < extra_rows ? 1 : 0);
            if (rows_to_process > 0) {
                threads.emplace_back(thread_func, current_row, current_row + rows_to_process);
                current_row += rows_to_process;
            }
        }
        
        // Ждем завершения всех потоков
        for (auto& t : threads) {
            t.join();
        }
        
        return C;
    }
    
    // Генерация случайной матрицы
    vector<vector<double>> generateRandomMatrix(int n, int m) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<double> dis(0.0, 10.0);
        
        vector<vector<double>> matrix(n, vector<double>(m));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                matrix[i][j] = dis(gen);
            }
        }
        return matrix;
    }
    
    // Проверка корректности результатов (с учетом погрешности вычислений с плавающей точкой)
    bool areMatricesEqual(const vector<vector<double>>& A, const vector<vector<double>>& B, double tolerance = 1e-6) {
        if (A.size() != B.size() || A[0].size() != B[0].size()) {
            return false;
        }
        
        for (size_t i = 0; i < A.size(); i++) {
            for (size_t j = 0; j < A[0].size(); j++) {
                if (abs(A[i][j] - B[i][j]) > tolerance) {
                    return false;
                }
            }
        }
        return true;
    }
    
    // Замер времени выполнения
    template<typename Func, typename... Args>
    double measureTime(Func func, Args&&... args) {
        auto start = high_resolution_clock::now();
        func(forward<Args>(args)...);
        auto end = high_resolution_clock::now();
        return duration<double, milli>(end - start).count();
    }
    
    // Запуск тестов
    void runBenchmarks() {
        vector<int> sizes = {64, 256, 1024, 4096}; // 2^6, 2^8, 2^10, 2^12
        const int iterations = 10;
        
        vector<vector<double>> sequential_times(sizes.size());
        vector<vector<double>> parallel_times(sizes.size());
        
        cout << "количество свободных потоков - " << thread::hardware_concurrency() << "\n\n";
        
        for (size_t idx = 0; idx < sizes.size(); idx++) {
            int n = sizes[idx];
            cout << "размер матрицы" << n << "x" << n << "\n";
            
            auto A = generateRandomMatrix(n, n);
            auto B = generateRandomMatrix(n, n);
            
            auto start_ref = high_resolution_clock::now();
            auto C_ref = multiplySequential(A, B);
            auto end_ref = high_resolution_clock::now();
            
            // Тестируем последовательный алгоритм
            cout << "Обычное умножеие матриц ";
            for (int i = 0; i < iterations; i++) {
                double time = measureTime([&]() {
                    auto C = multiplySequential(A, B);
                });
                sequential_times[idx].push_back(time);
            }
            cout << "- готово\n";
            
            // Тестируем параллельный алгоритм
            cout << "Параллельное умножение матриц";
            for (int i = 0; i < iterations; i++) {
                double time = measureTime([&]() {
                    auto C = multiplyParallel(A, B);
                });
                parallel_times[idx].push_back(time);
            }
            cout << "- готово\n";
            
            // Выводим статистику
            auto avg_seq = accumulate(sequential_times[idx].begin(), sequential_times[idx].end(), 0.0) / iterations;
            auto avg_par = accumulate(parallel_times[idx].begin(), parallel_times[idx].end(), 0.0) / iterations;
            auto speedup = avg_seq / avg_par;
            
            cout << "Размер матрицы " << n << "x" << n << ": "
                 << "Время последовательного выполнения: " << avg_seq << " мс, "
                 << "Время параллельного выполнения: " << avg_par << " мс, "
                 << "Разница: "  << speedup << "\n";
        }
    }
    void runBoxplotPar(int size = 64) {
        const int iterations = 10;
        
        vector<vector<double>> sequential_times(1);
        vector<vector<double>> parallel_times(1);
        
        cout << "количество свободных потоков - " << thread::hardware_concurrency() << "\n\n";
        
        for (int num = 0; num < 10; num++) {
            int n = size;
            cout << "размер матрицы" << n << "x" << n << "\n";
            
            auto A = generateRandomMatrix(n, n);
            auto B = generateRandomMatrix(n, n);
            
            auto start_ref = high_resolution_clock::now();
            auto C_ref = multiplySequential(A, B);
            auto end_ref = high_resolution_clock::now();
            
            for (int i = 0; i < iterations; i++) {
                double time = measureTime([&]() {
                    auto C = multiplyParallel(A, B);
                });
                parallel_times[1].push_back(time);
            }
            
            // Выводим статистику
            auto avg_par = accumulate(parallel_times[1].begin(), parallel_times[1].end(), 0.0) / iterations;
            cout << num << avg_par << " мс, ";
        }
    }
};

int main() {
    MatrixMultiplier multiplier;
    multiplier.runBoxplotPar();
    return 0;
}
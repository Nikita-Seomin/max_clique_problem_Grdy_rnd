#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>
#include <random>
#include <unordered_set>
#include <algorithm>
using namespace std;

class MaxCliqueProblem
{
public:
    // Генерирование случайных чисел на отрезке [a, b]
    static int GetRandom(int a, int b)
    {
        static mt19937 generator(time(nullptr)); // Сбросим состояние генератора (по времени)
        uniform_int_distribution<int> uniform(a, b);
        return uniform(generator);
    }

    // Чтение графа из файла формата DIMACS
    void ReadGraphFile(string filename)
    {
        ifstream fin(filename);
        string line;
        int vertices = 0, edges = 0;
        while (getline(fin, line))
        {
            if (line[0] == 'c') continue; // Пропускаем комментарии

            stringstream line_input(line);
            char command;
            if (line[0] == 'p') // Обрабатываем строку с параметрами
            {
                string type;
                line_input >> command >> type >> vertices >> edges;
                neighbour_sets.resize(vertices);
            }
            else // Добавляем ребра
            {
                int start, finish;
                line_input >> command >> start >> finish;
                neighbour_sets[start - 1].insert(finish - 1);
                neighbour_sets[finish - 1].insert(start - 1);
            }
        }
    }

    // Реализация жадного рандомизированного поиска максимальной клики
    void GreedyRandomizedMaximumClique(int randomization, int iterations)
    {
        static mt19937 generator(time(nullptr)); // Локальный экземпляр генерации
        for (int iteration = 0; iteration < iterations; ++iteration)
        {
            vector<int> current_clique; // Текущая найденная клика
            vector<int> candidates(neighbour_sets.size());
            // Заполняем кандидатов вершинами
            for (int i = 0; i < neighbour_sets.size(); ++i)
            {
                candidates[i] = i;
            }
            // Перемешиваем кандидатов
            shuffle(candidates.begin(), candidates.end(), generator);

            // Построение клики жадным образом
            while (!candidates.empty())
            {
                int last = candidates.size() - 1;
                int rnd = GetRandom(0, min(randomization - 1, last)); // Случайный выбор
                int vertex = candidates[rnd]; // Берем вершину
                current_clique.push_back(vertex);

                // Фильтруем список кандидатов, оставляя только соседей
                for (int c = 0; c < candidates.size(); ++c)
                {
                    int candidate = candidates[c];
                    if (neighbour_sets[vertex].count(candidate) == 0) // Если нет ребра
                    {
                        // Убираем кандидата (обмениваем с последним)
                        swap(candidates[c], candidates[last]);
                        candidates.pop_back();
                        --c;
                        --last;
                    }
                }
                shuffle(candidates.begin(), candidates.end(), generator); // Перемешиваем остаток
            }
            // Сохраняем лучшую клику, если текущая оказалась больше
            if (current_clique.size() > best_clique.size())
            {
                best_clique = current_clique;
            }
        }
    }

    // Возвращает найденную максимальную клику
    const vector<int>& GetClique()
    {
        return best_clique;
    }

    // Проверка найденной клики на корректность
    bool Check()
    {
        // Проверяем, что клика не содержит дубликатов
        if (unique(best_clique.begin(), best_clique.end()) != best_clique.end())
        {
            cout << "Duplicated vertices in the clique\n";
            return false;
        }
        // Проверяем, что каждая пара вершин в клике соединена ребром
        for (int i : best_clique)
        {
            for (int j : best_clique)
            {
                if (i != j && neighbour_sets[i].count(j) == 0)
                {
                    cout << "Returned subgraph is not a clique\n";
                    return false;
                }
            }
        }
        return true;
    }

private:
    vector<unordered_set<int>> neighbour_sets; // Список соседей для каждой вершины
    vector<int> best_clique; // Лучшая найденная клика
};

int main()
{
    // Ввод параметров: количество итераций и рандомизация
    int iterations = 100000;
    // cout << "Number of iterations: ";
    // cin >> iterations;
    int randomization = 100000;
    // cout << "Randomization: ";
    // cin >> randomization;

    // Список файлов для проверки
    vector<string> files = { "brock200_1.clq", "brock200_2.clq", "brock200_3.clq", "brock200_4.clq", "brock400_1.clq",
        "brock400_2.clq", "brock400_3.clq", "brock400_4.clq", "C125.9.clq", "gen200_p0.9_44.clq", "gen200_p0.9_55.clq", "hamming8-4.clq",
        "johnson16-2-4.clq", "johnson8-2-4.clq", "keller4.clq", "MANN_a27.clq", "MANN_a9.clq", "p_hat1000-1.clq",
        "p_hat1000-2.clq", "p_hat1000-2.clq", "p_hat1500-1.clq", "p_hat300-3.clq", "p_hat500-3.clq", "san1000.clq", "sanr200_0.9.clq",
        "sanr400_0.7.clq" };

    // Для каждого файла запускаем алгоритм поиска максимальной клики
    for (string file : files)
    {
        MaxCliqueProblem problem;
        problem.ReadGraphFile(file);

        clock_t start = clock(); // Засекаем время
        problem.GreedyRandomizedMaximumClique(randomization, iterations); // Запуск алгоритма
        if (!problem.Check()) // Проверка корректности результата
        {
            cout << "*** WARNING: incorrect clique ***\n";
            cout << "Output:\n0\n";
            continue;
        }

        const vector<int>& clique = problem.GetClique();
        cout << "\nOutput:\n" << file << " " <<  clique.size() << ' ' << double(clock() - start) / 1000 <<   "\n"; // Первая строка: размер максимальной клики
        for (int i = 0; i < clique.size(); ++i)
        {
            cout << clique[i] + 1; // Возвращаем в 1-индексный формат
            if (i < clique.size() - 1) cout << " ";
        }
        cout << '\n';
    }
    return 0;
}
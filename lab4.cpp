#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <fstream>

using namespace std;

class FordFulkerson {
private:
    int V; // количество вершин
    vector<vector<int>> capacity; // матрица ёмкостей
    vector<vector<int>> flow;     
    vector<int> parent;           

public:
    FordFulkerson(int vertices) : V(vertices), capacity(vertices, vector<int>(vertices, 0)), 
                                  flow(vertices, vector<int>(vertices, 0)), parent(vertices, -1) {}

    void addEdge(int u, int v, int cap) {
        capacity[u][v] = cap;
    }

    bool bfs(int source, int end) {
        fill(parent.begin(), parent.end(), -1);
        vector<bool> visited(V, false);
        queue<int> q;
        q.push(source);
        visited[source] = true;

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (int v = 0; v < V; ++v) {
                if (!visited[v] && capacity[u][v] > flow[u][v]) {
                    q.push(v);
                    visited[v] = true;
                    parent[v] = u;
                    if (v == end) return true;
                }
            }
        }
        return false;
    }

    int maxFlow(int source, int end) {
        int totalFlow = 0;
        int iterations = 0;

        while (bfs(source, end)) {
            ++iterations;
            int pathFlow = INT_MAX;
            int s = end;

            while (s != source) {
                int u = parent[s];
                pathFlow = min(pathFlow, capacity[u][s] - flow[u][s]);
                s = u;
            }

            s = end;
            while (s != source) {
                int u = parent[s];
                flow[u][s] += pathFlow;
                flow[s][u] -= pathFlow;
                s = u;
            }

            totalFlow += pathFlow;
        }

        cout << "Итераций: " << iterations << endl;
        return totalFlow;
    }

    int getEdgeCount() {
    int count = 0;
    for (int i = 0; i < V; ++i) {
        for (int j = 0; j < V; ++j) {
            if (capacity[i][j] > 0) {
                count++;
            }
        }
    }
    return count;
    }

    void printGraph() {
        cout << "\nГраф (ёмкости):\n";
        for (int i = 0; i < V; ++i) {
            for (int j = 0; j < V; ++j) {
                cout << capacity[i][j] << " ";
            }
            cout << endl;
        }
    }

    void printFlow() {
        cout << "\nраф (потоки):\n";
        for (int i = 0; i < V; ++i) {
            for (int j = 0; j < V; ++j) {
                cout << flow[i][j] << " ";
            }
            cout << endl;
        }
    }

    
};

// Тест 1: Простой граф
void test1() {
    cout << "\nТест 1 - Простой граф\n";  
    FordFulkerson ff(6);
    ff.addEdge(0, 1, 16);
    ff.addEdge(0, 2, 13);
    ff.addEdge(1, 2, 10);
    ff.addEdge(1, 3, 12);
    ff.addEdge(2, 4, 14);
    ff.addEdge(3, 2, 7);
    ff.addEdge(3, 4, 7);
    ff.addEdge(3, 5, 20);
    ff.addEdge(4, 5, 4);

    ff.printGraph();
    cout << "Размер графа: " << ff.getEdgeCount() << endl;
    int max_flow = ff.maxFlow(0, 5);
    cout << "Максимальный поток: " << max_flow << endl;
}

// Тест 2: Граф медленной работы (большое число итераций)
void test2() {
    cout << "\nТест 2 - Очень медленный граф\n";
    FordFulkerson ff(6);
    
    ff.addEdge(0, 1, 100);
    ff.addEdge(0, 2, 100);
    ff.addEdge(1, 2, 1);    // Критическое ребро 1
    ff.addEdge(1, 3, 100);
    ff.addEdge(2, 3, 100);
    ff.addEdge(3, 4, 1);    // Критическое ребро 2
    ff.addEdge(3, 5, 100);
    ff.addEdge(4, 5, 100);

    ff.printGraph();
    cout << "Размер графа: " << ff.getEdgeCount() << endl;
    int max_flow = ff.maxFlow(0, 5);
    cout << "Максимальный поток: " << max_flow << endl;
}


// Тест 3: Граф несходимости (нет пути от источника к стоку)
void test3() {
    cout << "\nТест 3 - Граф несходимости\n";
    FordFulkerson ff(4);
    ff.addEdge(0, 1, 10);
    ff.addEdge(1, 2, 5);
    ff.addEdge(2, 3, 8); // Убираем связь между 0 и 3 напрямую

    ff.printGraph();
    cout << "Размер графа: " << ff.getEdgeCount() << endl;
    int max_flow = ff.maxFlow(0, 3);
    cout << "Максимальный поток: " << max_flow << endl;
}

// Тест 4: Циклический граф с несколькими путями
void test4() {
    cout << "\nТест 4 - Граф с несколькими путями\n";
    FordFulkerson ff(5);
    ff.addEdge(0, 1, 12);
    ff.addEdge(0, 2, 10);
    ff.addEdge(1, 2, 5);
    ff.addEdge(1, 3, 10);
    ff.addEdge(2, 4, 12);
    

    ff.printGraph();
    cout << "Размер графа: " << ff.getEdgeCount() << endl;
    int max_flow = ff.maxFlow(0, 4);
    cout << "Максимальный поток: " << max_flow << endl;
}

int main() {
    cout << "Реализация алгоритма Форда-Фалкерсона\n";

    test1();
    test2();
    test3();
    test4();

    return 0;
}
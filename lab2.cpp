#include <vector>
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <string>
#include <sstream>
#include <climits>
#include <exception>
using namespace std;

template <typename T>
struct Node {
    T value;
    Node* lnode;  // не используется в этой реализации, но оставлен по условию
    Node* rnode;  // указатель на следующий узел в бакете хеш-таблицы (для разрешения коллизий)
    Node* next;   // указатель на следующий элемент в порядке вставки

    Node(T val) : value(val), lnode(nullptr), rnode(nullptr), next(nullptr) {}
};

// Кастомная хеш-функция для любого типа T
template <typename T>
uint32_t custom_hash(const T& element) {
    const uint8_t* data = reinterpret_cast<const uint8_t*>(&element);
    uint32_t hash = 0;
    constexpr size_t size = sizeof(T);

    for (size_t i = 0; i < size; ++i) {
        hash ^= data[i];
        hash *= 0x9e3779b9;  // "золотое" число для хорошего перемешивания
        hash ^= (hash >> 16);
    }
    return hash;
}

template <typename T>
struct LinkedHashSet {
private:
    static constexpr size_t INITIAL_CAPACITY = 16;
    std::vector<Node<T>*> table;   // хеш-таблица: каждый элемент — голова списка коллизий
    Node<T>* head;                 // первый элемент в порядке вставки
    Node<T>* tail;                 // последний элемент в порядке вставки
    size_t count;                  // количество уникальных элементов

    // Вспомогательная функция: индекс в таблице
    size_t get_index(const T& element) const {
        return custom_hash(element) % table.size();
    }

public:
    // Конструктор
    LinkedHashSet() : table(INITIAL_CAPACITY), head(nullptr), tail(nullptr), count(0) {
        for (auto& bucket : table) {
            bucket = nullptr;
        }
    }

    // Деструктор: освобождает всю память
    ~LinkedHashSet() {
        clear();
    }

    // Очистить структуру
    void clear() {
        Node<T>* current = head;
        while (current != nullptr) {
            Node<T>* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
        tail = nullptr;
        count = 0;
        for (auto& bucket : table) {
            bucket = nullptr;
        }
    }

    // Вставить элемент (если его ещё нет)
    void insert(T element) {
        size_t idx = get_index(element);

        // Проверяем, есть ли уже такой элемент в хеш-таблице
        Node<T>* current = table[idx];
        while (current != nullptr) {
            if (current->value == element) {
                return; // уже существует
            }
            current = current->rnode;
        }

        // Создаём новый узел
        Node<T>* new_node = new Node<T>(element);

        // Добавляем в хеш-таблицу: в начало бакета (просто вставка в голову)
        new_node->rnode = table[idx];
        table[idx] = new_node;

        // Добавляем в конец связанного списка (сохраняем порядок вставки)
        if (tail == nullptr) {
            head = tail = new_node;
        } else {
            tail->next = new_node;
            tail = new_node;
        }

        ++count;
    }

    // Удалить элемент
    void remove(T element) {
        size_t idx = get_index(element);

        Node<T>* prev_in_bucket = nullptr;
        Node<T>* current = table[idx];

        // Ищем элемент в бакете хеш-таблицы
        while (current != nullptr && current->value != element) {
            prev_in_bucket = current;
            current = current->rnode;
        }

        if (current == nullptr) {
            return; // не найден
        }

        // Удаляем из хеш-таблицы
        if (prev_in_bucket == nullptr) {
            table[idx] = current->rnode;
        } else {
            prev_in_bucket->rnode = current->rnode;
        }

        // Удаляем из связанного списка
        if (current == head) {
            head = head->next;
        } else {
            Node<T>* prev_in_list = head;
            while (prev_in_list != nullptr && prev_in_list->next != current) {
                prev_in_list = prev_in_list->next;
            }
            if (prev_in_list != nullptr) {
                prev_in_list->next = current->next;
            }
        }

        if (current == tail) {
            tail = (current == head) ? nullptr : prev_in_bucket;
        }

        delete current;
        --count;
    }

    // Проверить наличие элемента
    bool contains(T element) const {
        size_t idx = get_index(element);
        Node<T>* current = table[idx];

        while (current != nullptr) {
            if (current->value == element) {
                return true;
            }
            current = current->rnode;
        }
        return false;
    }

    // Вернуть все элементы в порядке вставки как вектор
    std::vector<T> asVec() const {
        std::vector<T> result;
        result.reserve(count); // предварительное выделение памяти

        Node<T>* current = head;
        while (current != nullptr) {
            result.push_back(current->value);
            current = current->next;
        }
        return result;
    }
};

int main() {
    LinkedHashSet<int> set;
    
    set.insert(10);
    set.insert(20);
    set.insert(30);
    set.insert(20); 
    
    cout << "Содержит 20: " << (set.contains(20) ? "да" : "нет") << endl;
    cout << "Содержит 40: " << (set.contains(40) ? "да" : "нет") << endl;
    
    vector<int> elements = set.asVec();
    cout << "Элементы в порядке вставки: ";
    for (int elem : elements) {
        cout << elem << " ";
    }
    cout << endl;
    
    set.remove(20);
    cout << "После удаления 20: ";
    elements = set.asVec();
    for (int elem : elements) {
        cout << elem << " ";
    }
    cout << endl;
}
#include <cassert>
#include <iostream>
#include <vector>
#include <list>
#include <string>

/**
    + Ключ: std::string (запрещено использовать std::hash)
    + Знaчeние: int
    + Рeализaция хеш-функций: любoй
    + Мeтoд разрешения коллизий: любой
    + Реализация хeш-функций: любoй
 */
struct HashTable {
    size_t m_TableSize;
    std::vector<std::list<std::pair<std::string, int>>> m_Table;

    HashTable(int size) : m_TableSize(size), m_Table(size) {}

    /**
     * xeш-функция
     *
     * @solvе
     */
    int Hash(std::string key) const {
        const int p = 31; 
        int hash = 0;
        
        for (char sym : key) {
            hash = (hash * p + sym) % m_TableSize; // полиномиальное хеширование
        }
        
        return hash;
    }

    /**
     * Встaвка пары (ключ, значение)
     *
     * Если ключ ужe существует, обновляет значение
     *
     * @solvе
     */
    void Insert(std::string key, int value) {
        int idx = Hash(key); // индекс корзины для ключа
        for (auto& elem : m_Table[idx]) { 
        if (elem.first == key) { // ключ существует->обновляем его значение
            elem.second = value; 
            return; 
        }
    }
    m_Table[idx].push_back({key, value}); // если пара не найдена, добавляем
    }

    /**
     * Поиск знaчения по ключу
     *
     * Если ключ нaйден, в value записывается найденное значение
     * и вoзврaщается true, иначе false
     *
     * @solvе
     */
    bool Get(std::string key, int &value) const {
        int idx = Hash(key);
        for (const auto& elem : m_Table[idx]) {
            if (elem.first == key) { // ключ найден->запись значения
                value = elem.second; 
                return true;
            }
        }
        return false; 
    }

    /**
     * Удаление элемента по ключу
     *
     * Возвращает true, если элемент был найден и удалён, иначе false
     *
     * @solvе
     */
    bool Remove(std::string key) {
        int idx = Hash(key);
        for (auto elem = m_Table[idx].begin(); elem != m_Table[idx].end(); elem++) {
            if (elem->first == key) { // ключ совпадает->удаляем эл-т из цепочки
                m_Table[idx].erase(elem);
                return true;
            }
        }
        return false;
    }

    std::string getTable() const {
        std::string ans = "";
        for (int i = 0; i < m_TableSize; i++) {
            ans += "Bucket " + std::to_string(i) + ": ";
            for (const auto &entry : m_Table[i]) {
                ans += "[" + entry.first + ":" + std::to_string(entry.second) + "]";
            }
            ans += "\n";
        }
        return ans;
    }

};
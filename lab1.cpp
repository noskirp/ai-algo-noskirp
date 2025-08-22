#include <iostream>
#include <queue>
#include <algorithm>

// Структура узла дерева
struct TreeNode {
    int value;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int val) : value(val), left(nullptr), right(nullptr) {}
};

// Функция для вставки значения в бинарное дерево поиска
TreeNode* insert(TreeNode* root, int value) {
    if (root == nullptr) {
        return new TreeNode(value);
    }
    if (value < root->value) {
        root->left = insert(root->left, value);
    } else {
        root->right = insert(root->right, value);
    }
    return root;
}

// Рекурсивная функция для отзеркаливания дерева
void mirror(TreeNode* root) {
    if (root == nullptr) return;
    std::swap(root->left, root->right);
    mirror(root->left);
    mirror(root->right);
}

// Функция для вывода дерева используя BFS (обход в ширину)
void printBFS(TreeNode* root) {
    if (root == nullptr) return;
    std::queue<TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        TreeNode* current = q.front();
        q.pop();
        std::cout << current->value << " ";
        if (current->left != nullptr) q.push(current->left);
        if (current->right != nullptr) q.push(current->right);
    }
    std::cout << std::endl;
}

int main() {
    TreeNode* root = nullptr;
    
    // Вставляем значения в дерево
    root = insert(root, 5);
    insert(root, 3);
    insert(root, 7);
    insert(root, 2);
    insert(root, 4);
    insert(root, 6);
    insert(root, 8);
    
    std::cout << "Исходное дерево (BFS): ";
    printBFS(root);
    
    mirror(root);
    
    std::cout << "После отзеркаливания (BFS): ";
    printBFS(root);
    
    return 0;
}
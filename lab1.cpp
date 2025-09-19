#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <climits>
#include <exception>
using namespace std;

struct TreeNode {
    int id;
    double x, y;
    TreeNode* left;
    TreeNode* right;

    TreeNode(int id_val, double x_val, double y_val)
        : id(id_val), x(x_val), y(y_val), left(nullptr), right(nullptr) {}
};

class TreeBuilder {
public:
    static TreeNode* buildTree(vector<string>& tokens, int& index) {
        if (index >= tokens.size()) return nullptr;
        if (tokens[index] == "#") {
            index++;
            return nullptr;
        }
        
        int id = stoi(tokens[index++]);
        double x = stod(tokens[index++]);
        double y = stod(tokens[index++]);

        TreeNode* node = new TreeNode(id, x, y);
        node->left = buildTree(tokens, index);
        node->right = buildTree(tokens, index);
        return node;
    }
};


class TreeAnalyzer {
public:
    static int countNodes(TreeNode* root) {
    if (root == nullptr) {
        return 0;
    } else {
        return 1 + countNodes(root->left) + countNodes(root->right);
    }
    }

    static int findMinID(TreeNode* root) {
        if (!root) return INT_MAX;
        while (root->left) root = root->left;  
        return root->id;
}

    static int findMaxID(TreeNode* root) {
        if (!root) return INT_MIN;
        while (root->right) root = root->right;
        return root->id;
    }

    static int getHeight(TreeNode* root) {
        return root ? 1 + max(getHeight(root->left), getHeight(root->right)) : 0;
    }

    static int countLeaves(TreeNode* root) {
        if (!root) return 0;
        if (!root->left && !root->right) return 1;
        return countLeaves(root->left) + countLeaves(root->right);
    }

    static bool isValidBST(TreeNode* root, long long minVal = LLONG_MIN, long long maxVal = LLONG_MAX) {
        if (!root) return true;
        if (root->id <= minVal || root->id >= maxVal) return false;
        return isValidBST(root->left, minVal, root->id) && isValidBST(root->right, root->id, maxVal);
    }
};

class TreeProcessor {
    TreeNode* root;

    void deleteTree(TreeNode* node) {
        if (!node) return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }

public:
    TreeProcessor(const string& input) : root(nullptr) {
        vector<string> tokens;
        stringstream ss(input);
        string token;
        
        while (ss >> token) {
            tokens.push_back(token);
        }

        int index = 0;
        root = TreeBuilder::buildTree(tokens, index);
    }

    ~TreeProcessor() {
        deleteTree(root);
    }
    void printNode(TreeNode* node) {
        if (!node) {
            cout << "#";
            return;
        }
        cout << node->id;
    }
    void printPreorder(TreeNode* node) {
        if (!node) {
            cout << "#" << " ";
            return;
        }
        cout << node->id << " ";
        printPreorder(node->left);
        printPreorder(node->right);
    }

    void analyze() {
        if (!root) {
            cout << "Общее количество узлов: 0\nМинимальный ID: 0\nМаксимальный ID: 0\n"
                 << "Высота дерева: 0\nКоличество листьев: 0\n";
            return;
        }


        cout << "Общее количество узлов: " << TreeAnalyzer::countNodes(root) << "\n"
             << "Минимальный ID: " << TreeAnalyzer::findMinID(root) << "\n"
             << "Максимальный ID: " << TreeAnalyzer::findMaxID(root) << "\n"
             << "Высота дерева: " << TreeAnalyzer::getHeight(root) << "\n"
             << "Количество листьев: " << TreeAnalyzer::countLeaves(root) << "\n";
        cout << "Структура дерева (preorder): ";
        printPreorder(root);
        cout << "\n";
    }
};

class TestGenerator {
    static void buildPreorder(const vector<int>& ids, int start, int end, vector<string>& preorder) {
        if (start > end) {
            preorder.push_back("#");
            return;
        }

        int mid = (start + end) / 2;
        preorder.push_back(to_string(ids[mid]));
        preorder.push_back(to_string(ids[mid] * 0.1));
        preorder.push_back(to_string(ids[mid] * 0.2));

        buildPreorder(ids, start, mid - 1, preorder);
        buildPreorder(ids, mid + 1, end, preorder);
    }

public:
    static string generateValidBST(int n) {
        if (n == 0) return "#";
        
        vector<int> ids;
        for (int i = 1; i <= n; i++) ids.push_back(i * 10);
        
        vector<string> preorder;
        buildPreorder(ids, 0, ids.size() - 1, preorder);
        
        string result;
        for (const auto& token : preorder) {
            if (!result.empty()) result += " ";
            result += token;
        }
        return result;
    }

    static string generateInvalidBST(int n) {
        string valid = generateValidBST(n);
        vector<string> tokens;
        stringstream ss(valid);
        string token;
        
        while (ss >> token) tokens.push_back(token);

        if (tokens.size() < 6) return valid;

        vector<int> nodeIndices;
        for (int i = 0; i < tokens.size(); i += 3) {
            if (tokens[i] != "#") nodeIndices.push_back(i);
        }

        if (nodeIndices.size() < 2) return valid;

        swap(tokens[nodeIndices[0]], tokens[nodeIndices.back()]);
        
        string result;
        for (const auto& t : tokens) {
            if (!result.empty()) result += " ";
            result += t;
        }
        return result;
    }
};

void runTests() {
    cout << "ТЕСТЫ\n\n";

    vector<pair<string, string>> tests = {
        {"Пример из условия", "10 12.3 45.6 5 3.1 4.4 # # 8 0.0 1.0 # 20 50.1 50.2 # #"},
        {"Пустое дерево", "#"},
        {"Один узел", "5 1.0 2.0 # #"},
        {"Некорректное BST", "10 12.3 45.6 5 3.1 4.4 8 0.0 1.0 # # # 20 50.1 50.2 # # "}
    };

    for (int i = 0; i < tests.size(); i++) {
        cout << "Тест " << i+1 << " (" << tests[i].first << "): ";
        try {
            TreeProcessor processor(tests[i].second);
            processor.analyze();
            cout << "успешно\n\n";
        } catch (const exception& e) {
            cout << "ошибка" << e.what() << "\n\n";
        }
    }

    // Автогенерированные тесты
    for (int i = 5; i <= 7; i++) {
        string input;
        string desc;
        
        if (i == 5) {
            input = TestGenerator::generateValidBST(10);
            desc = "Автогенерация 10 узлов";
        } else if (i == 6) {
            input = TestGenerator::generateInvalidBST(7);
            desc = "Некорректное автогенерированное";
        } else {
            input = TestGenerator::generateValidBST(100);
            desc = "Глубокое дерево 100 узлов";
        }

        cout << "Тест " << i << " (" << desc << "): ";
        try {
            TreeProcessor processor(input);
            processor.analyze();
            cout << (i == 6 ? "Должна быть ошибка!\n\n" : "Успешно\n\n");
        } catch (const exception& e) {
            cout << (i == 6 ? "верно" : "неверно ") << e.what() << "\n\n";
        }
    }

    cout << "ЕСТЫ ЗАВЕРШЕНЫ\n";
}

int main() {
    string input;
    if (getline(cin, input) && input.empty()) {
    runTests();
    } else {
    TreeProcessor processor(input);
    processor.analyze();
}
return 0;
}
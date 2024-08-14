#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <map>

using namespace std;

struct Node {
    vector<char> A;
    Node* left;
    Node* right;
    int height;
};

int countVowels(const vector<char>& A) {
    int count = 0;
    for (char c : A) {
        if (c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U') {
            count++;
        }
    }
    return count;
}

int mostFrequentCharCount(const vector<char>& A) {
    map<char, int> freq;
    for (char c : A) {
        freq[c]++;
    }
    int maxCount = 0;
    for (const auto& pair : freq) {
        maxCount = max(maxCount, pair.second);
    }
    return maxCount;
}

bool compareKeys(const vector<char>& A, const vector<char>& B) {
    int vowelsA = countVowels(A);
    int vowelsB = countVowels(B);
    if (vowelsA != vowelsB) {
        return vowelsA < vowelsB;
    }
    int freqA = mostFrequentCharCount(A);
    int freqB = mostFrequentCharCount(B);
    return freqA < freqB;
}

int height(Node* N) {
    return (N == nullptr) ? 0 : N->height;
}

Node* newNode(const vector<char>& key) {
    Node* node = new Node();
    node->A = key;
    node->left = node->right = nullptr;
    node->height = 1;
    return node;
}

Node* rightRotate(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
    return x;
}

Node* leftRotate(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    return y;
}

int getBalance(Node* N) {
    return (N == nullptr) ? 0 : height(N->left) - height(N->right);
}

Node* insert(Node* node, const vector<char>& key) {
    if (node == nullptr) {
        return newNode(key);
    }
    if (compareKeys(key, node->A)) {
        node->left = insert(node->left, key);
    } else if (compareKeys(node->A, key)) {
        node->right = insert(node->right, key);
    } else {
        return node;
    }
    node->height = 1 + max(height(node->left), height(node->right));
    int balance = getBalance(node);
    if (balance > 1 && compareKeys(key, node->left->A)) {
        return rightRotate(node);
    }
    if (balance < -1 && compareKeys(node->right->A, key)) {
        return leftRotate(node);
    }
    if (balance > 1 && compareKeys(node->left->A, key)) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && compareKeys(key, node->right->A)) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}

Node* minValueNode(Node* node) {
    Node* current = node;
    while (current->right != nullptr) {
        current = current->right;
    }
    return current;
}

Node* deleteNode(Node* root, const vector<char>& key) {
    if (root == nullptr) {
        return root;
    }
    if (compareKeys(key, root->A)) {
        root->left = deleteNode(root->left, key);
    } else if (compareKeys(root->A, key)) {
        root->right = deleteNode(root->right, key);
    } else {
        if ((root->left == nullptr) || (root->right == nullptr)) {
            Node* temp = root->left ? root->left : root->right;
            if (temp == nullptr) {
                temp = root;
                root = nullptr;
            } else {
                *root = *temp;
            }
            delete temp;
        } else {
            Node* temp = minValueNode(root->left);
            root->A = temp->A;
            root->left = deleteNode(root->left, temp->A);
        }
    }
    if (root == nullptr) {
        return root;
    }
    root->height = 1 + max(height(root->left), height(root->right));
    int balance = getBalance(root);
    if (balance > 1 && getBalance(root->left) >= 0) {
        return rightRotate(root);
    }
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    if (balance < -1 && getBalance(root->right) <= 0) {
        return leftRotate(root);
    }
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }
    return root;
}

void inOrder(Node* root) {
    if (root != nullptr) {
        inOrder(root->left);
        for (char c : root->A) {
            cout << c;
        }
        cout << " ";
        inOrder(root->right);
    }
}

void deleteNodesWithTwoVowels(Node*& root) {
    if (root == nullptr) {
        return;
    }
    deleteNodesWithTwoVowels(root->left);
    deleteNodesWithTwoVowels(root->right);
    if (countVowels(root->A) == 2) {
        root = deleteNode(root, root->A);
    }
}

int main() {
    ifstream file("data.txt");
    if (!file) {
        cerr << "Unable to open file data.txt";
        return 1;
    }

    Node* root = nullptr;
    string line;
    while (getline(file, line)) {
        vector<char> key(line.begin(), line.end());
        root = insert(root, key);
    }
    file.close();

    cout << "In-order traversal of the constructed AVL tree is: ";
    inOrder(root);
    cout << endl;

    deleteNodesWithTwoVowels(root);

    cout << "In-order traversal after deleting nodes with exactly 2 vowels: ";
    inOrder(root);
    cout << endl;

    return 0;
}
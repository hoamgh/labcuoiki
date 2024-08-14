#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

struct Time {
    int hour, minute;
};

struct Flight {
    string destination;
    string arrival;
    string plane;
    int eco, business;
    Time duration;
};

struct Node {
    Flight flight;
    Node* pLeft;
    Node* pRight;
    int height = 1;
};

Node* createNode(Flight info) {
    Node* result = new Node;
    result->flight = info;
    result->pLeft = result->pRight = nullptr;
    result->height = 1; // New node is initially added at leaf
    return result;
}

int getHeight(Node* node) {
    return node ? node->height : 0;
}

int getBalance(Node* node) {
    return node ? getHeight(node->pLeft) - getHeight(node->pRight) : 0;
}

Node* rightRotate(Node* y) {
    Node* x = y->pLeft;
    Node* T2 = x->pRight;

    x->pRight = y;
    y->pLeft = T2;

    y->height = max(getHeight(y->pLeft), getHeight(y->pRight)) + 1;
    x->height = max(getHeight(x->pLeft), getHeight(x->pRight)) + 1;

    return x;
}

Node* leftRotate(Node* x) {
    Node* y = x->pRight;
    Node* T2 = y->pLeft;

    y->pLeft = x;
    x->pRight = T2;

    x->height = max(getHeight(x->pLeft), getHeight(x->pRight)) + 1;
    y->height = max(getHeight(y->pLeft), getHeight(y->pRight)) + 1;

    return y;
}

void insertNode(Node*& node, Flight info) {
    if (node == nullptr) {
        node = createNode(info);
        return;
    }

    if (info.eco < node->flight.eco) {
        insertNode(node->pLeft, info);
    } else if (info.eco > node->flight.eco) {
        insertNode(node->pRight, info);
    } else {
        // Nếu eco bằng nhau, so sánh theo arrival
        if (info.arrival < node->flight.arrival) {
            insertNode(node->pLeft, info);
        } else if (info.arrival > node->flight.arrival) {
            insertNode(node->pRight, info);
        } else {
            return; // Nếu eco và arrival đều bằng nhau, không chèn (tránh trùng lặp hoàn toàn)
        }
    }

    node->height = 1 + max(getHeight(node->pLeft), getHeight(node->pRight));

    int balance = getBalance(node);

    if (balance > 1 && (info.eco < node->pLeft->flight.eco || (info.eco == node->pLeft->flight.eco && info.arrival < node->pLeft->flight.arrival))) {
        node = rightRotate(node);
    } else if (balance < -1 && (info.eco > node->pRight->flight.eco || (info.eco == node->pRight->flight.eco && info.arrival > node->pRight->flight.arrival))) {
        node = leftRotate(node);
    } else if (balance > 1 && (info.eco > node->pLeft->flight.eco || (info.eco == node->pLeft->flight.eco && info.arrival > node->pLeft->flight.arrival))) {
        node->pLeft = leftRotate(node->pLeft);
        node = rightRotate(node);
    } else if (balance < -1 && (info.eco < node->pRight->flight.eco || (info.eco == node->pRight->flight.eco && info.arrival < node->pRight->flight.arrival))) {
        node->pRight = rightRotate(node->pRight);
        node = leftRotate(node);
    }
}

string formatLine(string line) {
    int pos;
    while ((pos = line.find('{')) != string::npos) {
        line.erase(pos, 1);
    }
    while ((pos = line.find('}')) != string::npos) {
        line.erase(pos, 1);
    }
    while ((pos = line.find('"')) != string::npos) {
        line.erase(pos, 1);
    }
    while ((pos = line.find('[')) != string::npos) {
        line.erase(pos, 1);
    }
    while ((pos = line.find(']')) != string::npos) {
        line.erase(pos, 1);
    }
    while ((pos = line.find(':')) != string::npos) {
        line.replace(pos, 1, ",");
    }
    return line;
}
Flight getFlight(string line) {
    Flight flight;
    line = formatLine(line);  // Clean the line using formatLine
    stringstream ss(line);

    getline(ss, flight.arrival, ',');
    getline(ss, flight.destination, ',');
    getline(ss, flight.plane, ',');

    string seat;
    getline(ss, seat, ',');
    stringstream s(seat);
    s.ignore(); // Ignore the first character if it's an identifier
    s >> flight.business;

    getline(ss, seat, ',');
    stringstream s_(seat);
    s_.ignore(); // Ignore the first character if it's an identifier
    s_ >> flight.eco;

    
    string time;
    getline(ss, time, ',');
    if (time.find("hours") == string::npos) {
        stringstream sTime(time);
        sTime >> flight.duration.minute;
    }

    if (time.find("minutes") == string::npos) {
        stringstream sTime(time);
        sTime >> flight.duration.hour;
    }

    stringstream sTime(time);
    sTime >> flight.duration.hour;
    getline(ss, time);
    stringstream sMinute(time);
    sMinute >> flight.duration.minute;

    return flight;
}
vector<Flight> readFileForAVL(string fileName) {
    fstream fs;
    vector<Flight> data;
    fs.open(fileName);
    if (fs) {
        string line;
        while (getline(fs, line)) {
            Flight result;
            result = getFlight(line);
            data.push_back(result);
        }
        fs.close();
    } else {
        cout << "Cannot open file" << endl;
        return data;
    }
    return data;
}

Node* createTree(vector<Flight> data) {
    Node* result = nullptr;
    for (int i = 0; i < data.size(); i++) {
        insertNode(result, data[i]);
    }
    return result;
}

void inOrderTraversal(Node* root) {
    if (root == nullptr) {
        return;
    }
    inOrderTraversal(root->pLeft);
    cout << "Eco: " << root->flight.eco 
         << ", Arrival: " << root->flight.arrival 
         << ", Destination: " << root->flight.destination 
         << ", Plane: " << root->flight.plane 
         << ", Business: " << root->flight.business 
         << ", Duration: " << root->flight.duration.hour << "h " << root->flight.duration.minute << "m" 
         << endl;
    inOrderTraversal(root->pRight);
}

int treeSize(Node* root) {
    if (root == nullptr) return 0;
    return 1 + treeSize(root->pRight) + treeSize(root->pLeft);
}

int main() {
    string fileName = "data1.txt";
    vector<Flight> data = readFileForAVL(fileName);

    cout << "Total flights read from file: " << data.size() << endl;
    Node* root = createTree(data);

    if (root == nullptr) {
        cout << "Tree creation failed." << endl;
        return 1;
    }

    cout << "Duyệt cây theo thứ tự LNR: " << endl;
    inOrderTraversal(root);
    cout << endl;

    cout << "Số lượng node trong cây: " << treeSize(root) << endl;

    return 0;
}
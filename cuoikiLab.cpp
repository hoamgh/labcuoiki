#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <queue>
#include <unordered_map>
#include<algorithm>
using namespace std;

struct Time {
    int hour;
    int minute;
};

struct Flight {
    string plane;
    int eco, business;
    Time duration;
};

struct Graph {
    vector<vector<Flight>> flight;
    vector<string> nation;
    map<string, int> matrix;
};


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
        line.replace(pos,    1, ",");
    }
    return line;
}

Flight getFilght(string line) {
    Flight flight;
    string emptyLine, infor;
    stringstream ss(line);

    getline(ss, emptyLine, ',');
    getline(ss, emptyLine, ',');

    getline(ss, flight.plane, ',');

    string seat;
    getline(ss, seat, ',');
    stringstream s(seat);
    s.ignore();
    s >> flight.business;
    getline(ss, seat, ',');
    stringstream s_(seat);
    s_.ignore();
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
bool checkExist(vector<string> v, string nation) {
    for (int i = 0; i < v.size(); i++) {
        if (nation == v[i]) {
            return true;
        }
    }
    return false;
}

Graph readfile(string filename) {
    Graph map;
    ifstream fi;
    fi.open(filename);
    if (fi) {
        string infor;
        while (getline(fi, infor)) {
            string departure, arrival;
            string line = formatLine(infor);
            stringstream ss(line);

            getline(ss, departure, ',');
            if (!checkExist(map.nation, departure)) {
                map.nation.push_back(departure);
                map.matrix[departure] = map.nation.size() - 1;
            }

            getline(ss, arrival, ',');
            if (!checkExist(map.nation, arrival)) {
                map.nation.push_back(arrival);
                map.matrix[arrival] = map.nation.size() - 1;
            }

            // Resize the flight vector to a square matrix
            if (map.flight.size() < map.nation.size()) {
                map.flight.resize(map.nation.size());
                for (int i = 0; i < map.nation.size(); ++i) {
                    map.flight[i].resize(map.nation.size());
                }
            }

            map.flight[map.matrix[departure]][map.matrix[arrival]] = getFilght(line);
            map.flight[map.matrix[arrival]][map.matrix[departure]] = map.flight[map.matrix[departure]][map.matrix[arrival]];
        }
    } else {
        cout << "File Error!" << endl;
    }
    fi.close();
    return map;
}

bool isDirected(const Graph& graph) {
    int n = graph.flight.size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (graph.flight[i][j].plane != graph.flight[j][i].plane ||
                graph.flight[i][j].duration.hour != graph.flight[j][i].duration.hour ||
                graph.flight[i][j].duration.minute != graph.flight[j][i].duration.minute) {
                return true;
            }
        }
    }
    return false;
}

bool check(Graph FL, string nation1, string nation2) {
    if (nation1 == nation2) return false;
    int from = FL.matrix[nation1];
    int to = FL.matrix[nation2];
    return !(FL.flight[from][to].plane.empty());
}


vector<int> calculateDegreeDirected(const Graph& graph) {
    int n = graph.flight.size();
    vector<int> inDegree(n, 0);
    vector<int> outDegree(n, 0);

    for (int u = 0; u < n; ++u) {
        for (int v = 0; v < n; ++v) {
            if (!graph.flight[u][v].plane.empty()) {
                outDegree[u]++; // Out-degree
                inDegree[v]++;  // In-degree
            }
        }
    }

    vector<int> degree(n, 0);
    for (int i = 0; i < n; ++i) {
        degree[i] = inDegree[i] + outDegree[i];
    }

    return degree;
}

vector<int> calculateDegreeUndirected(const Graph& graph) {
    int n = graph.flight.size();
    vector<int> degree(n, 0);

    for (int u = 0; u < n; ++u) {
        for (int v = 0; v < n; ++v) {
            if (!graph.flight[u][v].plane.empty()) {
                degree[u]++;
                degree[v]++;
            }
        }
    }

    return degree;
}

vector<int> calculateDegree(const Graph& graph) {
    if (isDirected(graph)) {
        return calculateDegreeDirected(graph);
    } else {
        return calculateDegreeUndirected(graph);
    }
}

int countListEdges(Graph graph) {
    int count = 0;
    int n = graph.flight.size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (!graph.flight[i][j].plane.empty()) {
                count++;
            }
        }
    }
    if (!isDirected(graph)) {
        count /= 2;
    }
    return count;
}
vector<string> getIsolatedNations(const Graph& graph) {
    int n = graph.flight.size();
    vector<string> isolatedNations;

    for (int i = 0; i < n; ++i) {
        bool isIsolated = true;
        for (int j = 0; j < n; ++j) {
            if (!graph.flight[i][j].plane.empty() || !graph.flight[j][i].plane.empty()) {
                isIsolated = false;
                break;
            }
        }
        if (isIsolated) {
            isolatedNations.push_back(graph.nation[i]);
        }
    }

    return isolatedNations;
}
vector<int> getIsolatedNationsIndex(const Graph& graph) {
    int n = graph.flight.size();
    vector<int> isolatedNations;

    for (int i = 0; i < n; ++i) {
        bool isIsolated = true;
        for (int j = 0; j < n; ++j) {
            if (!graph.flight[i][j].plane.empty() || !graph.flight[j][i].plane.empty()) {
                isIsolated = false;
                break;
            }
        }
        if (isIsolated) {
            isolatedNations.push_back(i);
        }
    }

    return isolatedNations;
}
vector<string> countLeafVertices(const Graph& graph) {
    int n = graph.flight.size();
    vector<string> leafVertices;

    for (int i = 0; i < n; ++i) {
        int edgeCount = 0;
        for (int j = 0; j < n; ++j) {
            if (!graph.flight[i][j].plane.empty() || !graph.flight[j][i].plane.empty()) {
                edgeCount++;
            }
        }
        if (edgeCount == 1) {
            leafVertices.push_back(graph.nation[i]);
        }
    }

    return leafVertices;
}
vector<int> countLeafVerticesindex(const Graph& graph) {
    int n = graph.flight.size();
    vector<int> leafVertices;

    for (int i = 0; i < n; ++i) {
        int edgeCount = 0;
        for (int j = 0; j < n; ++j) {
            if (!graph.flight[i][j].plane.empty() || !graph.flight[j][i].plane.empty()) {
                edgeCount++;
            }
        }
        if (edgeCount == 1) {
            leafVertices.push_back(i);
        }
    }

    return leafVertices;
}
bool checkPath(Graph& graph, const string& nation1, const string& nation2, int maxTime, vector<string>& path) {
    if (graph.matrix.find(nation1) == graph.matrix.end() || graph.matrix.find(nation2) == graph.matrix.end()) {
        return false; // Kiểm tra xem quốc gia có tồn tại trong đồ thị hay không
    }

    int start = graph.matrix[nation1];
    int end = graph.matrix[nation2];

    vector<bool> visited(graph.nation.size(), false);
    queue<pair<int, int>> q; // pair of (current index, accumulated time in minutes)
    q.push({start, 0});
    visited[start] = true;

    vector<int> parent(graph.nation.size(), -1); // để theo dõi đường đi

    while (!q.empty()) {
        auto [current, currentTime] = q.front();
        q.pop();

        if (current == end && currentTime <= maxTime) {
            // Tái tạo đường đi
            int node = end;
            while (node != -1) {
                path.push_back(graph.nation[node]);
                node = parent[node];
            }
            reverse(path.begin(), path.end());
            return true;
        }

        for (int i = 0; i < graph.flight[current].size(); ++i) {
            if (!graph.flight[current][i].plane.empty() && !visited[i]) {
                int flightTime = graph.flight[current][i].duration.hour * 60 + graph.flight[current][i].duration.minute;
                int newTime = currentTime + flightTime;

                if (newTime <= maxTime) {
                    q.push({i, newTime});
                    visited[i] = true;
                    parent[i] = current; // Đánh dấu đường đi
                }
            }
        }
    }

    return false;
}
void addFlight(Graph& graph, const string& from, const string& to, const Flight& flight) {
    int fromIndex, toIndex;

    if (graph.matrix.find(from) == graph.matrix.end()) {
        fromIndex = graph.nation.size();
        graph.matrix[from] = fromIndex;
        graph.nation.push_back(from);
        graph.flight.resize(graph.nation.size());
        for (auto& sublist : graph.flight) {
            sublist.resize(graph.nation.size());
        }
    } else {
        fromIndex = graph.matrix[from];
    }

    if (graph.matrix.find(to) == graph.matrix.end()) {
        toIndex = graph.nation.size();
        graph.matrix[to] = toIndex;
        graph.nation.push_back(to);
        graph.flight.resize(graph.nation.size());
        for (auto& sublist : graph.flight) {
            sublist.resize(graph.nation.size());
        }
    } else {
        toIndex = graph.matrix[to];
    }

    graph.flight[fromIndex][toIndex] = flight;
}
void removeFlight(Graph& graph, const string& from, const string& to) {
    if (graph.matrix.find(from) != graph.matrix.end() && graph.matrix.find(to) != graph.matrix.end()) {
        int fromIndex = graph.matrix[from];
        int toIndex = graph.matrix[to];
        graph.flight[fromIndex][toIndex] = Flight();
    }
}

vector<int> bfsShortestPath(const Graph& graph, int start, int end) { // đường đi ngắn nhất
    vector<bool> visited(graph.nation.size(), false);
    vector<int> distance(graph.nation.size(), -1);
    queue<int> q;

    visited[start] = true;
    distance[start] = 0;
    q.push(start);

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        for (int i = 0; i < graph.nation.size(); i++) {
            if (!graph.flight[current][i].plane.empty() && !visited[i]) {
                visited[i] = true;
                distance[i] = distance[current] + 1;
                q.push(i);
                if (i == end) {
                    return distance;
                }
            }
        }
    }

    return distance;
}
int calculateTravelTime(const Graph& graph, int start, int end) {
    vector<int> distance = bfsShortestPath(graph, start, end);
    return distance[end];
}

// Function to find the shortest path visiting all given cities
vector<int> findShortestPathVisitingAllCities(const Graph& graph, const vector<int>& cities) {
    vector<int> bestPath;
    int minTime = INT_MAX;

    vector<int> perm = cities;
    do {
        int currentTime = 0;
        for (int i = 0; i < perm.size() - 1; ++i) {
            currentTime += calculateTravelTime(graph, perm[i], perm[i + 1]);
        }

        if (currentTime < minTime) {
            minTime = currentTime;
            bestPath = perm;
        }
    } while (next_permutation(perm.begin(), perm.end()));

    return bestPath;
}
bool checkPath(const Graph& graph, const vector<int>& path, const vector<int>& destinations) {
    for (int dest : destinations) {
        bool found = false;
        for (int node : path) {
            if (node == dest) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

void bfs(Graph& graph, int start, int end) {
    vector<bool> visited(graph.nation.size(), false);
    vector<int> parent(graph.nation.size(), -1);  // To store the parent of each node
    queue<int> q;
    q.push(start);
    visited[start] = true;

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        if (current == end) {
            cout << "Path from " << graph.nation[start] << " to " << graph.nation[end] << " using BFS:" << endl;

            // Reconstruct the path by backtracking through the parent nodes
            vector<int> path;
            for (int v = end; v != -1; v = parent[v]) {
                path.push_back(v);
            }
            reverse(path.begin(), path.end());

            for (int node : path) {
                cout << graph.nation[node] << " ";
            }
            cout << endl;
            return;
        }

        for (int i = 0; i < graph.nation.size(); ++i) {
            if (!graph.flight[current][i].plane.empty() && !visited[i]) {
                q.push(i);
                visited[i] = true;
                parent[i] = current;  // Track the parent of each node
            }
        }
    }

    cout << "No path found from " << graph.nation[start] << " to " << graph.nation[end] << " using BFS." << endl;
}
void dfs(Graph& graph, int start, vector<bool>& visited, vector<int>& path) {
    visited[start] = true;
    path.push_back(start);

    for (int i = 0; i < graph.nation.size(); ++i) {
        if (!graph.flight[start][i].plane.empty() && !visited[i]) {
            dfs(graph, i, visited, path);
        }
    }
}

// Sử dụng hàm dfs
void dfsMain(Graph& graph, int start, int end) {
    vector<bool> visited(graph.nation.size(), false);
    vector<int> path;

    dfs(graph, start, visited, path);

    // Kiểm tra xem đường đi có tồn tại hay không
    bool found = false;
    for (int node : path) {
        if (node == end) {
            found = true;
            break;
        }
    }

    if (found) {
        cout << "Đường đi từ " << graph.nation[start] << " đến " << graph.nation[end] << " bằng dfs:" << endl;
        for (int node : path) {
            cout << graph.nation[node] << " ";
        }
        cout << endl;
    } else {
        cout << "Không tìm thấy đường đi từ " << graph.nation[start] << " đến " << graph.nation[end] << " bằng dfs." << endl;
    }
}
// Function to find the maximum number of cities that can be visited within a given time limit
int calculateTravelTime1(const Graph& graph, int start, int end) {
    if (graph.flight[start][end].plane.empty()) {
        return INT_MAX;
    }
    return graph.flight[start][end].duration.hour * 60 + graph.flight[start][end].duration.minute;
}


vector<int> findMaxCitiesWithinTime(const Graph& graph, int start, int timeLimit) {
    vector<bool> visited(graph.nation.size(), false);
    vector<int> path;
    int currentTime = 0;
    int currentCity = start;

    while (true) {
        visited[currentCity] = true;
        path.push_back(currentCity);

        int nextCity = -1;
        int minTime = INT_MAX;

        for (int i = 0; i < graph.nation.size(); ++i) {
            if (!visited[i]) {
                int travelTime = calculateTravelTime1(graph, currentCity, i);
                if (travelTime < minTime) {
                    minTime = travelTime;
                    nextCity = i;
                }
            }
        }

        if (nextCity == -1 || currentTime + minTime > timeLimit) {
            break;
        }

        currentTime += minTime;
        currentCity = nextCity;
    }

    return path;
}
// đếm các nước có bậc chẵn in ra các nước có bậc chẵn cao nhất
void countEvenDegreeNations(const Graph& graph) {
    vector<int> degrees = calculateDegree(graph);
    int evenCount = 0;
    int highestEvenDegree = 0;
    vector<string> highestEvenDegreeNations;

    for (int i = 0; i < degrees.size(); ++i) {
        if (degrees[i] % 2 == 0) {
            evenCount++;
            if (degrees[i] > highestEvenDegree) {
                highestEvenDegree = degrees[i];
                highestEvenDegreeNations.clear();
                highestEvenDegreeNations.push_back(graph.nation[i]);
            } else if (degrees[i] == highestEvenDegree) {
                highestEvenDegreeNations.push_back(graph.nation[i]);
            }
        }
    }

    cout << evenCount << " even vertice, the highest: ";
    for (const string& nation : highestEvenDegreeNations) {
        cout << nation << " ";
    }
    cout << endl;
}
// in ra các nước có thể đi trực tiếp từ nation 1
void printDirectFlights(const Graph& graph, const string& nation1) {
    if (graph.matrix.find(nation1) == graph.matrix.end()) {
        cout << "Nation not found in the graph." << endl;
        return;
    }

    int nationIndex = graph.matrix.at(nation1);
    int count = 0;

    cout << "Nations with direct flights to " << nation1 << ":" << endl;
    for (int i = 0; i < graph.nation.size(); ++i) {
        if (!graph.flight[i][nationIndex].plane.empty()) {
            cout << graph.nation[i] << endl;
            count++;
            if (count == 10) {
                break;
            }
        }
    }

    if (count == 0) {
        cout << "No direct flights found to " << nation1 << "." << endl;
    }
}

// hàm tìm 4 nước phát triển trong đó có 2 nước cho sẵn
bool hasDirectFlight(const Graph& graph, int from, int to) {
    return !graph.flight[from][to].plane.empty();
}

void findDevelopedRegions(const Graph& graph, const string& nation1, const string& nation2) {
    if (graph.matrix.find(nation1) == graph.matrix.end() || graph.matrix.find(nation2) == graph.matrix.end()) {
        cout << "One or both nations not found in the graph." << endl;
        return;
    }

    int index1 = graph.matrix.at(nation1);
    int index2 = graph.matrix.at(nation2);

    vector<vector<int>> developedRegions;

    for (int i = 0; i < graph.nation.size(); ++i) {
        for (int j = i + 1; j < graph.nation.size(); ++j) {
            for (int k = j + 1; k < graph.nation.size(); ++k) {
                for (int l = k + 1; l < graph.nation.size(); ++l) {
                    vector<int> region = {i, j, k, l};
                    bool isDeveloped = true;

                    for (int m = 0; m < region.size(); ++m) {
                        for (int n = m + 1; n < region.size(); ++n) {
                            if (!hasDirectFlight(graph, region[m], region[n])) {
                                isDeveloped = false;
                                break;
                            }
                        }
                        if (!isDeveloped) break;
                    }

                    if (isDeveloped && (find(region.begin(), region.end(), index1) != region.end()) && 
                        (find(region.begin(), region.end(), index2) != region.end())) {
                        developedRegions.push_back(region);
                    }
                }
            }
        }
    }

    cout << "Total developed regions: " << developedRegions.size() << endl;
    cout << "Examples of developed regions:" << endl;
    for (int i = 0; i < min(3, (int)developedRegions.size()); ++i) {
        for (int index : developedRegions[i]) {
            cout << graph.nation[index] << " ";
        }
        cout << endl;
    }
}
bool isBoeingFlight(const Flight& flight) {
    return flight.plane.find("Boeing") != string::npos;
}

void findDevelopedAreas(const Graph& graph, const string& country) {
    vector<vector<string>> developedAreas;
    int n = graph.nation.size();

    // Check all combinations of 4 nations
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            for (int k = j + 1; k < n; ++k) {
                for (int l = k + 1; l < n; ++l) {
                    vector<string> quad = {graph.nation[i], graph.nation[j], graph.nation[k], graph.nation[l]};
                    bool allConnected = true;
                    int boeingCount = 0;
                    int totalFlights = 0;

                    // Check if all pairs are connected and count Boeing flights
                    for (int m = 0; m < 4; ++m) {
                        for (int p = m + 1; p < 4; ++p) { // Renamed inner loop variable to 'p'
                            if (graph.matrix.find(quad[m]) == graph.matrix.end() ||
                                graph.matrix.find(quad[p]) == graph.matrix.end() ||
                                graph.flight[graph.matrix.at(quad[m])][graph.matrix.at(quad[p])].plane.empty()) {
                                allConnected = false;
                                break;
                            }
                            totalFlights++;
                            if (isBoeingFlight(graph.flight[graph.matrix.at(quad[m])][graph.matrix.at(quad[p])])) {
                                boeingCount++;
                            }
                        }
                        if (!allConnected) break;
                    }

                    if (allConnected && boeingCount >= (2 * totalFlights) / 3) {
                        developedAreas.push_back(quad);
                    }
                }
            }
        }
    }

    cout << developedAreas.size() << " quad" << endl;
    for (int i = 0; i < min(3, (int)developedAreas.size()); ++i) {
        for (const string& nation : developedAreas[i]) {
            cout << nation << ", ";
        }
        cout << endl;
    }
}
int  main() {
    string filename = "data1.txt";
    Graph map = readfile(filename);
    // vector<int> cities={0,1,2,4,5,6};
    // vector<int> temp;
    // temp= findShortestPathVisitingAllCities(map,cities);
    // for(int i = 0; i < cities.size();i++){
    //     cout<<map.nation[cities[i]]<<" ";
    // }
    // cout<<endl<<endl;
    // for(int i = 0; i < temp.size();i++){
    //     cout<<map.nation[temp[i]]<<" ";
    // }
    // return 0;
    //     int startCity = 2; // Starting city index
    // int timeLimit = 900; // Time limit in minutes (e.g., 10 hours)

    // vector<int> maxCitiesPath = findMaxCitiesWithinTime(map, startCity, timeLimit);

    // cout << "Cities visited within time limit: ";
    // for (int city : maxCitiesPath) {
    //     cout << map.nation[city] << " ";
    // }
    // cout << endl;
    // printDirectFlights(map,"Vietnam");
    // countEvenDegreeNations(map);
    findDevelopedAreas(map,"Vietnam");
    return 0;
}
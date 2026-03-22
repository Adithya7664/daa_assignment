#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <chrono>

using namespace std;

struct Structure {
    vector<double> BC;
    double time;
    size_t size;
};

Structure betweennessCentrality(int n, vector<vector<int>> &adj) {
    vector<double> CB(n, 0.0);

    auto start = chrono::high_resolution_clock::now();

    for (int s = 0; s < n; s++) {
        stack<int> S;
        vector<vector<int>> P(n);
        vector<int> sigma(n, 0);
        vector<int> dist(n, -1);

        sigma[s] = 1;
        dist[s] = 0;

        queue<int> Q;
        Q.push(s);

        // BFS
        while (!Q.empty()) {
            int v = Q.front();
            Q.pop();
            S.push(v);

            for (int w : adj[v]) {
                if (dist[w] < 0) {
                    Q.push(w);
                    dist[w] = dist[v] + 1;
                }
                if (dist[w] == dist[v] + 1) {
                    sigma[w] += sigma[v];
                    P[w].push_back(v);
                }
            }
        }

        vector<double> delta(n, 0.0);

        // Back-propagation
        while (!S.empty()) {
            int w = S.top();
            S.pop();

            for (int v : P[w]) {
                delta[v] += ((double)sigma[v] / sigma[w]) * (1.0 + delta[w]);
            }

            if (w != s) {
                CB[w] += delta[w];
            }
        }
    }

    // divide by 2 for undirected graph
    for (int i = 0; i < n; i++) {
        CB[i] /= 2.0;
    }

    auto end = chrono::high_resolution_clock::now();
    double execT = chrono::duration<double>(end - start).count();

    size_t total_memory = 0;
    for (auto &v : adj) {
        total_memory += v.size() * sizeof(int);
    }
    total_memory += n * sizeof(int) * 2;
    total_memory += n * sizeof(double) * 2;

    return {CB, execT, total_memory};
}

int main(int argc, char *argv[]) {

    ios::sync_with_stdio(false);
    cin.tie(NULL);

    if (argc < 2) {
        cout << "Usage: ./bc <dataset>\n";
        return 1;
    }

    ifstream in(argv[1]);
    if (!in) {
        cout << "File not opened\n";
        return 1;
    }

    unordered_map<int, int> idMap;
    vector<int> reverseMap;
    vector<pair<int, int>> edges;

    string line;
    int idx = 0;

    // Read graph
    while (getline(in, line)) {
        if (line.empty() || line[0] == '#')
            continue;

        int u, v;
        if (sscanf(line.c_str(), "%d %d", &u, &v) != 2)
            continue;

        if (!idMap.count(u)) {
            idMap[u] = idx++;
            reverseMap.push_back(u);
        }

        if (!idMap.count(v)) {
            idMap[v] = idx++;
            reverseMap.push_back(v);
        }

        int um = idMap[u];
        int vm = idMap[v];

        edges.push_back({um, vm});
    }

    int n = idMap.size();

    vector<vector<int>> adj(n);

    // UNDIRECTED graph
    for (auto &e : edges) {
        int u = e.first;
        int v = e.second;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    cout << "Nodes: " << n << ", Edges: " << edges.size() << endl;

    Structure result = betweennessCentrality(n, adj);

    vector<pair<double, int>> BC;
    for (int i = 0; i < n; i++) {
        BC.push_back({result.BC[i], i});
    }

    sort(BC.rbegin(), BC.rend());

    cout << "\nTop 20 Nodes:\n";
    for (int i = 0; i < min(20, n); i++) {
        cout << "Node " << reverseMap[BC[i].second]
             << " : " << BC[i].first << endl;
    }

    cout << "\nExecution time: " << result.time << " sec\n";
    cout << "Space: " << result.size << " bytes\n";

    return 0;
}

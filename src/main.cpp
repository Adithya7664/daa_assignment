#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <chrono>

using namespace std;

struct Result {
    vector<double> BC;
    double time;
    size_t approx_mem;
};

Result brandes(int n, vector<vector<int>> &adj) {

    vector<double> CB(n, 0.0);
    vector<vector<int>> P(n);
    vector<int> sigma(n);
    vector<int> dist(n);
    vector<double> delta(n);

    auto start = chrono::high_resolution_clock::now();

    for (int s = 0; s < n; s++) {

        stack<int> S;

        for (int i = 0; i < n; i++) P[i].clear();
        fill(sigma.begin(), sigma.end(), 0);
        fill(dist.begin(), dist.end(), -1);
        fill(delta.begin(), delta.end(), 0.0);

        sigma[s] = 1;
        dist[s] = 0;

        queue<int> Q;
        Q.push(s);

        while (!Q.empty()) {
            int v = Q.front(); Q.pop();
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
        
        while (!S.empty()) {
            int w = S.top(); S.pop();

            for (int v : P[w]) {
                delta[v] += ((double)sigma[v] / sigma[w]) * (1.0 + delta[w]);
            }

            if (w != s) {
                CB[w] += delta[w];
            }
        }
    }

    for (int i = 0; i < n; i++) CB[i] /= 2.0;

    auto end = chrono::high_resolution_clock::now();
    double exec_time = chrono::duration<double>(end - start).count();

    size_t mem = 0;
    for (auto &v : adj) mem += v.size() * sizeof(int);
    mem += n * sizeof(int) * 2;
    mem += n * sizeof(double) * 2;

    return {CB, exec_time, mem};
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

    unordered_map<int,int> idMap;
    vector<int> reverseMap;
    vector<pair<int,int>> edges;

    string line;
    int idx = 0;

    while (getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;

        int u, v;
        if (sscanf(line.c_str(), "%d %d", &u, &v) != 2) continue;

        if (!idMap.count(u)) {
            idMap[u] = idx++;
            reverseMap.push_back(u);
        }
        if (!idMap.count(v)) {
            idMap[v] = idx++;
            reverseMap.push_back(v);
        }

        edges.push_back({idMap[u], idMap[v]});
    }

    int n = idMap.size();
    vector<vector<int>> adj(n);

    for (auto &e : edges) {
        int u = e.first, v = e.second;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    cout << "Nodes: " << n << ", Edges: " << edges.size() << "\n";

    Result res = brandes(n, adj);

    vector<pair<double,int>> order;
    for (int i = 0; i < n; i++) order.push_back({res.BC[i], i});

    sort(order.rbegin(), order.rend());

    cout << "\nTop 20 Nodes:\n";
    for (int i = 0; i < min(20, n); i++) {
        cout << "Node " << reverseMap[order[i].second]
             << " : " << order[i].first << "\n";
    }

    cout << "\nExecution time: " << res.time << " sec\n";
    cout << "Approx memory: " << res.approx_mem << " bytes\n";

    return 0;
}

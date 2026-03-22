#include <bits/stdc++.h>
using namespace std;

struct Structure
{
    vector<double> BC;
    double time;
    size_t size;
};

Structure betweennessCentrality(int n, vector<vector<int>> &adj, bool undirected)
{
    vector<double> CB(n, 0.0);
    auto start = chrono::high_resolution_clock::now();
    stack<int> S;
    vector<vector<int>> P(n);
    vector<int> sigma(n, 0);
    vector<int> dist(n, -1);

    for (int s = 0; s < n; s++)
    {
        // cout << s << endl;

        sigma[s] = 1;
        dist[s] = 0;

        queue<int> Q;
        Q.push(s);

        while (!Q.empty())
        {
            int v = Q.front();
            Q.pop();
            S.push(v);

            for (int w : adj[v])
            {
                if (dist[w] < 0)
                {
                    Q.push(w);
                    dist[w] = dist[v] + 1;
                }
                if (dist[w] == dist[v] + 1)
                {
                    sigma[w] += sigma[v];
                    P[w].push_back(v);
                }
            }
        }

        vector<double> delta(n, 0.0);

        while (!S.empty())
        {
            int w = S.top();
            S.pop();

            for (int v : P[w])
            {
                delta[v] += ((double)sigma[v] / sigma[w]) * (1.0 + delta[w]);
            }

            if (w != s)
            {
                CB[w] += delta[w];
            }
        }
    }

    if (undirected)
    {
        for (int i = 0; i < n; i++)
            CB[i] /= 2.0;
    }
    auto end = chrono::high_resolution_clock::now();
    double execT = chrono::duration<double>(end - start).count();

    size_t total_memory = 0;
    for (auto &v : adj)
    {
        total_memory += v.size() * sizeof(int);
    }
    for (auto &v : P)
    {
        total_memory += v.size() * sizeof(int);
    }
    total_memory += n * sizeof(int) * 2;
    total_memory += n * sizeof(double) * 2;

    return {CB, execT, total_memory};
}

int main(int argc, char *argv[])
{

    cout << "Program running" << endl;

    ifstream in(argv[1]);
    if (!in)
    {
        cout << "FILE NOT OPENED\n";
        return 1;
    }
    cout << "file opened" << endl;
    unordered_map<int, int> mmap;
    vector<pair<int, int>> edges;
    vector<int> revmap;

    string line;
    int idx = 0;
    while (getline(in, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        int u, v;
        if (sscanf(line.c_str(), "%d %d", &u, &v) != 2)
            continue;
        // cout << u << " " << v << endl;
        if (!mmap.count(u))
        {
            mmap[u] = idx++;
            revmap.push_back(u);
        }
        if (!mmap.count(v))
        {
            mmap[v] = idx++;
            revmap.push_back(v);
        }
        int um = mmap[u];
        int vm = mmap[v];
        edges.push_back({um, vm});
    }
    int n = mmap.size();
    vector<vector<int>> adj(n);
    for (auto &e : edges)
    {
        int u = e.first;
        int v = e.second;
        if (u >= n || v >= n)
        {
            cout << "CRASH INDEX: " << u << " " << v << endl;
        }
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    cout << "n: " << n << endl;
    cout << "adj size: " << adj.size() << endl;
    Structure r = betweennessCentrality(n, adj, false);
    vector<pair<double, int>> BC;

    for (int i = 0; i < n; i++)
    {
        BC.push_back({r.BC[i], i});
    }

    sort(BC.rbegin(), BC.rend());
    cout << "Betweenness Centrality for top 20 nodes: \n"
         << endl;
    for (int i = 0; i < min(20, n); i++)
    {
        cout << "Node " << revmap[BC[i].second] << " : " << BC[i].first << endl;
    }
    cout << "\nExecution time: " << r.time << "s " << endl;
    cout << "Space consumption: " << r.size << " Bytes" << endl;
    return 0;
}

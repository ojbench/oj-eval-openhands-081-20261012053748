#include <bits/stdc++.h>
using namespace std;

class Dinic {
public:
    struct Edge {
        int to, cap, flow;
    };
    
    vector<Edge> edges;
    vector<vector<int>> adj;
    vector<int> level, iter;
    int n;
    
    Dinic(int n) : n(n) {
        adj.resize(n);
        level.resize(n);
        iter.resize(n);
    }
    
    void add_edge(int u, int v, int cap) {
        edges.push_back({v, cap, 0});
        edges.push_back({u, 0, 0});
        adj[u].push_back(edges.size() - 2);
        adj[v].push_back(edges.size() - 1);
    }
    
    bool bfs(int s, int t) {
        level.assign(n, -1);
        level[s] = 0;
        queue<int> q;
        q.push(s);
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            for (int idx : adj[u]) {
                Edge &e = edges[idx];
                if (e.cap - e.flow > 0 && level[e.to] < 0) {
                    level[e.to] = level[u] + 1;
                    q.push(e.to);
                }
            }
        }
        return level[t] >= 0;
    }
    
    int dfs(int u, int t, int pushed) {
        if (u == t || pushed == 0) return pushed;
        
        for (int &cid = iter[u]; cid < (int)adj[u].size(); cid++) {
            int idx = adj[u][cid];
            Edge &e = edges[idx];
            
            if (e.cap - e.flow > 0 && level[e.to] == level[u] + 1) {
                int tr = dfs(e.to, t, min(pushed, e.cap - e.flow));
                if (tr > 0) {
                    e.flow += tr;
                    edges[idx ^ 1].flow -= tr;
                    return tr;
                }
            }
        }
        return 0;
    }
    
    int max_flow(int s, int t) {
        int flow = 0;
        while (bfs(s, t)) {
            iter.assign(n, 0);
            while (int pushed = dfs(s, t, INT_MAX)) {
                flow += pushed;
            }
        }
        return flow;
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n, m;
    cin >> n >> m;
    
    vector<pair<int, int>> edges;
    
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        u--; v--;
        edges.push_back({u, v});
    }
    
    if (n == 1) {
        cout << 0 << endl;
        return 0;
    }
    
    long long total = 0;
    
    // Since n ≤ 3000 and degree ≤ 3, the graph is very sparse.
    // We can afford to run max flow for each pair.
    for (int a = 0; a < n; a++) {
        for (int b = a + 1; b < n; b++) {
            Dinic dinic(n);
            
            // Add all edges (undirected, capacity 1)
            for (auto [u, v] : edges) {
                dinic.add_edge(u, v, 1);
                dinic.add_edge(v, u, 1);
            }
            
            int flow = dinic.max_flow(a, b);
            total += flow;
        }
    }
    
    cout << total << endl;
    
    return 0;
}
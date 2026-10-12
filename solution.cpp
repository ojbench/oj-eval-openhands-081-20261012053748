#include <bits/stdc++.h>
using namespace std;

// Since each node has degree ≤ 3 and each edge has capacity 1,
// the max flow between any two nodes is at most 3.
// We can use a simple BFS-based approach to find edge-disjoint paths.

int max_flow_bfs(int s, int t, const vector<vector<int>>& adj, const vector<pair<int, int>>& edges, int m) {
    int flow = 0;
    vector<bool> edge_used(m, false);
    
    for (int path = 0; path < 3; path++) {
        queue<int> q;
        vector<int> parent(adj.size(), -1);
        vector<int> parent_edge(adj.size(), -1);
        
        q.push(s);
        parent[s] = s;
        
        while (!q.empty() && parent[t] == -1) {
            int u = q.front();
            q.pop();
            
            for (int v : adj[u]) {
                if (parent[v] == -1) {
                    // Find the edge index
                    int edge_idx = -1;
                    for (int i = 0; i < m; i++) {
                        if ((edges[i].first == u && edges[i].second == v) ||
                            (edges[i].first == v && edges[i].second == u)) {
                            edge_idx = i;
                            break;
                        }
                    }
                    
                    if (edge_idx != -1 && !edge_used[edge_idx]) {
                        parent[v] = u;
                        parent_edge[v] = edge_idx;
                        q.push(v);
                    }
                }
            }
        }
        
        if (parent[t] == -1) break;
        
        // Mark the path as used
        int curr = t;
        while (curr != s) {
            edge_used[parent_edge[curr]] = true;
            curr = parent[curr];
        }
        
        flow++;
    }
    
    return flow;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n, m;
    cin >> n >> m;
    
    vector<pair<int, int>> edges;
    vector<vector<int>> adj(n);
    
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        u--; v--;
        edges.push_back({u, v});
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    
    if (n == 1) {
        cout << 0 << endl;
        return 0;
    }
    
    long long total = 0;
    
    // For each pair (a, b) with a < b, compute max flow
    for (int a = 0; a < n; a++) {
        for (int b = a + 1; b < n; b++) {
            int flow = max_flow_bfs(a, b, adj, edges, m);
            total += flow;
        }
    }
    
    cout << total << endl;
    
    return 0;
}
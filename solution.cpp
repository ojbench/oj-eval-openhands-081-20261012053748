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
        
        for (int &cid = iter[u]; cid < adj[u].size(); cid++) {
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
    
    void reset_flow() {
        for (Edge &e : edges) {
            e.flow = 0;
        }
    }
};

class GomoryHuTree {
public:
    struct TreeNode {
        int parent, weight;
        vector<int> children;
    };
    
    vector<TreeNode> tree;
    int n;
    
    GomoryHuTree(int n) : n(n) {
        tree.resize(n);
        for (int i = 0; i < n; i++) {
            tree[i].parent = -1;
            tree[i].weight = 0;
        }
    }
    
    void add_edge(int u, int v, int weight) {
        tree[v].parent = u;
        tree[v].weight = weight;
        tree[u].children.push_back(v);
    }
    
    // LCA preprocessing for efficient min edge queries
    vector<vector<int>> up;
    vector<vector<int>> min_edge;
    vector<int> depth;
    int LOG;
    
    void dfs_lca(int u, int p, int d) {
        depth[u] = d;
        for (int v : tree[u].children) {
            if (v != p) {
                up[v][0] = u;
                min_edge[v][0] = tree[v].weight;
                dfs_lca(v, u, d + 1);
            }
        }
    }
    
    void build_lca() {
        LOG = 0;
        while ((1 << LOG) <= n) LOG++;
        
        up.assign(n, vector<int>(LOG));
        min_edge.assign(n, vector<int>(LOG));
        depth.assign(n, 0);
        
        // Find root (node with parent = -1)
        int root = 0;
        for (int i = 0; i < n; i++) {
            if (tree[i].parent == -1) {
                root = i;
                break;
            }
        }
        
        up[root][0] = root;
        min_edge[root][0] = INT_MAX;
        
        dfs_lca(root, -1, 0);
        
        for (int k = 1; k < LOG; k++) {
            for (int v = 0; v < n; v++) {
                up[v][k] = up[up[v][k-1]][k-1];
                min_edge[v][k] = min(min_edge[v][k-1], min_edge[up[v][k-1]][k-1]);
            }
        }
    }
    
    int get_min_edge_on_path(int u, int v) {
        if (u == v) return INT_MAX;
        
        int result = INT_MAX;
        
        if (depth[u] < depth[v]) swap(u, v);
        
        // Bring u up to depth of v
        for (int k = LOG-1; k >= 0; k--) {
            if (depth[u] - (1 << k) >= depth[v]) {
                result = min(result, min_edge[u][k]);
                u = up[u][k];
            }
        }
        
        if (u == v) return result;
        
        for (int k = LOG-1; k >= 0; k--) {
            if (up[u][k] != up[v][k]) {
                result = min(result, min_edge[u][k]);
                result = min(result, min_edge[v][k]);
                u = up[u][k];
                v = up[v][k];
            }
        }
        
        result = min(result, min_edge[u][0]);
        result = min(result, min_edge[v][0]);
        
        return result;
    }
};

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
    
    // Build Gomory-Hu tree
    GomoryHuTree ght(n);
    vector<int> parent(n, 0);
    
    for (int i = 1; i < n; i++) {
        // Create flow network
        Dinic dinic(n);
        
        // Add all edges
        for (auto [u, v] : edges) {
            dinic.add_edge(u, v, 1);
            dinic.add_edge(v, u, 1);
        }
        
        // Find min cut between i and parent[i]
        int flow = dinic.max_flow(i, parent[i]);
        
        // Find which side of cut each node is on
        vector<bool> visited(n, false);
        queue<int> q;
        q.push(i);
        visited[i] = true;
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            for (int idx : dinic.adj[u]) {
                auto &e = dinic.edges[idx];
                if (e.cap - e.flow > 0 && !visited[e.to]) {
                    visited[e.to] = true;
                    q.push(e.to);
                }
            }
        }
        
        // Update parent pointers
        for (int j = i + 1; j < n; j++) {
            if (parent[j] == parent[i] && visited[j]) {
                parent[j] = i;
            }
        }
        
        // Add edge to Gomory-Hu tree
        ght.add_edge(parent[i], i, flow);
    }
    
    // Build LCA structure for efficient queries
    ght.build_lca();
    
    // Calculate sum of max flows for all pairs a < b
    long long total = 0;
    
    for (int a = 0; a < n; a++) {
        for (int b = a + 1; b < n; b++) {
            int max_flow = ght.get_min_edge_on_path(a, b);
            total += max_flow;
        }
    }
    
    cout << total << endl;
    
    return 0;
}
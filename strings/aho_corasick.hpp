#ifndef AHO_CORASICK_HPP
#define AHO_CORASICK_HPP

// Verification: https://codeforces.com/group/CYMPFXi8zA/contest/261526/problem/I

#include <bits/stdc++.h>

using namespace std;

struct Aho_Corasick {

    using ll = long long int;

    int mxN = 2e6; // ! in constructor
    static const int AS = 26;
    const int ROOT = 1; // !
    int cnt = 2;

    struct Node {
        int ch, par, ref;
        int terminal; // 0 - not terminal, else - string_id (1-indexed)
        int prev_terminal; // nearest terminal node through refs to ROOT
        vector<int> next_terminals; // nearests terminal nodes through refs to LEAVES
        int to[AS];  // or use map
        int go[AS];
    };
    

    vector<Node> trie;

    vector<int> copy_s, left_index, right_index, st_to_node, visited, st_sz;
    
    // TASK dependent
    // copy_s - if there are duplicate pattern strings in input
    // left_index - most left entry of pattern in text string
    // right_index 
    // st_to_node - map[patter_string_id] = node_id
    // visited - for cache computing
    // sz_sz - sizes of pattern strings
    
    // see make_trie and solve
    
    // see solve 
    Aho_Corasick(int mxN): mxN(mxN) {
        trie.resize(mxN);
    }
    
    void bfs(int r) {
        
        queue<int> q;
        q.push(r);
        while(!q.empty()) {
            int u = q.front();
            q.pop();
            for(int i = 0; i < AS; ++i) {
                int v = trie[u].to[i];
                if (v) {
                    int prev_ref = trie[u].ref;
                    while(prev_ref != ROOT && trie[prev_ref].to[i]==0)
                        prev_ref = trie[prev_ref].ref;
                    if(trie[prev_ref].to[i] && v != trie[prev_ref].to[i])
                        trie[v].ref = trie[prev_ref].to[i];
                    else
                        trie[v].ref = ROOT;
                    
                    trie[v].prev_terminal = trie[trie[v].ref].terminal? trie[v].ref: trie[trie[v].ref].prev_terminal;
                    if (trie[v].terminal) {
                        trie[trie[v].prev_terminal].next_terminals.push_back(v);
                    }
                    
                    q.push(v);
                }
            }
        }
    }

    int go(int u, int i) {
        assert(u);
        if (u == ROOT) {
            trie[u].go[i] = trie[u].to[i];
            if (!trie[u].go[i])
                trie[u].go[i] = ROOT;
        }
        if (!trie[u].go[i]) {
            if (trie[u].to[i])
                trie[u].go[i] = trie[u].to[i];
            else {
                trie[u].go[i] = go(trie[u].ref, i);
            }
        }
        return trie[u].go[i];
    }

    // TASK dependent

    void calc(int i) {
        if (!visited[i]) {
            visited[i] = 1;
            int u = st_to_node[i];
            for(int nx: trie[u].next_terminals)
                calc(trie[nx].terminal-1);
            for(int nx: trie[u].next_terminals) {
                left_index[i] = min(left_index[i], left_index[(trie[nx].terminal-1)]);
                right_index[i] = max(right_index[i], right_index[(trie[nx].terminal-1)]);
            }
        }
    }


    void make_trie(const vector<string> & vs) { 
        trie[ROOT].ref = ROOT;
        int n = vs.size(); // # pattern strings
        copy_s.assign(n,-1), left_index.assign(n,mxN), right_index.assign(n,-mxN);
        st_to_node.assign(n, -1), visited.assign(n, 0), st_sz.assign(n ,0);
        
        for(int i = 0; i < n; ++i) {
            const string & s = vs[i];
            st_sz[i] = s.size();
            int cur_v = ROOT;
            for(char c: s) {
                if (trie[cur_v].to[c-'a']) {
                    cur_v = trie[cur_v].to[c-'a'];
                }
                else {
                    trie[cnt].ch = c;
                    trie[cnt].par = cur_v;
                    trie[cur_v].to[c-'a'] = cnt;
                    cur_v = cnt;
                    ++cnt;
                }
            }
            if (trie[cur_v].terminal) {
                copy_s[i] = trie[cur_v].terminal - 1;
            }
            else {
                trie[cur_v].terminal = (i+1);
            }
            st_to_node[i] = cur_v;
        }
        
        bfs(ROOT);
    }

    // TASK dependent
    // EXAMPLE: leftmost and rightmost entries of pattern strings
    void solve(const string & t) {
        int cur_v = ROOT;
        for(int i = 0; i < (int)t.size(); ++i) {
            int c = t[i] - 'a';
            cur_v = go(cur_v, c);
            // calc
            int term_s = trie[cur_v].terminal? trie[cur_v].terminal: trie[trie[cur_v].prev_terminal].terminal; // mb trie[0]
            if (term_s) {
                term_s--;
                left_index[term_s] = min(left_index[term_s], i);
                right_index[term_s] = max(right_index[term_s], i);
            }
        }
        int n = copy_s.size(); // n - # pattern strings
        for(int i = 0; i < n; ++i) {
            int j = copy_s[i] == -1? i: copy_s[i];
            calc(j);
            if (left_index[j] == mxN) {
                cout << "-1 -1"; 
            }
            else {
                cout << left_index[j] - st_sz[i] + 1 << " " << right_index[j] - st_sz[i] + 1;
            }
            cout << "\n";
        }
    }

};

#endif

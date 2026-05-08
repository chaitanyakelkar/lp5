#include<iostream>
#include<omp.h>
#include<chrono>
#include<vector>
#include<queue>
#include<stack>

using namespace std;
using namespace chrono;

class Graph{
    public:
        int vertices;
        vector<vector<int>> graph;
        vector<bool> visited;

        Graph(int v){
            this->vertices = v;
            this->graph.resize(v);
            this->visited.resize(v, false);
        }

        void reset_visited(){
            for (int i = 0; i < this->vertices; i++){
                this->visited[i] = false;
            }
        }

        void add_edge(int a, int b){
            this->graph[a].push_back(b);
            this->graph[b].push_back(a); //undirected graph
        }

        void sequential_dfs(int start){
            reset_visited();
            stack<int> s;
            
            s.push(start);
            visited[start] = true;
            while(!s.empty()){
                int current = s.top();
                s.pop();

                cout << current << " ";
                for (int i = 0; i < this->graph[current].size(); i++){
                    int neighbour = this->graph[current][i];
                    if (!this->visited[neighbour]){
                        s.push(neighbour);
                        this->visited[neighbour] = true;
                    }
                }
            }
            cout << endl;
        }

        void parallel_dfs(int start){
            reset_visited();
            #pragma omp parallel
            {
                #pragma omp single
                {
                    dfs_task(start);
                }
            }
        }

        void dfs_task(int start){
            stack<int> s;
            
            s.push(start);
            visited[start] = true;
            while(!s.empty()){
                int current = s.top();
                s.pop();

                cout << current << " ";

                #pragma omp parallel for
                for (int i = 0; i < this->graph[current].size(); i++){
                    int neighbour = this->graph[current][i];
                    if (!this->visited[neighbour]){
                        #pragma omp critical
                        {
                            if (!this->visited[neighbour]){
                                s.push(neighbour);
                                this->visited[neighbour] = true;
                            }
                        }
                    }
                }
            }
            cout << endl;
        }

        void sequential_bfs(int start){
            reset_visited();
            queue<int> s;
            
            s.push(start);
            visited[start] = true;
            while(!s.empty()){
                int current = s.front();
                s.pop();

                cout << current << " ";
                for (int i = 0; i < this->graph[current].size(); i++){
                    int neighbour = this->graph[current][i];
                    if (!this->visited[neighbour]){
                        s.push(neighbour);
                        this->visited[neighbour] = true;
                    }
                }
            }
            cout << endl;
        }

        void parallel_bfs(int start){
            reset_visited();
            #pragma omp parallel
            {
                #pragma omp single
                {
                    bfs_task(start);
                }
            }
        }

        void bfs_task(int start){
            queue<int> s;
            
            s.push(start);
            visited[start] = true;
            while(!s.empty()){
                int current = s.front();
                s.pop();

                cout << current << " ";

                #pragma omp parallel for
                for (int i = 0; i < this->graph[current].size(); i++){
                    int neighbour = this->graph[current][i];
                    if (!this->visited[neighbour]){
                        #pragma omp critical
                        {
                            if (!this->visited[neighbour]){
                                s.push(neighbour);
                                this->visited[neighbour] = true;
                            }
                        }
                    }
                }
            }
            cout << endl;
        }
};

int main(){
    int V = 50;
    int E = 1000;
    Graph g(V);

    for (int i = 0; i < E; i++){
        int a = rand() % V;
        int b = rand() % V;

        if (a != b){
            g.add_edge(a,b);
        }
    }

    auto start = high_resolution_clock::now();
    g.sequential_bfs(0);
    auto end = high_resolution_clock::now();
    double time1 = duration<double>(end-start).count();
    cout << "Sequential BFS Time: " << time1 << endl;

    start = high_resolution_clock::now();
    g.parallel_bfs(0);
    end = high_resolution_clock::now();
    double time2 = duration<double>(end-start).count();
    cout << "Parallel BFS Time: " << time2 << endl;
    cout << "SpeedUp in BFS: " << time1 / time2 << endl;

    start = high_resolution_clock::now();
    g.sequential_dfs(0);
    end = high_resolution_clock::now();
    double time3 = duration<double>(end-start).count();
    cout << "Sequential DFS Time: " << time3 << endl;

    start = high_resolution_clock::now();
    g.parallel_dfs(0);
    end = high_resolution_clock::now();
    double time4 = duration<double>(end-start).count();
    cout << "Parallel DFS Time: " << time4 << endl;
    cout << "SpeedUp in DFS: " << time3 / time4 << endl;

    return 0;
}
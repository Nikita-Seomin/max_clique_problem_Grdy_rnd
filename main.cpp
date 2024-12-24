#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <numeric>

using namespace std;

class Graph {
    int n; // Number of vertices
    vector<vector<bool>> adjMatrix; // Adjacency matrix for the graph

public:
    Graph(int size) : n(size), adjMatrix(size, vector<bool>(size, false)) {}

    void addEdge(int u, int v) {
        adjMatrix[u - 1][v - 1] = true;
        adjMatrix[v - 1][u - 1] = true;
    }

    // Get adjacency list of a vertex
    vector<int> getNeighbors(int v) {
        vector<int> neighbors;
        for (int i = 0; i < n; i++) {
            if (adjMatrix[v - 1][i]) {
                neighbors.push_back(i + 1);
            }
        }
        return neighbors;
    }

    // Check if all vertices in the set are connected to each other
    bool isClique(const set<int>& vertices) {
        for (int u : vertices) {
            for (int v : vertices) {
                if (u != v && !adjMatrix[u - 1][v - 1]) {
                    return false;
                }
            }
        }
        return true;
    }

    // Greedy random heuristic to find a clique
    vector<int> findClique() {
        set<int> clique;                 // Stores current clique
        vector<int> candidates(n);      // Candidate vertices (initially all vertices)
        vector<int> bestClique;         // Store the best clique found
        iota(candidates.begin(), candidates.end(), 1); // Fill with 1, ..., n

        while (!candidates.empty()) {
            // Randomly shuffle candidates for randomized behavior
            srand(time(0));
            random_shuffle(candidates.begin(), candidates.end());

            // Pick the first vertex and add it to the clique
            int v = candidates.back();
            candidates.pop_back();
            clique.insert(v);

            // Update candidates to only those connected to all in the clique
            vector<int> newCandidates;
            for (int w : candidates) {
                bool isConnected = true;
                for (int u : clique) {
                    if (!adjMatrix[u - 1][w - 1]) {
                        isConnected = false;
                        break;
                    }
                }
                if (isConnected) {
                    newCandidates.push_back(w);
                }
            }
            candidates = newCandidates;

            // Update best clique if the current clique is larger
            if (clique.size() > bestClique.size()) {
                bestClique = vector<int>(clique.begin(), clique.end());
            }
        }
        return bestClique;
    }
};

Graph parseDIMACSFile(const string& filename) {
    ifstream inputFile(filename);
    if (!inputFile) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }

    string line;
    int numVertices = 0, numEdges = 0;

    while (getline(inputFile, line)) {
        if (line[0] == 'p') {
            // Problem line: p edge <numVertices> <numEdges>
            sscanf(line.c_str(), "p edge %d %d", &numVertices, &numEdges);
            break;
        }
    }

    Graph graph(numVertices);

    while (getline(inputFile, line)) {
        if (line[0] == 'e') {
            int u, v;
            sscanf(line.c_str(), "e %d %d", &u, &v);
            graph.addEdge(u, v);
        }
    }

    inputFile.close();
    return graph;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <DIMACS file>" << endl;
        return 1;
    }

    // Parse the graph from input file
    Graph graph = parseDIMACSFile(argv[1]);

    // Run the greedy randomized algorithm
    vector<int> clique = graph.findClique();

    // Output results
    cout << clique.size() << endl;
    for (int v : clique) {
        cout << v << " ";
    }
    cout << endl;

    return 0;
}
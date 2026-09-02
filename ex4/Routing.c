#include <stdio.h>
#include <stdlib.h>

#define INF 9999
#define MAX_NODES 20

int costMatrix[MAX_NODES][MAX_NODES];
int numNodes;

struct RouteTable {
    int dist[MAX_NODES];
    int nextHop[MAX_NODES];
} rt[MAX_NODES];

void inputGraph();
void printInitialGraph();
void distanceVectorRouting();
void dijkstraRouting();
void findCustomPathAndAllPaths();
void modifyEdgeCost();
void printPath(int parent[], int j);
void findAllPathsDFS(int current, int dest, int visited[], int path[], int pathIdx, int currentCost, int *minCost, int shortestPath[], int *shortestPathLen);

int main() {
    int choice;
    inputGraph();

    while(1) {
        printf("\nMENU:\n1. Print Initial Graph & Routing Tables\n2. Distance Vector Routing\n3. Dijkstra's Algorithm\n4. Find Shortest & All Paths Between 2 Nodes\n5. Change Edge Cost\n6. Exit\nEnter choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid selection type. Exiting.\n");
            exit(1);
        }

        switch(choice) {
            case 1: printInitialGraph(); break;
            case 2: distanceVectorRouting(); break;
            case 3: dijkstraRouting(); break;
            case 4: findCustomPathAndAllPaths(); break;
            case 5: modifyEdgeCost(); break;
            case 6: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
    return 0;
}

void inputGraph() {
    int i, j;
    printf("Enter number of nodes: ");
    if (scanf("%d", &numNodes) != 1 || numNodes <= 0 || numNodes > MAX_NODES) {
        printf("Invalid number of nodes.\n");
        exit(1);
    }

    printf("Enter the %dx%d cost matrix (row by row):\n", numNodes, numNodes);
    for(i = 0; i < numNodes; i++) {
        for(j = 0; j < numNodes; j++) {
            if (scanf("%d", &costMatrix[i][j]) != 1) {
                printf("Error parsing matrix input data values.\n");
                exit(1);
            }
            if(i == j) {
                costMatrix[i][j] = 0;
            }
        }
    }
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void printInitialGraph() {
    int i, j;
    printf("\n--- INITIAL MATRIX ---\n    ");
    for(i = 0; i < numNodes; i++) printf("%c\t", 'A' + i);
    printf("\n");
    for(i = 0; i < numNodes; i++) {
        printf("%c | ", 'A' + i);
        for(j = 0; j < numNodes; j++) {
            if(costMatrix[i][j] == INF) printf("INF\t");
            else printf("%d\t", costMatrix[i][j]);
        }
        printf("\n");
    }

    printf("\n--- INITIAL ROUTING TABLES ---\n");
    for(i = 0; i < numNodes; i++) {
        printf("\nTable for Node %c:\nDest\tCost\tNext\n", 'A' + i);
        for(j = 0; j < numNodes; j++) {
            if (costMatrix[i][j] == INF) printf("%c\tINF\t-\n", 'A' + j);
            else printf("%c\t%d\t%c\n", 'A' + j, costMatrix[i][j], 'A' + j);
        }
    }
}

void distanceVectorRouting() {
    int i, j, k, count;

    for(i = 0; i < numNodes; i++) {
        for(j = 0; j < numNodes; j++) {
            rt[i].dist[j] = costMatrix[i][j];
            if(costMatrix[i][j] != INF) rt[i].nextHop[j] = j;
            else rt[i].nextHop[j] = -1;
        }
    }

    do {
        count = 0;
        for(i = 0; i < numNodes; i++) {
            for(j = 0; j < numNodes; j++) {
                for(k = 0; k < numNodes; k++) {
                    if(rt[i].dist[k] != INF && costMatrix[k][j] != INF) {
                        if(rt[i].dist[j] > rt[i].dist[k] + costMatrix[k][j]) {
                            rt[i].dist[j] = rt[i].dist[k] + costMatrix[k][j];
                            rt[i].nextHop[j] = rt[i].nextHop[k];
                            count++;
                        }
                    }
                }
            }
        }
    } while(count != 0);

    printf("\n--- DISTANCE VECTOR: FINAL TABLES ---\n");
    for(i = 0; i < numNodes; i++) {
        printf("\nFinal Table for Node %c:\nDest\tCost\tNext\n", 'A' + i);
        for(j = 0; j < numNodes; j++) {
            if(rt[i].dist[j] == INF) printf("%c\tINF\t-\n", 'A' + j);
            else printf("%c\t%d\t%c\n", 'A' + j, rt[i].dist[j], 'A' + rt[i].nextHop[j]);
        }
    }

    printf("\n--- DISTANCE VECTOR: SHORTEST PATHS ---\n");
    for(i = 0; i < numNodes; i++) {
        for(j = 0; j < numNodes; j++) {
            if(i != j) {
                printf("Path %c -> %c: %c", 'A' + i, 'A' + j, 'A' + i);
                int temp = i;
                while(temp != j && rt[temp].nextHop[j] != -1) {
                    temp = rt[temp].nextHop[j];
                    printf(" -> %c", 'A' + temp);
                }
                if(rt[i].dist[j] == INF || temp != j) printf(" [No Path]");
                else printf(" | Cost: %d", rt[i].dist[j]);
                printf("\n");
            }
        }
    }
}

void printPath(int parent[], int j) {
    if (parent[j] == -1) return;
    printPath(parent, parent[j]);
    printf(" -> %c", 'A' + j);
}

void dijkstraRouting() {
    char srcChar;
    printf("\nEnter source node (A-%c): ", 'A' + numNodes - 1);
    scanf(" %c", &srcChar);
    int source = srcChar - 'A';

    if(source < 0 || source >= numNodes) return;

    int dist[MAX_NODES], visited[MAX_NODES], parent[MAX_NODES];
    int i, count, v, u;

    for(i = 0; i < numNodes; i++) {
        dist[i] = costMatrix[source][i];
        visited[i] = 0;
        if(costMatrix[source][i] != INF && i != source) parent[i] = source;
        else parent[i] = -1;
    }
    dist[source] = 0;
    visited[source] = 1;

    printf("\n--- DIJKSTRA: INITIAL STATE (%c) ---\nDest\tCost\n", 'A' + source);
    for(i = 0; i < numNodes; i++) {
        if(dist[i] == INF) printf("%c\tINF\n", 'A' + i);
        else printf("%c\t%d\n", 'A' + i, dist[i]);
    }

    for(count = 1; count < numNodes - 1; count++) {
        int minDist = INF; u = -1;
        for(v = 0; v < numNodes; v++) {
            if(!visited[v] && dist[v] < minDist) { minDist = dist[v]; u = v; }
        }
        if(u == -1) break;
        visited[u] = 1;
        for(v = 0; v < numNodes; v++) {
            if(!visited[v] && costMatrix[u][v] != INF && dist[u] + costMatrix[u][v] < dist[v]) {
                dist[v] = dist[u] + costMatrix[u][v];
                parent[v] = u;
            }
        }
    }

    printf("\n--- DIJKSTRA: FINAL STATE (%c) ---\nDest\tCost\n", 'A' + source);
    for(i = 0; i < numNodes; i++) {
        if(dist[i] == INF) printf("%c\tINF\n", 'A' + i);
        else printf("%c\t%d\n", 'A' + i, dist[i]);
    }

    printf("\n--- DIJKSTRA: SHORTEST PATHS ---\n");
    for(i = 0; i < numNodes; i++) {
        if(i != source) {
            if(dist[i] == INF) printf("Path %c -> %c: No Path\n", 'A' + source, 'A' + i);
            else {
                printf("Path %c -> %c: %c", 'A' + source, 'A' + i, 'A' + source);
                printPath(parent, i);
                printf(" | Cost: %d\n", dist[i]);
            }
        }
    }
}

void findAllPathsDFS(int current, int dest, int visited[], int path[], int pathIdx, int currentCost, int *minCost, int shortestPath[], int *shortestPathLen) {
    visited[current] = 1;
    path[pathIdx] = current;
    pathIdx++;

    if (current == dest) {
        // Print the path discovered
        for (int i = 0; i < pathIdx; i++) {
            printf("%c", 'A' + path[i]);
            if (i < pathIdx - 1) printf(" -> ");
        }
        printf(" | Total Cost: %d\n", currentCost);

        // Keep track of the absolute shortest path discovered so far
        if (currentCost < *minCost) {
            *minCost = currentCost;
            *shortestPathLen = pathIdx;
            for (int i = 0; i < pathIdx; i++) {
                shortestPath[i] = path[i];
            }
        }
    } else {
        for (int i = 0; i < numNodes; i++) {
            if (!visited[i] && costMatrix[current][i] != INF && costMatrix[current][i] != 0) {
                findAllPathsDFS(i, dest, visited, path, pathIdx, currentCost + costMatrix[current][i], minCost, shortestPath, shortestPathLen);
            }
        }
    }

    // Backtrack to unvisit node for alternative path discovery
    pathIdx--;
    visited[current] = 0;
}

void findCustomPathAndAllPaths() {
    char srcChar, destChar;
    printf("\nEnter Source and Destination nodes (e.g., A C): ");
    scanf(" %c %c", &srcChar, &destChar);

    int src = srcChar - 'A';
    int dest = destChar - 'A';

    if (src < 0 || src >= numNodes || dest < 0 || dest >= numNodes) {
        printf("Invalid source or destination selection!\n");
        return;
    }

    int visited[MAX_NODES] = {0};
    int path[MAX_NODES];
    int shortestPath[MAX_NODES];
    int minCost = INF;
    int shortestPathLen = 0;

    printf("\n--- ALL POSSIBLE PATHS FROM %c TO %c ---\n", srcChar, destChar);
    findAllPathsDFS(src, dest, visited, path, 0, 0, &minCost, shortestPath, &shortestPathLen);

    if (minCost == INF) {
        printf("No valid paths exist between %c and %c.\n", srcChar, destChar);
    } else {
        printf("\n--- SHORTEST PATH AMONG THEM ---\nPath: ");
        for (int i = 0; i < shortestPathLen; i++) {
            printf("%c", 'A' + shortestPath[i]);
            if (i < shortestPathLen - 1) printf(" -> ");
        }
        printf("\nCost: %d\n", minCost);
    }
}

void modifyEdgeCost() {
    char src, dest;
    int newCost;

    printf("\nEnter Edge (Source Dest, e.g., A B): ");
    scanf(" %c %c", &src, &dest);

    int u = src - 'A';
    int v = dest - 'A';

    if (u >= 0 && u < numNodes && v >= 0 && v < numNodes) {
        printf("OLD edge cost: %d\n", costMatrix[u][v]);
        printf("NEW edge cost: ");
        scanf("%d", &newCost);

        costMatrix[u][v] = newCost;
        printf("Edge %c -> %c updated to %d\n", src, dest, newCost);
    } else {
        printf("Invalid nodes entered!\n");
    }
}


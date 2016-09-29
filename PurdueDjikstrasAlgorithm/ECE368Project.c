#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>


/*****************************************
 Contributors:
 Zane Johnson, Yi Yang, Mark Brooks, Andrew Grossman
 
 References:
 
 Dijkstra Algorithm for Finding Shortest Path of a Graph - The Crazy Programmer. (2014). Retrieved March 30, 2016, from http://www.thecrazyprogrammer.com/2014/03/dijkstra-algorithm-for-finding-shortest-path-of-a-graph.html
 
 Much of second half of dijkstra's function was done using help from this article.
 *****************************************/

#define FALSE 0
#define TRUE 1
#define MAX_NODES (99999)
#define MAX_VAL (999999)
#define MAX_STR (15)
//STRUCTS
typedef char graphNode[MAX_STR];
typedef struct graphCDT *graphADT;

typedef struct vertexTag {		//Vertex
    graphNode name;			//identifying name of the vertex
    double x;			//x coordinate of the vertex
    double y;			//y coordinate of the vertex
    double dist;			//distance from start
    int visited;			//if vertex has been visited or not
    struct edgeTag *edges;		//edges connecting to the vertex
    struct vertexTag *next;		//next vertex
} vertexT;

typedef struct edgeTag {		//Edge
    double    distance;
    vertexT *connectsTo;		//the vertex the edge connects to
    struct edgeTag *next;		//next edge
} edgeT;

typedef struct graphCDT {		//Graph
    vertexT *vertices;		//first vertex of the graph
} graphCDT;

typedef struct _Map_Arr {
    char * name;
    int index;
}Map_Arr;


//FUNCTIONS
int firstInput(graphADT graph, char * filename, int * size);								//parses data from first input file
int secondInput(graphADT graph, char * filename, int size);							//parses data from second input file
int addVertex(graphADT graph, char vert_name[], double x, double y);	//adds the vertex and its coordinates to the graph
int addEdge(graphADT graph, char from[], char to[]);							//adds the edge to the graph
vertexT* findStart(graphADT graph, char from[]);
vertexT* findEnd(graphADT graph, char to[]);
double calcDist(vertexT* from, vertexT* to);
int graph_free_all(graphADT graph);
int findEdgeDistances(graphADT graph);
void dijkstra(graphADT graph, vertexT* start, vertexT* end, int size);
int VertexNameToIndex(char* name, Map_Arr *map);
int graph_free_recursion(vertexT* vert);
int edge_free_recursion(edgeT* edge);

//MAIN
int main(int argc, char ** argv) {
    //initialize the graph
    graphADT graph;
    int err;
    int size;
    graph = (graphADT)malloc(sizeof(struct graphCDT));
    graph->vertices = NULL;
    
    if(argc < 3) {
        printf("\n Must enter input file, and path file");
    }
    
    //first input file
    int first_read = firstInput(graph, argv[1], &size);
    if (!first_read) {
        err = findEdgeDistances(graph);
    }
    
    else {
        printf("\nError reading first file.");
    }
    //second input file
    int second_read = secondInput(graph, argv[2], size);
    
    if (second_read) {
        printf("\nError reading second file.\n\n");
    }
    printf("\n\n");
    // free all memory mallocd
    graph_free_all( graph );
    
    return 1;
}

int addVertex(graphADT graph, char vert_name[], double xcoord, double ycoord) {
    vertexT *tmpvert, *vert;
    
    if (NULL != graph) {
        vert = graph->vertices;		//set 'vert' equal to the first vertex of the graph
        //check to see whether we really need to create a new vertex (checks for duplicate name)
        tmpvert = vert;
        while (tmpvert != NULL) {
            vert = tmpvert;
            if (tmpvert->name == vert_name) {
                return 0;
            }
            tmpvert = tmpvert->next;
        }
        //allocate memory and add to the graph
        tmpvert = (vertexT*)malloc(sizeof(vertexT));
        //initializes all the tmpvert's value like x,y coordinates, edge and next list, etc.
        strcpy(tmpvert->name, vert_name);			//vertex name
        tmpvert->x = xcoord;					//x coordinate
        tmpvert->y = ycoord;					//y coordinate
        tmpvert->dist = MAX_VAL;					//distance from root for traversing (initialized to infinite)
        tmpvert->visited = 0;					//not yet visited
        tmpvert->edges = NULL;					//no edges (yet)
        tmpvert->next = NULL;					//no next vertex (yet)
        //puts tmpvert at beginning of graph if first, otherwise is traverses to the end and places it there
        if (vert == NULL) {
            graph->vertices = tmpvert;	//set as the first vertex of the graph
        }
        else {
            vert->next = tmpvert;			//set as the next vertex of the graph
        }
    }
    return 1;
}

int addEdge(graphADT graph, char from[], char to[]) {
    //adds an edge pointing from the vertex with the name of 'from[]' to the vertex with the name of 'to[]'
    vertexT *vert, *start = NULL, *end = NULL; //vert is used to traverse through the graphs vertices
    //start and end are set as the to and from vertices once found
    edgeT *edge, *new_edge; //edge is used to traverse through edge graph assuming there is one, new_edge is what is
    //added to the end of the edge list
    
    //traverses through the graphs vertice linked list using strcmp to find the start and end vertices
    for (vert = graph->vertices; vert != NULL; vert = vert->next) {
        if (!strcmp(vert->name, from)) {
            start = vert;			//start is now the pointer of the 'from' vertex
        }
        if(!strcmp(vert->name, to)) {
            end = vert;					//end is now the pointer of the 'to' vertex
        }
    }
    //if there's no edge list, it intializes it, otherwise it traverses through the edge list and adds it to the end
    if (start->edges == NULL) {
        edge = (edgeT*)malloc(sizeof(edgeT));
        edge->connectsTo = end;
        start->edges = edge;
        start->edges->next = NULL;
    }
    else {
        edge = start->edges;
        while (edge->next != NULL) {
            edge = edge->next;
        }
        new_edge = (edgeT*)malloc(sizeof(edgeT));
        new_edge->connectsTo = end;
        new_edge->next = NULL;
        edge->next = new_edge;
    }
    return 1;
}

vertexT* findStart(graphADT graph, char from[]) {
    vertexT *vert; //used to traverse through graphs vertices
    for (vert = graph->vertices; vert != NULL; vert = vert->next) {
        if (!strcmp(vert->name, from)) {
            vert->dist = 0;
            return vert;	//start is now the pointer of the 'from' vertex
        }
    }
    return NULL;
}

vertexT* findEnd(graphADT graph, char to[]) {
    vertexT *vert; //used to traverse through graphs vertices
    for (vert = graph->vertices; vert != NULL; vert = vert->next) {
        if (!strcmp(vert->name, to)) {
            return vert;	//start is now the pointer of the 'to' vertex
        }
    }
    return NULL;
}

double calcDist(vertexT* from, vertexT* to) {
    double distance_s;
    double distance;
    double end_x, start_x, end_y, start_y;
    
    end_x = to->x;
    start_x = from->x;
    end_y = to->y;
    start_y = from->y;
    
    distance_s = pow((end_x - start_x), 2) + pow((end_y - start_y), 2);
    distance = pow(distance_s, .5);
    
    return distance;
}

int findEdgeDistances(graphADT graph) {
    // computes dstances of each edge
    vertexT *vert; //used to traverse through graphs vertices
    edgeT *edge; //used to traverse through vertices edges
    double distance;
    
    for (vert = graph->vertices; vert != NULL; vert = vert->next) {
        edge = vert->edges;
        
        if (edge != NULL) {
            distance = calcDist(vert, edge->connectsTo);
            edge->distance = distance;
            //traverses through edges and one each edge it changes its distance
            while (edge != NULL) {
                edge = edge->next;
                if (edge != NULL) {
                    distance = calcDist(vert, edge->connectsTo);
                    edge->distance = distance;
                }
            }
        }
    }
    return 1;
}



// Funtion that implements 2-D Array Dijkstra's algorithm
void dijkstra(graphADT graph, vertexT* start, vertexT* end, int size) {
    vertexT *vert;
    edgeT *edge;
    int j = 0, k = 0;
    int count;	vert = graph->vertices;
    int i;
    int startIndex;
    int endIndex;
    char * finalArray[MAX_STR];
    int cnt;
    Map_Arr* map = (Map_Arr*) malloc(sizeof(Map_Arr) * size);
    
    
    double **adjMat = (double**) malloc(sizeof(double*) * size);
    for (i = 0; i < size; i++) {
        adjMat[i] = (double*) malloc(sizeof(double) * size);
    }
    for (i = 0; i < size; i++) {
        map[i].name = vert->name;
        map[i].index = i;
        vert = vert->next;
    }
    for (i = 0; i < size; i++) {
        if (!(strcmp(map[i].name,start->name))) {
            startIndex = i;
        }
    }
    for (i = 0; i < size; i++) {
        if (!(strcmp(map[i].name,end->name))) {
            endIndex = i;
        }
    }
    
    for (i = 0; i < (size); i++) {
        for (j = 0; j < (size); j++) {
            if (i == j) {
                adjMat[i][j] = 0;
            }
            else {
                adjMat[i][j] = MAX_VAL;
            }
        }
    }
    //  print2dArray(adjMat, size, size);
    vert = graph->vertices;
    while (vert->next != NULL) {
        j = VertexNameToIndex(vert->name, map);
        edge = vert->edges;
        while (edge != NULL) {
            k = VertexNameToIndex(edge->connectsTo->name, map);
            adjMat[j][k] = edge->distance;
            edge = edge->next;
        }
        vert = vert->next;
    }
    // print2dArray(adjMat, size, size);
    double *distance = (double*) malloc(sizeof(double) * size);
    int *pred = (int*) malloc(sizeof(int) * size);
    int *visited = (int*) malloc(sizeof(int) * size), nextnode;
    double mindistance;
    for (i = 0; i< size; i++) {
        distance[i] = adjMat[startIndex][i];
        pred[i] = startIndex;
        visited[i] = 0;
    }
    //printotherArrays(distance, pred, visited);
    distance[startIndex] = 0;
    visited[startIndex] = 1;
    count = 1;
    // printotherArrays(distance, pred, visited);
    while (count< size - 1) {
        mindistance = MAX_VAL;
        //nextnode gives the node at minimum distance
        for (i = 0; i<size; i++) {
            if (distance[i]<mindistance&&!visited[i]) {
                mindistance = distance[i];
                nextnode = i;
            }
        }
        //check if a better path exists through nextnode
        visited[nextnode] = 1;
        //printotherArrays(distance, pred, visited);
        for (i = 0; i<size; i++) {
            if (!visited[i]) {
                if (mindistance + adjMat[nextnode][i]<distance[i]) {
                    distance[i] = mindistance + adjMat[nextnode][i];
                    pred[i] = nextnode;
                }
            }
            // printotherArrays(distance, pred, visited);
        }
        count++;
    }
    //print the path and distance of each node
    cnt = 0;
    for (i = 0; i<size; i++) {
        
        if (i != startIndex && i == endIndex) {
            if(distance[i] != MAX_VAL) {
                printf("\nDistance of node %s = %f miles", map[i].name, distance[i]);
                printf("\n Path = ");
                finalArray[cnt] = map[i].name;
                cnt++;
                j = i;
                do {
                    j = pred[j];
                    finalArray[cnt] = map[j].name;
                    cnt++;
                } while (j != startIndex);
                
                while(cnt > 1) {
                    cnt--;
                    printf("%s->", finalArray[cnt]);
                }
                printf("%s", finalArray[0]);
            }
            else {
                printf("\n The two nodes are not connected\n");
            }
        }
    }
    for (i = 0; i < size; i++) {
        free(adjMat[i]);
    }
    free(adjMat);
    free(pred);
    free(visited);
    free(distance);
    free(map);
}

int VertexNameToIndex(char* name, Map_Arr* map) {
    int i = 0;
    while (map[i].name != name) {	//matches name in map with index in map
        i++;
    }
    return i;
}

int firstInput(graphADT graph, char * filename, int * size) {
    FILE * fp = NULL;
    int err = FALSE;				//file open error
    int tot_nodes = 0;
    int tot_connect = 0;			//total vertex and connections
    char vert_name[50];				//name of the vertex
    double xcoord, ycoord;			//x and y coordinates
    char connect_from[50], connect_to[50];	//connections between vertexes
    int i;					//loop counters
    
    if (!err) { // Open filename
        fp = fopen(filename, "r");
        //file open error
        if (!fp) {
            fprintf(stderr, "Failed to open file '%s'\n", filename);
            err = TRUE;
            return err;
        }
        
        fscanf(fp, "%d%d", &tot_nodes, &tot_connect);	//assign total nodes and total connections values to be used in the following loops
        //printf("total nodes\n");
        for (i = 0; i < tot_nodes; i++) {
            fscanf(fp, "%s%lf%lf", vert_name, &xcoord, &ycoord);
            addVertex(graph, vert_name, xcoord, ycoord);	//Add vertex to graph, with coordinate information
            //printf("%d\n", i);
        }
        //printf("scanned nodes\n");
        
        for (i = 0; i < tot_connect; i++) {
            fscanf(fp, "%s %s", connect_from, connect_to);
            addEdge(graph, connect_from, connect_to);	//Create the link between the respective vertexes
        }
        //printf("scanned edges\n");
    }
    *size = tot_nodes;
    fclose(fp);
    return err;
}

int secondInput(graphADT graph, char * filename, int size) {
    FILE * fp = NULL;
    int err = FALSE;							//file open error
    char from_node[50], to_node[50];	//starting and ending vertexes
    if (!err) { // Open filename
        
        fp = fopen(filename, "r");
        //file open error
        if (!fp) {
            fprintf(stderr, "Failed to open file '%s'\n", filename);
            err = TRUE;
            return err;
        }
        
        vertexT* start;
        vertexT* end;
        fscanf(fp, "%s %s", from_node, to_node);
        start = findStart(graph, from_node);
        //Determine the shortest path between nodes and calculates distance
        if (start == NULL) {
            printf("Start node non-existent\n");
            return 0;
        }
        end = findEnd(graph, to_node);
        if (end == NULL) {
            printf("End node non-existent\n");
            return 0;
        }
        dijkstra(graph, start, end, size);
    }
    fclose(fp);
    return err;
}

int graph_free_all(graphADT graph) {
    graph_free_recursion(graph->vertices);
    free(graph);
    return 1;
}

int graph_free_recursion(vertexT* vert) {
    if(vert->next != NULL) {
        graph_free_recursion(vert->next);
    }
    edge_free_recursion(vert->edges);
    free(vert);
    return 1;
}

int edge_free_recursion(edgeT* edges) {
    if(edges!=NULL) {
        if(edges->next != NULL)	{
            edge_free_recursion(edges->next);
        }
        free(edges);
    }	
    return 1;
}


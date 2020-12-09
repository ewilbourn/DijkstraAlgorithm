//Emily Wilbourn
//11-27-2020
//Program 4 - CSC 245
#include "graph.h" /*graph object*/
#include "queue.h" /*queue object*/
#include <string>
#include <iostream> /*cout & cin*/
#include <fstream> /*for reading in data from a given file*/
#include <sstream> /*for converting string to integer in my initializeArray method*/
#include <algorithm> /*used for getting a vector of unique values in uniquePlaces method*/ 
#include <math.h> /*log10 and floor for determing number of digits in a number*/
#include <climits> /*for INT_MAX*/
#include <stack> /*for finding cycles in a graph*/ 


struct vertex_info
{
	string origin;
	bool mark;
	int distance;
	string destination;
};

void printInfo(vector<vertex_info> uniqueNodes, int numVertices);
void initializeArray(vector <vertex_info> &v, string fileName);
vector <vertex_info> uniquePlaces(vector <vertex_info> v);
void fillVertices(Graph <string> &g, vector<vertex_info> place);
void fillGraph(Graph<string> &g, vector <vertex_info> uniqueVals, vector <vertex_info> v);
int findVertex(vector<vertex_info> uniqueVals, string vertex);
void fillEdges(Graph <string> &g, vector<vertex_info> v);
void dijkstra (Graph<string> &g,vector <vertex_info> &uniqueVals, string start_vertex);
int adjacentDistUpdate(Graph<string> g, vector<vertex_info> &uniqueVals, string current, vector<int> &marked_indexes);
void markVertex(Graph<string> &g, vector<vertex_info>&uniqueVals, vector<int> &markedIndexes);
void printArray(vector<vertex_info> uniqueVals);
void printSpaces(int numSpaces);
int s_numSpaces(string vertex);
int n_numSpaces(int value);
bool hasCycle(Graph<string> &g, vector <vertex_info> v);
int depthfirstsearch(Graph<string>&g, stack<string>s, vector <vertex_info> &v, vector <string> &path);
void printCycle(vector <string> path);
int main(int argc, char *argv[])
{
	//Instantiate a vector of vertex_info objects.
	//This vector will hold all the original Origin/Destination/Distance 
	//values from our input file.
	vector <vertex_info> v;

	//Get the file name with the graph data from user input on command line
	string fileName = argv[1];

	//test to see if the fileName is a valid file
 	ifstream test(fileName);
	if (!test.is_open())
	{
		cout << "The file " << fileName << " does not exist." << endl;
		cout << "Please try again." << endl;
		return -1;
	}
	//call method to initialize our array of vertex_info objects
	initializeArray(v, fileName);	

	//Create vector that we will update with vertex_info structs as we apply
	//Dijkstra's algorithm.
	vector <vertex_info> uniqueVals = uniquePlaces(v);

	//Create another vector, the same as uniqueVals, to be used in implementing
	//cycle detection for a graph.
	vector <vertex_info> cycleVals = uniquePlaces(v);

	//Get the number of vertices in our graph; this is the size of our
	//vector v because each object in v is a different vertex.
	int numVertices = uniqueVals.size();

	//print out the title for the program and the given, unique vertices in the input file
	printInfo(uniqueVals, numVertices);

	//get the starting position for the graph
	string startingVertex;
	cout << "\n\t\tPlease input your starting vertex: ";
	cin >> startingVertex;

	int index = findVertex(uniqueVals, startingVertex);
	while (index == -1)
	{
		cout << "\t\tStarting location does not exist..." << endl;
		cout << "\t\tPlease input your starting vertex: ";
		cin >> startingVertex;
		index = findVertex(uniqueVals, startingVertex);
	}

	//instantiate our graph and queue objects
	Graph<string> myGraph(numVertices);
	
	//call to fill the vertices/edges of the graph	
	fillGraph(myGraph, uniqueVals, v);

	//call to perform Dijkstra's Algorithm given the starting vertex
	dijkstra(myGraph, uniqueVals, startingVertex);	

	printArray(uniqueVals);	

	bool cycle = hasCycle(myGraph, cycleVals);
	cout << (cycle ? "The graph contains a cycle." : "The graph does not contain a cycle.") << endl;
	
	return 0;
}

//method to print the "Dijksta's Algorithm" title and all the vertices given in the input file
//precondition: vector of vertex_info structs with all the unique vertices in the file and the number of 
//unique vertices in the file
//postcondition: return nothing, but print out the title and all the unique vertices for the user
void printInfo(vector<vertex_info> uniqueNodes, int numVertices)
{
	cout << "\t\t^^^^^^^^^^^^^^^^^^^^^^^^^^    DIJKSTRA'S ALGORITHM    ^^^^^^^^^^^^^^^^^^^^^^^^^^\n" << endl;
        cout << "\t\tA Weighted Graph Has Been Built For These " << numVertices << "  Cities :\n" << endl;

	for (int i = 0; i < uniqueNodes.size(); i++)
	{
		int numSpaces = s_numSpaces(uniqueNodes.at(i).destination);
		printSpaces(numSpaces);
		cout << uniqueNodes.at(i).destination;
	
		//if the current i value is a multiple of 3, then print a new line	
		if ((i+1)%3 == 0)
			cout << endl;
	}
}

//precondition: v is empty
//postcondition: nothing is returned, but v is filled
void initializeArray(vector <vertex_info> &v, string fileName)
{
	vector <string> words;
	string line;
	string word;
	ifstream input(fileName);

	//procedure to fill a vector of strings with all the words in the file
	if (input.is_open())
	{
		while (getline (input, line))
		{
			//this turns our line into an istringstream object so we can
			//use getline on it
			istringstream s (line);
			
			while (getline (s, word, ';'))
			{
				words.push_back(word);
			}
		}
		input.close();
	}

	//since we know that each line has the following format: Origin;Destination;Trip-length,
	//then we can fill our vector of structs with the info from our words vector
	for (int i = 0; (i+2) < words.size(); i+=3)
	{
		//define vertex_info object that we're filling to add the vector v
		vertex_info node;
		
		node.origin = words.at(i);	
		
		node.destination = words.at(i+1);		
		
		//distance needs to be converted from a string to an integer
		//string stream object that holds the string
		stringstream s (words.at(i+2));

		//the integer we are feeding the string s into
		int d = 0;
		s >> d;

		node.distance = d;

		//push the node of data to our vector v
		v.push_back(node);
	}
}
//method to fill a vector of strings with the unique nodes in our graph
//of the unique values in the file
//precondition: pass in the vector with the file info that we are using to 
//fill our vector of unique characters
//postcondition: return the places vector is filled with unique places
vector <vertex_info> uniquePlaces(vector <vertex_info> v)
{
	//vector of vertex_info objects that will be filled as we implement 
	//Dijkstra's Algorithm
	vector <vertex_info> places;	

	//vector to hold all the unique vertices that we have
	vector <string> s;
	for (int i = 0; i < v.size(); i++)
	{
		s.push_back(v.at(i).destination);
		s.push_back(v.at(i).origin);
	}

	//sort vector s and delete duplicate origins/destinations so we have
	//a vector of unique vertices to work with.
	sort(s.begin(), s.end());
	s.erase(unique(s.begin(), s.end()), s.end());

	//Fill up our places vector with the destinations as the strings
	//stored in our s vector.
	for (int i = 0; i < s.size(); i++)
	{
		vertex_info node;
		node.destination = s.at(i);
		node.origin = "N/A";

		//initialize all our marks to false
		node.mark = false;

		//initialize all our distances to INT_MAX, since we haven't 
		//traveled anywhere yet.
		node.distance = INT_MAX;
		places.push_back(node);
	}

	return places;
}
//method to fill in our vertices for our graph
//precondition: pass in the graph object and the vector of vertex_info structs that has all the unique
//nodes in the file
//postcondition: return nothing, but all the vertices have been added to the graph
void fillVertices(Graph <string> &g, vector<vertex_info> place)
{
	for (int i = 0; i < place.size(); i++)
	{
		g.AddVertex(place.at(i).destination);
	}
}     

//method used to fill the edges matrix for our graph object
//precondition: pass in the graph object and the vector of vertex_info structs that has all the 
//origin/destination pairs from the file
//postcondition: return nothing, but all the edges have been added to the graph
void fillEdges(Graph <string> &g, vector<vertex_info> v)
{
	for (int i = 0; i < v.size(); i++)
	{
		g.AddEdge(v.at(i).origin, v.at(i).destination, v.at(i).distance);
	}
}

//method to fill our graph by calling the fillVertices and fillEdges methods
//precondition: pass in the graph object, vector of unique vertices, and vector of all origin/destination
//vertex pairs
//postcondition: return nothing, but the graph is filled with vertices and edges
void fillGraph(Graph<string> &g, vector<vertex_info> uniqueVals, vector <vertex_info> v)
{
	fillVertices(g, uniqueVals);
	fillEdges(g, v);	
}      

//method to return the index of a given vertex we're looking for
//precondition: pass in our vector of unique vertices and the vertex we're searching for
//postcondition: return the position (an integer) for the vertex in the vector of unique vertices
int findVertex(vector<vertex_info> uniqueVals, string vertex)
{
	//instantiate this to -1 so that if it isn't found, we'll return -1
	int location = -1;
	for (int i = 0; i < uniqueVals.size(); i++)
	{
		if(uniqueVals.at(i).destination.compare(vertex) == 0)
		{
			location = i;			
			break;
		}
	}	
	return location;
}


//method that actually implements Dijkstra's algorithm to find the shortest path through the graph
//precondition: pass in our graph object, vector of unique vertives, and the vertex we're starting at
//postcondition: return nothing, but our vector of unique vertices is completely full 
void dijkstra (Graph <string> &g, vector <vertex_info> &uniqueVals, string start_vertex)
{
	Queue <string> q(uniqueVals.size());

	//Vector to hold the index of the vector we are marking in the order
	//they are being marked; so, the integer in index 0 will be the index of 
	//the first struct in the array of vertex_info structs that was marked, 
	//and so on.
	vector <int> marked_indexes;

	//start by dealing with first/starting vertex
	
	//mark the first vertex
	g.MarkVertex(start_vertex);	

	//find the location of the start_vertex in vector uniqueVals
	int index = findVertex(uniqueVals, start_vertex);	

	//find the first vertex (selected by user) in our vector of structs
	//and instantiate the previous vector to "N/A"
	uniqueVals.at(index).origin = "N/A";
	uniqueVals.at(index).distance = 0;		
	
	//add the index of the start_vertex in the uniqueVals 
	//vector to our marked_indexes array to know that this
	//value was marked first.
	marked_indexes.push_back(index);
	
	//mark the adjacent vertex for our starting vertex and return the number of adjacent vertices
	int numAdjacentVertices = adjacentDistUpdate(g, uniqueVals, start_vertex, marked_indexes);

	if(numAdjacentVertices != 0)
	{
		//iterate through all our uniqueVals in the vector
		for (int i = 0; i < uniqueVals.size(); i++)
		{
			//our current index is the index of the most recently marked vertex
			int current_index = marked_indexes.at(marked_indexes.size()-1);
			string current = uniqueVals.at(current_index).destination;
			adjacentDistUpdate(g, uniqueVals, current, marked_indexes);	
			markVertex(g, uniqueVals, marked_indexes);		
		}
	}
}

//method to update the distances and origins of the current vertex's adjacent vertices; returns the number of adjacent vertices
//precondition: pass in our graph object, vector of unique vertices, the current vertex we're looking at,
//and the vector of the indexes of previously marked vertices
//postcondition: return the number of adjacent vertices to our current vertex and update the distances of 
//the vertices in our vector of unique values
int adjacentDistUpdate(Graph<string> g, vector<vertex_info> &uniqueVals, string current, 
		  vector <int> &marked_indexes)
{
	int numAdjacentVertices = 0;
	Queue <string> q(uniqueVals.size());

	//fill queue with adjacent vertices to the start_vertex
	g.GetToVertices(current, q);
	while(!q.isEmpty())
	{
		string front = q.getFront();
		
		int weight_graph = g.WeightIs(current, front);
		
		//get the index of "front" in uniqueVals and its weight in uniqueVals
		int front_index = findVertex(uniqueVals, front);
		int weight_vector = uniqueVals.at(front_index).distance;
	
		//get the index of the last marked index and its weight in uniqueVals
		int last_marked_index = marked_indexes.at(marked_indexes.size()-1);
		int weight_marked = uniqueVals.at(last_marked_index).distance;

		//add the weight_graph value to the distance of the last marked vertex
		int sum = weight_graph + weight_marked;

		//if the adjacent vertex is currently unmarked and its distance
		//value in local vector is greater than the sum of the weight
		//value plus the distance of the last marked vertex or the distance is 
		if (!g.IsMarked(front) && ((weight_vector > sum) || weight_vector == INT_MAX))
		{
			//reset distance value of adjacent vertex to the smaller sum and 
			//store current vertex as the previous vertex of the adjacent vertex
			uniqueVals.at(front_index).distance = sum;
			uniqueVals.at(front_index).origin = current;			
		}
		q.dequeue();
		numAdjacentVertices += 1;
	} 
	q.makeEmpty();

	return numAdjacentVertices;
}

//method to merk the vertex with the smallest non-zero/non-negative distance
//precondition: pass in our graph object, the vector of unique vertices, and the vector of indexes of 
//previously marked vertices
//postcondition: update our markedIndexes vector to have the index of the vertex we just marked
void markVertex(Graph<string> &g, vector<vertex_info>&uniqueVals, vector<int> &markedIndexes)
{
	//sort vector of structs by the destination
	sort(uniqueVals.begin(), uniqueVals.end(), [] (const vertex_info &left, const vertex_info &right)
	{
		return (left.distance < right.distance);
	});
	
	for (int i = 0; i < uniqueVals.size(); i++)
	{
		if(!g.IsMarked(uniqueVals.at(i).destination) && uniqueVals.at(i).distance != INT_MAX && uniqueVals.at(i).distance != 0)
		{
			g.MarkVertex(uniqueVals.at(i).destination);
			uniqueVals.at(i).mark = true;
			markedIndexes.push_back(i);
			break;				
		}
	}
}
//method to help with the proper printing output for strings
//precondition: pass in the string we're looking at
//postcondtion: return the number of spaces needed to be printed before the string so it
//takes up a total of 30 characters
int s_numSpaces(string vertex)
{
	int numLetters = vertex.size();
	return 30 - numLetters;
}

//method to help with the proper printing output for integers
//precondition: pass in the integer we're looking at
//postcondtion: return the number of spaces needed to be printed before the integer so it
//takes up a total of 30 characters
int n_numSpaces(int value)
{
	//if value == 0; then log10 is undefined, which is why we have a special case for it
	double numDigits = (value == 0 ? 1 : floor(log10(value)) + 1.0);	
	return 30 - numDigits;	
}

//method to print out a given number of spaces
//precondition: pass in the number of spaces we want printed
//postcondition: return nothing, but the spaces are printed
void printSpaces(int numSpaces)
{
	for (int i = 0; i < numSpaces; i++)
		cout << " ";
}

//method to print out our vector of unique vertices
//precondition: pass in our vector of unique vertices
//postcondition: return nothing, but a table is printed in order from least - greatest distance
void printArray(vector<vertex_info> uniqueVals)
{	
	//sort vector of structs by the destination
	sort(uniqueVals.begin(), uniqueVals.end(), [] (const vertex_info &left, const vertex_info &right)
	{
		return (left.distance < right.distance);
	});
	cout << "\t\t--------------------------------------------------------------------------" << endl;
		
	//get the proper spacing for printing:
	int numSpaces;
	printSpaces(24);
	cout << "Vertex";
	printSpaces(22);
	cout << "Distance";
	printSpaces(22);
	cout << "Previous\n" << endl;

	for (int i = 0; i < uniqueVals.size(); i++)
	{
		numSpaces = s_numSpaces(uniqueVals.at(i).destination);
		printSpaces(numSpaces);
		cout << uniqueVals.at(i).destination;

		if(uniqueVals.at(i).distance == INT_MAX)
		{
			printSpaces(19);
			cout << "Not On Path";
		}
		else
		{
			numSpaces = n_numSpaces(uniqueVals.at(i).distance);
			printSpaces(numSpaces);
			cout << uniqueVals.at(i).distance;
		}
		numSpaces = s_numSpaces(uniqueVals.at(i).origin);
		printSpaces(numSpaces);
		cout << uniqueVals.at(i).origin;
		cout << endl;	
	}
	cout << "\t\t--------------------------------------------------------------------------" << endl;
}

//for depth first search, mark each adjacent vertex as you visit it
//pop, output, mark, and push adjacent vertices
//1. add first vertex to the stack; pop it, output it, and mark it (in graph class)
//2. push the adjacent vertices to the stack; pop the top value, output it, mark it,
//and don't repush the marked vertex
//3. keep repeating until we find a vertex with no adjacent vertices, or until we've
//reached our starting vertex
bool hasCycle(Graph<string> &g, vector <vertex_info> v)
{
	//clear out the marks from when we used Dijstra's Algorithm with this graph
	g.ClearMarks();

	//instantiate a stack to store all the adjacent vertices of the current
	//vertex we're looking at
	stack <string> s;

	string start = v.at(0).destination;
	s.push(start);

	//vector of strings to hold all the vertices that make up the path of a cycle
	vector <string> path;
	
	//if hasCycle is greater than 0, then we have a cycle in our graph
	int hasCycle = depthfirstsearch(g, s, v, path);
	
	cout << "*******out of hasCycle method****************" << endl;
	if (hasCycle == 0)
	{	
		//make sure that all vertices were visited in our depthfirstsearch
		//store the number of unvisited vertices in an integer "counter"
		int counter;
		do
		{ 
			//go ahead and empty our stack (since we passed by value for the function,
			//the stack only has one element on it right now
			s.pop();
			
			//initialize the number of unvisited vertices to zero
			counter = 0;
			for (int i = 0; i < v.size(); i++)	
			{
				if(v.at(i).mark == false)
				{
					//go ahead and push the first value that isn't marked
					//to our stack to be passed into depthfirstsearch
					if (counter == 0)
						s.push(v.at(i).destination);
					counter += 1;
				}
			}
			if (counter > 0)
			{
				//clear our vector of strings for the path for a cycle too
				path.clear();			
				hasCycle = depthfirstsearch(g, s, v, path);
			}
		} while (counter > 0);
	}
	if (hasCycle > 0)
		printCycle(path);
	return (hasCycle > 0);	
}

//method that returns an integer that denotes if we have a cycle or not - any value greater than 0
//tells us that we have a cycle; anything that's 0 indicates that we don't have a cycle
//
//precondition: g = graph that we're passing in, s = the stack of strings, where we've already pushed
//the first value into it, which is the starting position, v = the vector of vertex_info objects so that
//we can mark vertices as we visit them, path = the vector of strings that will be filled with vertices on 
//the path to our cycle
//
//postcondition: return an integer that tells us if there's a cycle found from the depth first search; 
//0 = no cycle, anyting > 0 = has a cycle
int depthfirstsearch(Graph<string>&g, stack<string>s, vector <vertex_info> &v, vector <string> &path)
{
	int hasCycle = 0;	

	//create a queue that will be filled with adjacent vertices as we call 
	//getToVertices everytime we get to another vertex
	Queue <string> q (v.size());
	
	//create a vector with the adjacent vertices for a given vertex
	vector <string> adj;	

	//represents the number of adjacent vertices for a given vertex
	int numAdjacent;

	do	
	{
		numAdjacent = 0;
		adj.clear();

		//set the current vertex we're looking at to be the top of the stack
		string current = s.top();

		//this is needed because if a current vertex has already been marked, but we're 
		//going through this process again because of the recursive call, we don't want
		//it to accidentally tell us there's a cycle when there isn't
		if(!g.IsMarked(current))
		{
			//find index of vertex in our vector of <vertex_info> structs
			int index = findVertex(v, current);

			//mark the vertex in the graph
			g.MarkVertex(current);
			v.at(index).mark = true;
			path.push_back(current);
			s.pop();

			//fill queue with adjacent vertices to the start_vertex
			g.GetToVertices(current, q);

			while (!q.isEmpty())
			{
				string adjacent = q.getFront();
				if(!g.IsMarked(adjacent))
				{
					s.push(adjacent);
					adj.push_back(adjacent);
				}

				else
				{
					hasCycle += 1;
					return hasCycle;
				}
				numAdjacent += 1;
				q.dequeue();
			}
			
			if(hasCycle == 0 && numAdjacent != 0)
			{
				for (int i = 0; i < adj.size(); i++)
				{
					int index = findVertex(v, adj.at(i));
				
					//if we haven't visited an adjacent vertex
					if (!g.IsMarked(v.at(index).destination))
					{
						hasCycle += depthfirstsearch(g,s,v,path);
					}			
				}
			}	
		}
	} while (s.size() != 0 and numAdjacent != 0);

	return hasCycle;	
}

//method to print out the cycle of a graph
//precondition: pass in the vector of strings that make up the path of a cycle
//postcondition: return nothing, but the path is printed
void printCycle(vector <string> path)
{
	cout << "The Cycle is: " << endl;
	cout << "Vertex" << " To" << endl;
	for (int i = 0; (i+1) < path.size(); i++)
	{
		cout << path.at(i) << "->" << path.at(i+1) << endl; 
	}
	cout << path.at(path.size()-1) << "->" << path.at(0) << endl;
}

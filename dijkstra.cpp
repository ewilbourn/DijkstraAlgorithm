//Emily Wilbourn
//11-27-2020
//Program 4 - CSC 245
#include "graph.h" /*graph object*/
#include "queue.h" /*queue object*/
#include <string>
#include <iostream> /*cout & cin*/
#include <fstream> /*for reading in data from a given file*/
#include <sstream> /*for converting string to integer in my initializeArray method*/
#include <algorithm> /*used for getting a vector of unique values in uniquePlaces method and for deleting a specific value from our cycle vector*/ 
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
vector <string> hasCycle(Graph<string> &g, vector <vertex_info> v);
int depthfirstsearch(Graph<string>&g, stack<string>s, stack<string>&s_final,vector <vertex_info> &v, vector <string> &path);
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
	//string startingVertex;
	char startingVertex[30];
	cout << "\n\t\tPlease input your starting vertex: ";
//	cin >> startingVertex;
	cin.getline(startingVertex,30);

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

	vector <string> cyclePath = hasCycle(myGraph, cycleVals);

	if(cyclePath.size() > 0)
	{
		printSpaces(23);
		cout << "\t\tThe graph contains a cycle." << endl;
		printCycle(cyclePath);	
	}
	else
	{
		printSpaces(19);
		cout << "\t\tThe graph does not contain a cycle." << endl;
	}
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

//method that returns a vector of strings that has our cycle path in it; if the size of this vector is
//zero, then we don't have a cycle. Else, we have a cycle.
//precondition: pass in our graph object and the vector of vertex_info structs that has all our vertices
//postcondition: return a vector of strings that contains our cycle path
vector <string> hasCycle(Graph<string> &g, vector <vertex_info> v)
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
	
	//stack to hold the path of the final vertex
	stack <string> s_final;
	
	//if hasCycle is greater than 0, then we have a cycle in our graph
	int hasCycle = depthfirstsearch(g, s, s_final, v, path);

	//If the graph isn't connected, then there's a chance that on the first pass we
	//won't find a cycle. If this is the case, then keep calling depthfirstsearch until
	//all vertices have been visited.	
	if (hasCycle == 0)
	{
		//make sure that all vertices were visited in our depthfirstsearch
		//store the number of unvisited vertices in an integer "counter"
		int counter;
		do
		{ 
			//clear out the marks from when we used Dijstra's Algorithm with this graph
			//g.ClearMarks();

			//empty the stack
			while (!s.empty())
				s.pop();
			
			//empty the stack
			while (!s_final.empty())
				s_final.pop();
	
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
				//else
				//	v.at(i).mark = true;
			}
			if (counter > 0)
			{
				//clear our vector of strings for the path for a cycle too
				path.clear();			
				hasCycle = depthfirstsearch(g, s, s_final, v, path);
			}
		} while (counter > 0);
	}

	return path;	
}

//method that returns an integer that denotes if we have a cycle or not - any value greater than 0
//tell us that we have a cycle; anything that's 0 indicates that we don't have a cycle
//
//precondition: g = graph that we're passing in, s = the stack of strings, where we've already pushed
//the first value into it, which is the starting position, v = the vector of vertex_info objects so that
//we can mark vertices as we visit them, path = the vector of strings that will be filled with vertices on 
//the path to our cycle
//
//postcondition: return an integer that tells us if there's a cycle found from the depth first search;
//0 = no cycle, anything > 0 = has a cycle
int depthfirstsearch(Graph<string>&g, stack<string>s, stack <string> &s_final, vector <vertex_info> &v, vector <string> &path)
{
	//integer that represents if we have a cycle or not; if this value stays 0,
	//then we have a cycle. If it's greater than 0, then we have a cycle.
	int hasCycle = 0;	

	//create a queue that will be filled with adjacent vertices as we call 
	//getToVertices everytime we get to another vertex
	Queue <string> q (v.size());


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

		//go ahead and push the current vertex to our s_final stack, which
		//will fill up our path vector, which will be printed our at the very 
		//end with a cycle that was found in the graph.
		s_final.push(current);			

		//fill queue with adjacent vertices to the start_vertex
		g.GetToVertices(current, q);
	
		//go ahead and pop the top, since we've already visited it.
		s.pop();
			
		//This runs while we still have adjacent vertices in our queue, that was filled by 
		//our call to GetToVertics in the graph class.
		while (!q.isEmpty())
		{
			string adjacent = q.getFront();
				
			//If an adjacent vertex hasn't been visited, then go ahead and push it to
			//the stack and call depthfirstsearch (recursive call) so that we can see
			//if there is a cycle down that path.
			if(!g.IsMarked(adjacent))
			{
				s.push(adjacent);
				hasCycle += depthfirstsearch(g,s,s_final,v,path);
			}
					
			//We have to include path.size() == 0 because if the program finds a second 
			//cycle after finding the initial one, we don't want it to mess up our path
			//vector. We are only printing out the first cycle the program finds.
			else if (g.IsMarked(adjacent) && hasCycle == 0 && path.size() == 0)
			{
				s_final.push(adjacent);
					
				hasCycle += 1;
				int size = 0;	
				while (!s_final.empty())
				{
					path.push_back(s_final.top());
					s_final.pop();
					size+=1;
				}
			
				//We need this case to fix the printing for ginfile4.dat.
				//This essentially says that if the first value in the path (which 
				//is at path.size()-1, because we're printing from a stack which is 
				//LIFO) is not the same value as the adjacent value that ends the 
				//cycle (which should be the first value in the path), then delete it
				//from the path vector.
				if (path.at(path.size()-1) != adjacent)
					path.erase(remove(path.begin(), path.end(), path.at(path.size()-1)), path.end());
				return hasCycle;
			}
			q.dequeue();
		}
	}
	
	return hasCycle;	
}

//method to print out the cycle of a graph
//precondition: pass in the vector of strings that make up the path of a cycle
//postcondition: return nothing, but the path is printed
void printCycle(vector <string> path)
{
	string title1 = "The Cycle is:";
	string title2 = "Vertex";
	string title3 = "To";
	
	//variables to hold the number of spaces needed to precede a string
	int numSpaces;
	
	numSpaces = s_numSpaces(title1);
	printSpaces(numSpaces);
	cout << title1 << endl;

	numSpaces = s_numSpaces(title2);
	printSpaces(numSpaces);
	cout << title2;

	numSpaces = s_numSpaces(title3) + s_numSpaces("->");
	printSpaces(numSpaces);
	cout << title3 << endl;
	
	for (int i = (path.size()-1); i > 0; i--)
	{
		numSpaces = s_numSpaces(path.at(i));
		printSpaces(numSpaces);
		cout << path.at(i);

		numSpaces = s_numSpaces("->");
		printSpaces(numSpaces);
		cout << "->";
	
		numSpaces = s_numSpaces(path.at(i-1));
		printSpaces(numSpaces);
		cout << path.at(i-1) << endl;
	}
}

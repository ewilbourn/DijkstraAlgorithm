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
 
struct vertex_info
{
	string origin;
	bool mark;
	int distance;
	string destination;
};

void printInfo(vector<vertex_info> uniqueNodes, int numVertices);
void initializeArray(/*Graph<string> &g, */vector <vertex_info> &v, string fileName);
vector <vertex_info> uniquePlaces(vector <vertex_info> v);
void fillVertices(Graph <string> &g, vector<vertex_info> place);
void fillGraph(Graph<string> &g, vector <vertex_info> uniqueVals, vector <vertex_info> v);
void fillEdges(Graph <string> &g, vector<vertex_info> v);
void dijkstra (Graph<string> &g,vector <vertex_info> uniqueVals, string start_vertex);
void printArray(vector<vertex_info> uniqueVals);

int main(int argc, char *argv[])
{
	//Instantiate a vector of vertex_info objects.
	//This vector will hold all the original Origin/Destination/Distance 
	//values from our input file.
	vector <vertex_info> v;

	//Get the file name with the graph data from user input on command line
	string fileName = argv[1];

	//call method to initialize our array of vertex_info objects
	initializeArray(v, fileName);	

	//Create vector that we will update with vertex_info structs as we apply
	//Dijkstra's algorithm.
	vector <vertex_info> uniqueVals = uniquePlaces(v);

	//Get the number of vertices in our graph; this is the size of our
	//vector v because each object in v is a different vertex.
	int numVertices = uniqueVals.size();

	//print out the title for the program and the given, unique vertices in the input file
	printInfo(uniqueVals, numVertices);

	//get the starting position for the graph
	string startingVertex;
	cout << "\n\t\tPlease input your starting vertex: ";
	cin >> startingVertex;

	//instantiate our graph and queue objects
	Graph<string> myGraph(numVertices);
	
	fillGraph(myGraph, uniqueVals, v);
	dijkstra(myGraph, uniqueVals, startingVertex);	
//	Queue<string> myQ(numVertices);
	return 0;
}

//method to print the "Dijksta's Algorithm" title and all the vertices given in the input file
void printInfo(vector<vertex_info> uniqueNodes, int numVertices)
{
	cout << "\t\t^^^^^^^^^^^^^^^^    DIJKSTRA'S ALGORITHM    ^^^^^^^^^^^^^^^^\n" << endl;
        cout << "\t\tA Weighted Graph Has Been Built For These " << numVertices << "  Cities :\n" << endl;

	for(int i = 0; (i+2) < uniqueNodes.size(); i+=3)
	{
		cout << "\t\t" << uniqueNodes.at(i).destination << "\t\t\t" << uniqueNodes.at(i+1).destination << "\t\t\t" << uniqueNodes.at(i+2).destination << endl;
	}

	//since we're printing in groups of 3's, if our number of vertices is not a multiple of 3, we need two
	//cases to handle printing the left over vertices.
	if(uniqueNodes.size() % 3 == 1)
	{
		cout << "\t\t" << uniqueNodes.at(numVertices-1).destination << endl;
	}

	else if(uniqueNodes.size() % 3  == 2)
	{
		cout << "\t\t" << uniqueNodes.at(numVertices-2).destination << "\t\t\t" << uniqueNodes.at(numVertices-1).destination << endl;
	}
}

//precondition: v is empty
//postcondition: nothing is returned, but v is filled
void initializeArray(/*Graph<string> &g, */vector <vertex_info> &v, string fileName)
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

	//sort vector of structs by the destination
	sort(v.begin(), v.end(), [] (const vertex_info &left, const vertex_info &right)
	{
		return (left.destination < right.destination);
	});

	for (int i = 0; (i+1) < v.size(); i++)
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
		node.origin = "";

		//initialize all our marks to false
		node.mark = false;

		//initialize all our distances to -1 so we know we haven't 
		//visited it yet.
		node.distance = -1;
		places.push_back(node);
	}

	return places;
}
//method to fill in our vertices for our graph
//precondition: pass in the graph object     
void fillVertices(Graph <string> &g, vector<vertex_info> place)
{
	for (int i = 0; i < place.size(); i++)
	{
		g.AddVertex(place.at(i).destination);
	}
}     

//method used to fill the edges matrix for our graph object
void fillEdges(Graph <string> &g, vector<vertex_info> v)
{
	for (int i = 0; i < v.size(); i++)
	{
		g.AddEdge(v.at(i).origin, v.at(i).destination, v.at(i).distance);
	}
}

//method to fill our graph  
void fillGraph(Graph<string> &g, vector<vertex_info> uniqueVals, vector <vertex_info> v)
{
	fillVertices(g, uniqueVals);
	fillEdges(g, v);	
}      

//method to return the index of a given vertex we're looking for
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

void dijkstra (Graph <string> &g, vector <vertex_info> uniqueVals, string start_vertex)
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

	//find the first vertex (selected by user) in our vector of structs
	//and instantiate the previous vector to "N/A" and the distance to 0
	for(int i = 0; i < uniqueVals.size(); i++)
	{
		if (uniqueVals.at(i).destination.compare(start_vertex) == 0)
		{
			uniqueVals.at(i).distance = 0;
			uniqueVals.at(i).origin = "N/A";
		
			//add the index of the start_vertex in the uniqueVals 
			//vector to our marked_indexes array to know that this
			//value was marked first.
			marked_indexes.push_back(i);
			break;
		}		
	}
	printArray(uniqueVals);	
	//fill queue with adjacent vertices to the start_vertex
	g.GetToVertices(start_vertex, q);
	while(!q.isEmpty())
	{
		string front = q.getFront();
		
		//if the adjacent vertex is currently unmarked and its distance
		//value in local vector is greater than the sum of the weight
		//value plus the distance of the last marked vertex	
	} 
}

void printArray(vector<vertex_info> uniqueVals)
{
	cout << "------------------------------------------------------------------" << endl;
	cout << "	   Vertex		 Distance		   Previous" << endl;
	for (int i = 0; i < uniqueVals.size(); i++)
	{
		cout << uniqueVals.at(i).destination << "\t\t" << uniqueVals.at(i).distance << "\t\t" << 
		uniqueVals.at(i).origin << endl;	
	}
	cout << "------------------------------------------------------------------" << endl;
}

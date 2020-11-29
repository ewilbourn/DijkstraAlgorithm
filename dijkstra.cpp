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
void fillEdges(Graph <string> &g, vector<vertex_info> v);
void dijkstra (Graph<string> &g,vector <vertex_info> uniqueVals, string start_vertex);
void markAdjacent(Graph<string> g, vector<vertex_info> &uniqueVals, string current,
		  vector<int> &marked_indexes);
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

	//call to fill the vertices/edges of the graph	
	fillGraph(myGraph, uniqueVals, v);

	//call to perform Dijkstra's Algorithm given the starting vertex
	dijkstra(myGraph, uniqueVals, startingVertex);	
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

		//initialize all our distances to -1, since we haven't 
		//traveled anywhere yet.
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
	
	//mark the adjacent vertex for our starting vertex
	markAdjacent(g, uniqueVals, start_vertex, marked_indexes);

	//iterate through all our uniqueVals in the vector
	for (int i = 0; i < uniqueVals.size(); i++)
	{
		//our current index is the index of the most recently marked vertex
		int current_index = marked_indexes.size()-1;
		string current = uniqueVals.at(current_index).destination;

		markAdjacent(g, uniqueVals, current, marked_indexes);		
	}
	printArray(uniqueVals);	
}

//method to mark the adjacent matrix and update the uniqueVals
void markAdjacent(Graph<string> g, vector<vertex_info> &uniqueVals, string current, 
		  vector <int> &marked_indexes)
{
	Queue <string> q(uniqueVals.size());
	//fill queue with adjacent vertices to the start_vertex
	g.GetToVertices(current, q);
	cout << "current: " << current << endl;

	while(!q.isEmpty())
	{
		string front = q.getFront();
	//	cout << "front: " << front << endl;		
		
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
		//-1, meaning it hasn't been visited...	
		// 0 = false, 1 = true
	//	cout << "front is Marked? " << g.IsMarked(front) << endl;	
	//	cout << "weight_vector: " << weight_vector << endl;
	//	cout << "sum: " << sum << endl;
		if (!g.IsMarked(front) && ((weight_vector > sum) || weight_vector == -1))
		{
			//reset distance value of adjacent vertex to the smaller sum and 
			//store current vertex as the previous vertex of the adjacent vertex
			uniqueVals.at(front_index).distance = sum;
			uniqueVals.at(front_index).origin = current;			
			uniqueVals.at(front_index).mark = true;
			
			//push the current front_index to our marked indexes vector
			marked_indexes.push_back(front_index);
			break;
		}
		q.dequeue();
	} 
	q.makeEmpty();
}

//method to help with the proper printing output for strings
int s_numSpaces(string vertex)
{
	int numLetters = vertex.size();
	return 30 - numLetters;
}

//method to help with the proper printing output for integers
int n_numSpaces(int value)
{
	//if value == 0; then log10 is undefined, which is why we have a special case for it
	double numDigits = (value == 0 ? 1 : floor(log10(value)) + 1.0);	
	if (value == -1) numDigits = 2;	
	return 30 - numDigits;	
}

void printSpaces(int numSpaces)
{
	for (int i = 0; i < numSpaces; i++)
		cout << " ";
}

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

		numSpaces = n_numSpaces(uniqueVals.at(i).distance);
		printSpaces(numSpaces);
		cout << uniqueVals.at(i).distance;

		numSpaces = s_numSpaces(uniqueVals.at(i).origin);
		printSpaces(numSpaces);
		cout << uniqueVals.at(i).origin;
		cout << endl;	
	}
	cout << "\t\t--------------------------------------------------------------------------" << endl;
}

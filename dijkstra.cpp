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

void printInfo(vector<string> uniqueNodes, int numVertices);
void initializeArray(vector <vertex_info> &v, string fileName);
vector <string> uniquePlaces(vector <vertex_info> v);
void fillVertices(Graph <string> &g, vector<string> place);
void fillGraph(Graph<string> &g, vector<string> place, string start_pos);
void fillEdges(Graph <string> &g, vector<vertex_info> v);

int main(int argc, char *argv[])
{
	//instantiate a vector of vertex_info objects
	vector <vertex_info> v;

	string fileName = argv[1];

	//call method to initialize our array of vertex_info objects
	initializeArray(v, fileName);
	vector <string> uniqueNodes = uniquePlaces(v);

	//get the number of vertices in our graph; this is the size of our
	//vector v because each object in v is a different vertex
	int numVertices = uniqueNodes.size();

	printInfo(uniqueNodes, numVertices);
	//get the starting position for the graph
	string startingVertex;
	cout << "Please input your starting vertex: ";
	cin >> startingVertex;

	//instantiate our graph and queue objects
	Graph<string> myGraph(numVertices);
	Queue<string> myQ(numVertices);
	return 0;
}

//method to print the "Dijksta's Algorithm" title and all the vertices given in the input file
void printInfo(vector<string> uniqueNodes, int numVertices)
{
	cout << "^^^^^^^^^^^^^^^^    DIJKSTRA'S ALGORITHM    ^^^^^^^^^^^^^^^^\n" << endl;
        cout << "A Weighted Graph Has Been Built For These " << numVertices << "  Cities :" << endl;

	for(int i = 0; (i+2) < uniqueNodes.size(); i+=3)
	{
		cout << uniqueNodes.at(i) << "\t" << uniqueNodes.at(i+1) << "\t" << uniqueNodes.at(i+2) << endl;
	}

	//since we're printing in groups of 3's, if our number of vertices is not a multiple of 3, we need two
	//cases to handle printing the left over vertices.
	if(uniqueNodes.size() % 3 == 1)
	{
		cout << uniqueNodes.at(numVertices-1) << endl;
	}

	else if(uniqueNodes.size() % 3  == 2)
	{
		cout << uniqueNodes.at(numVertices-2) << "\t" << uniqueNodes.at(numVertices-1) << endl;
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
vector <string> uniquePlaces(vector <vertex_info> v)
{
	   vector <string> places;
        for (int i = 0; i < v.size(); i++)
        {
                //add all of the locations to the places vector    
        	places.push_back(v.at(i).origin);
		places.push_back(v.at(i).destination);
        }
        sort(places.begin(), places.end());
        places.erase(unique(places.begin(), places.end()), places.end());
	
	return places;
}

//method used to fill in our vertices for our graph
//precondition: pass in the graph object     
void fillVertices(Graph <string> &g, vector<string> place)
{
	for (int i = 0; i < place.size(); i++)
	{
		g.AddVertex(place.at(i));
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

void fillGraph(Graph<string> &g, vector<string> place, string start_pos)
{
	fillVertices(g, place);
	//mark the first vertex
		
}      



//Emily Wilbourn
//11-27-2020
//Program 4 - CSC 245
#include "graph.h"
#include "queue.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

struct vertex_info
{
	string origin;
	bool mark;
	int distance;
	string destination;
};

void initializeArray(vector <vertex_info> &v, string fileName);

int main(int argc, char *argv[])
{
	Graph<string> myGraph(50);
	Queue<string> myQ(50);
	vector <vertex_info> v;

	string fileName = argv[1];

	initializeArray(v, fileName);
	return 0;
}

//read in the data file and fill each slot in array v with 
//vertex_info structs

//precondition: v is empty
//postcondition: nothing is returned, but v is filled
void initializeArray(vector <vertex_info> &v, string fileName)
{
	vector <string> words;
	string line;
	string word;
	ifstream input(fileName);
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
				cout << word << endl;
			}
		}
		input.close();
	}

	cout << words.size() << endl;
	for (int i = 0; (i+2) < words.size(); i+=3)
	{
		cout << "top of loop" << endl;	
		vertex_info node;
		
		node.origin = words.at(i);		
		cout << "origin: " << node.origin << endl;
		
		node.destination = words.at(i+1);
		cout << "destination: " <<  node.destination << endl;
	
		//distance needs to be converted from a string to an integer
		//string stream object that holds the string
		stringstream s (words.at(i+2));

		//the integer we are feeding the string s into
		int d = 0;
		s >> d;

		node.distance = d;
		cout << "distance: " << node.distance << endl;

		//push the node of data to our vector v
		v.push_back(node);
		cout << "end of loop" << endl;
		cout << endl;
	}
}



//Emily Wilbourn
//11-27-2020
//Program 4 - CSC 245
#include "graph.h"
#include "queue.h"
#include <string>
#include <iostream>
#include <fstream>

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
	string line;
	ifstream input(fileName);
	if (input.is_open())
	{
		while (getline (input, line, ';'))
		{	
			cout << line << endl;
		}
		input.close();
	}
}



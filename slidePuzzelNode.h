
// This is the header file for the nodes used to perform searches on a slider puzzel

#ifndef SLIDEPUZZELNODE_H
#define SLIDEPUZZELNODE_H
#include <vector>
#include <string>

using namespace std;

struct slidePuzzelNode
{
	// Array to hold the current state
	int tilePositions[9];

	// string to hold string representation of the puzzle board
	string positions;

	// Int to hold how deep the node is in the tree
	int depth;

	// vector to hold the moves used to get to the end
	vector<int> moveOrder;

	// Move count
	int nodeMoveCount;

	// Heuristic Information (Missplaced tiles or Manhattan distance
	int heurisiticInfo;

	// function to return if the heuristic information in the current node is less than that of another
	bool operator < (const slidePuzzelNode other)const
	{
		return heurisiticInfo > other.heurisiticInfo;
	}

};


#endif

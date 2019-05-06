/*
	Tyler McCaleb
	Slider Puzzel Solver
	10/14/2018

	This program will attempt to solve an eight piece slider puzzel.  The puzzels will either be inputted by the user or randomly generated.
	The program will not allow the user to input duplicate numbers in to the puzzel.  It will also check to see if a randomly generated maze
	is viable, however it will stil attempt to solve an unviable maze.  The program will attempt to solve the puzzel using 4 types of searches
	(Breadth First, Depth First, Number of Misplaced Tiles, and the Manhattan distance.  The program will also time each of these runs and output
	the results to a .csv file for future reference.

*/

#include <iostream>
#include <fstream>
#include <stack>
#include <queue>
#include <cstdlib>
#include <ctime>
#include "slidePuzzelNode.h"
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>


using namespace std;

// Array Size Variable
const int arraySize = 9;

// create the map
map<string, int> visitedMap;

// Function Prototypes


//********************* SEARCH FUNCTIONS ********************************

// Function to perform a breadth first search
void breadthFirstSearch(int slidePuzzelArray[arraySize]);

// Function to perform a depthFirst Search
void depthFirstSearch(int slidePuzzleArray[arraySize]);

// Function to perform a breadth first search
void missingTilesSearch(int slidePuzzelArray[arraySize]);

// Function to perform a search based on the Manhattan Distance
void manhattanDistanceSearch(int slidePuzzelArray[arraySize]);



//*************** FUNCTIONS TO CREATE PUZZLE BOARDS ********************

// Function to create a random puzzel
void createRandomPuzzel(int sliderPuzzelArray[arraySize]);

// Function to create a user made puzzel
void createUserPuzzel(int sliderPuzzelArray[arraySize]);

// Function to check and see if a tile is in the puzzel
bool tileInPuzzel(int sliderPuzzelArray[arraySize], int checkedTile);

// Function to make sure the user only enter a number into the tile once
int createTile(int userInput[arraySize]);



//******************* FUNCTIONS USED IN SEARCHES ***********************

// Function to choose the type of search the user wishes to perform on the board
void chooseSearch(int sliderPuzzelArray[arraySize]);

// Function to reinitialize the puzzel for another run
void initializePuzzelBoard(int sliderPuzzelArray[arraySize]);

// Function to generate moves depending on where the 0 is.
void generateMoves(int currentState[arraySize], vector<int> &moves);

// Function to make make moves on the puzzel board
void makeMove(int nextState[arraySize], int moveNumber, int currentState[arraySize]);

// function to create a string out of a puzzel board
string createStringRepresentation(int currentState[arraySize]);

// Function to see if the current state was already visited
bool stateVisited(string currentPosition);

// Function to print the puzzel board
void printPuzzelBoard(int sliderPuzzelArray[arraySize]);

// Function to print the results of the search
void printResults(slidePuzzelNode endingNode, int startingBoard[arraySize]);

// Function to calculate the number of misplaced tiles in a board
int calculateMisplacedTiles(int puzzleState[arraySize]);

// Function to calculate the manhattan distance of a board
int calculateManhattanDistance(int puzzleState[arraySize]);

// function to calculate the number of misplaced tiles in a board
int calculateMisplacedTileTest(int startingArray[arraySize], int goalArray[arraySize]);



//*************** FUNCTIONS TO CHECK FOR ERRORS ******************************

// Function to check the input from the user
int checkInput(int location);



//************ FUNCTIONS TO PERFORM TEST RUNS ON THE SEARCHES****************

// Function to start the test runs of each search
void testRuns();

// Function to perform a breadth first search (Just the search with no output - used to pull information)
void breadthFirstTestRun(int slidePuzzelArray[arraySize], ofstream &output);

// Function to perform a depthFirst Search  (Just the search with no output - used to pull information)
void depthFirstTestRun(int slidePuzzleArray[arraySize], ofstream &output);

// Function to perform a breadth first search  (Just the search with no output - used to pull information)
void missingTilesTestRun(int slidePuzzelArray[arraySize], ofstream &output);

// Function to perform a search based on the Manhattan Distance  (Just the search with no output - used to pull information)
void manhattanDistanceTestRun(int slidePuzzelArray[arraySize], ofstream &output);

// Function to print a description of the moves made to the file
void printMoves(vector<int> moves, ofstream &output);




int main()
{
	// boolean to flag if the user wants to exit the program
	bool exitProgram = false;

	// integer to hold the user input for what type of puzzle they want to do
	int puzzleType = 0;

	// array to hold the user create or randomly created puzzle
	int slidePuzzel[arraySize];

	// Seed the random function
	srand(time(NULL));
	
	// while loop to run the menue
	while (exitProgram != true)
	{
		// clear any entries in the map from previous puzzle runs.
		visitedMap.clear();

		// ask the user if they would like to create the puzzel or use a randomly generated one
		cout << "\nWould you like to create a puzzle to be solved or use a randomly generated one?" << endl;
		cout << "Options:\n1.\tCreate a puzzle.\n2.\tRandomly Generate a puzzle.\n3.\tPerform search test runs.\n4.\tExit the Program." << endl;
		puzzleType = checkInput(1);
		cin.ignore(10000, '\n');

		// switch case to handle the user's choice
		switch (puzzleType)
		{
		case 1:
			// call function to begin creating a puzzel
			createUserPuzzel(slidePuzzel);

			// Choose the search to perfrom on the puzzel
			chooseSearch(slidePuzzel);
			break;

		case 2:
			// call function to randomly generate a puzzel
			createRandomPuzzel(slidePuzzel);

			// Choose the serach to perform on the puzzel
			chooseSearch(slidePuzzel);
			break;

		case 3:
			// run the simulation and output the results to the file
			cout << "Starting the simulation" << endl;
			testRuns();
			break;

		case 4:
			exitProgram = true;
			break;
		}

	}
	   	 
	system("CLS");
	cout << "Press Enter to exit..." << endl;
	cin.get();

	return 0;
}


// Function Definitions

//********************* SEARCH FUNCTIONS ********************************

// Function to perform a breadth first search
void breadthFirstSearch(int slidePuzzelArray[arraySize])
{
	// Create Queue
	queue<slidePuzzelNode> nodeQueue;

	// Vector to be used to store the moves to be done
	vector<int> moves;

	// Count to hold the depth of the node
	int depthCount = 0;

	// Integer to hold user input to view results
	int viewResults = 0;

	// boolean to hold if the puzzle was solved
	bool solved = false;

	// array to hold a temporary state
	int tempArray[arraySize] = { 0 };

	// integer to hold how many nodes have been expanded
	int nodesExpanded = 0;

	// string to hold the string representaiton of a generated state
	string moveState;

	// integer to accompany the strings in the map
	int mapCount = 1;

	// array to hold the the goal state
	int goalArray[arraySize] = { 1, 2, 3, 4, 5, 6, 7, 8, 0 };

	// array to hold onto the tileArray in the front of the queue
	int currentFrontArray[arraySize];
	
	// Create the first state.
	slidePuzzelNode puzzleState;

	// Copy the initial state into a working state
	// copy the tile positions
	for (int i = 0; i < arraySize; i++)
	{
		puzzleState.tilePositions[i] = slidePuzzelArray[i];
	}

	// Create the string representation of the tile state
	puzzleState.positions = createStringRepresentation(puzzleState.tilePositions);

	// assign the depthcout of zero to the initial state
	puzzleState.depth = depthCount;
	
	// put the starting node in the map
	visitedMap.insert(pair<string, int>(puzzleState.positions, mapCount));
	mapCount++;

	// Add the first state to the queue
	nodeQueue.push(puzzleState);

	// while loop to check and see if the puzzel was solved
	while (!solved)
	{
		// make sure the queue is not empty
		if (nodeQueue.empty())
		{
			// Inform the user that the goal state could not be found
			system("CLS");
			cout << "There are no more moves to make, the puzzle was unsolvable from the starting position." << endl;
			cout << nodesExpanded << " nodes were expanded in this search.\n" << endl;
			cout << "Enter 1 to Search the same puzzle again." << endl;
			cout << "Enter 2 to Return to the Main Menu" << endl;
			viewResults = checkInput(5);
			system("CLS");
			visitedMap.clear();

			if (viewResults == 1)
			{
				chooseSearch(slidePuzzelArray);
			}
			return;
		}

		// Evaluate the first node to see if it is in the solved state.
		if (nodeQueue.front().positions == "123456780")
		{
			solved = true;

			// Inform the user that the search is complete and display the results
			system("CLS");
			cout << "Search Complete." << endl;
			printPuzzelBoard(nodeQueue.front().tilePositions);
			cout << "This goal state was found at depth level " << nodeQueue.front().depth << "." << endl;
			cout << nodesExpanded << " nodes were expanded in this search.\n" << endl;

			// Allow the user to see the steps taken to get to the result
			cout << "Enter 1 to see the steps to solve the puzzle" << endl;
			cout << "Enter 2 to Search the same puzzle again." << endl;
			cout << "Enter 3 to Return to the Main Menu" << endl;
			viewResults = checkInput(4);
			cin.ignore(10000, '\n');
			system("CLS");
			visitedMap.clear();

			if (viewResults == 1)
			{
				printResults(nodeQueue.front(), slidePuzzelArray);
			}
			else if (viewResults == 2)
			{
				chooseSearch(slidePuzzelArray);
			}

			return;
		}

		// Copy the front of the queue to an array
		for (int i = 0; i < arraySize; i++)
		{
			currentFrontArray[i] = nodeQueue.front().tilePositions[i];
		}


		// Else statement to move on from the current node if it is not the solved puzzel
		if (solved != true)
		{
			// Generate Moves depending on position of the 0
			generateMoves(currentFrontArray, moves);

			// increment nodes expandes since children were just created.
			nodesExpanded++;

			// for loop to perform each move
			for (int i = 0; i < moves.size(); i++)
			{
				//initialize tempArray
				initializePuzzelBoard(tempArray);

				// make the move and store it in the new node
				makeMove(tempArray, moves[i], currentFrontArray);

				// generate the string for the new node
				moveState = createStringRepresentation(tempArray);

				// Save the front of the queue's depth
				depthCount = nodeQueue.front().depth;

				// Add the new node to the queue only if it's not already there
				if (stateVisited(moveState) == false)
				{
					// write the move into the map
					visitedMap.insert(pair<string, int>(moveState, mapCount));

					// Increment mapCount
					mapCount++;

					// Create new node
					slidePuzzelNode generatedState;

					// Set the depth count of the new node
					generatedState.depth = depthCount + 1;

					// Save the string representation of the state
					generatedState.positions = moveState;

					// Add the tempArray to the new node
					for (int i = 0; i < arraySize; i++)
					{
						generatedState.tilePositions[i] = tempArray[i];
					}

					// Add the current node's path to new node
					for (int i = 0; i < nodeQueue.front().moveOrder.size(); i++)
					{
						generatedState.moveOrder.push_back(nodeQueue.front().moveOrder[i]);
					}

					// Add the move performed into movePath of the node
					generatedState.moveOrder.push_back(moves[i]);

					// push the new state to the queue
					nodeQueue.push(generatedState);
				}
			}

			// clear the moves
			moves.clear();

			// pop the stack
			nodeQueue.pop();
		}
	}
	return;
}

// Function to perform a depthFirst Search
void depthFirstSearch(int slidePuzzleArray[arraySize])
{
	// integer to hold if the user wants to search again
	int searchAgain = 0;

	// integer to hold the number of nodes expanded
	int nodesExpanded = 0;

	// create the stack
	stack<slidePuzzelNode> nodeStack;

	// Vector to be sued to store the moves to be done
	vector<int> moves;

	// integer to hold the depth 
	int depthCount = 0;

	// boolean to hold if the puzzle has been solved
	bool solved = false;

	// array to hold an array
	int tempArray[arraySize] = { 0 };

	// string to hold the string representation of a state
	string moveState;

	// int to hold an integer to accompiany a string in the map
	int mapCount = 1;

	// array to hold the goal state
	int goalArray[arraySize] = { 1,2,3,4,5,6,7,8,0 };

	// arra to hold the tilePositions from the top of the stack
	int currentTopArray[arraySize] = { 0 };

	// Create the first state
	slidePuzzelNode puzzleState;

	// set the tile positions array
	for (int i = 0; i < arraySize; i++)
	{
		puzzleState.tilePositions[i] = slidePuzzleArray[i];
	}

	// create the string representation to be compared
	puzzleState.positions = createStringRepresentation(puzzleState.tilePositions);
	
	// Set the dpeth of the initial node
	puzzleState.depth = depthCount;

	// put the starting state in the map
	visitedMap.insert(pair<string, int>(puzzleState.positions, mapCount));

	// Increment the map counter
	mapCount++;

	// Add the initial state to the stack
	nodeStack.push(puzzleState);

	//while loop to create new states until there are no more viable ones or the goal is found
	while (!solved)
	{
		// Make sure the stack is not empty
		if (nodeStack.empty())
		{
			system("CLS");
			cout << "There are no more moves to make, the puzzle was unsolvable from the starting position." << endl;
			cout << nodesExpanded << " nodes were expanded in this search.\n" << endl;
			cout << "Enter 1 to Search the same puzzle again." << endl;
			cout << "Enter 2 to return to the main menu." << endl;
			searchAgain = checkInput(5);
			system("CLS");
			// clear the map
			visitedMap.clear();

			if (searchAgain == 1)
			{
				chooseSearch(slidePuzzleArray);
			}
			

			return;
		}

		// Evaluate the top of the stack and see if it is the goal state
		if (nodeStack.top().positions == "123456780")
		{
			solved = true;

			// Print something
			system("CLS");
			cout << "Search Complete." << endl;
			printPuzzelBoard(nodeStack.top().tilePositions);
			cout << "The goal state was found at depth level " << nodeStack.top().depth << "." << endl;
			cout << nodesExpanded << " nodes were expanded in this search.\n" << endl;
			cout << "Enter 1 to Search the same puzzle again." << endl;
			cout << "Enter 2 to return to the main menu." << endl;
			searchAgain = checkInput(5);
			system("CLS");
			// clear the map
			visitedMap.clear();

			if (searchAgain == 1)
			{
				chooseSearch(slidePuzzleArray);
			}

			return;
		}

		// copy the top of the stack to an array
		for (int i = 0; i < arraySize; i++)
		{
			currentTopArray[i] = nodeStack.top().tilePositions[i];
		}

		// copy the depth count of top of the stack
		depthCount = nodeStack.top().depth;

		// Pop the stack
		nodeStack.pop();

		// create the moves that are possible from this state
		if (solved != true)
		{
			// generate themoves depending on the position of the 0 in the initial state
			generateMoves(currentTopArray, moves);

			// increment nodes expanded since children were just created
			nodesExpanded++;

			// for loop to make each move and put it on the stack if a new state is generated
			for (int i = 0; i < moves.size(); i++)
			{
				// initialize the temp array
				initializePuzzelBoard(tempArray);

				// Make the move and store it in a new node
				makeMove(tempArray, moves[i], currentTopArray);

				// Generate the string for the new node
				moveState = createStringRepresentation(tempArray);

				// Add the new node to the stack only if it's not already there
				if (stateVisited(moveState) == false)
				{
					// Write the new state to the map
					visitedMap.insert(pair<string, int>(moveState, mapCount));

					// incremnet mapCount
					mapCount++;

					// create a new node
					slidePuzzelNode generatedState;

					// set the depthCOunt of the new node
					generatedState.depth = depthCount + 1;

					// set the state string
					generatedState.positions = moveState;

					// Add the tempAray to the new node
					for (int i = 0; i < arraySize; i++)
					{
						generatedState.tilePositions[i] = tempArray[i];
					}

					// Add the new state to the stack
					nodeStack.push(generatedState);
				}
			}
			// clear the moves
			moves.clear();
		}
	}
	return;
}

// Function to perform a search based on total misplaced tiles
void missingTilesSearch(int slidePuzzelArray[arraySize])
{
	// Create Queue
	priority_queue<slidePuzzelNode> nodeQueue;

	// Integer to hold how many nodes were expanded
	int nodesExpanded = 0;

	// Vector to be used to store the moves to be done
	vector<int> moves;

	// temporary vector to hold onto moves while generating children
	vector<int> currentStateMoves;

	// integer to hold the top of the queue's heuristic info
	int currentHeurisitcInfo = 0;

	// Count to hold the depth of the node
	int depthCount = 0;

	// Integer to hold user input to view results
	int viewResults = 0;

	// boolean to hold if the puzzle was solved
	bool solved = false;

	// array to hold a temporary state
	int tempArray[arraySize] = { 0 };

	// string to hold the string representaiton of a generated state
	string moveState;

	// integer to accompany the strings in the map
	int mapCount = 1;

	// array to hold the the goal state
	int goalArray[arraySize] = { 1, 2, 3, 4, 5, 6, 7, 8, 0 };

	// array to hold onto the tileArray in the front of the queue
	int currentFrontArray[arraySize];

	// Create the first state.
	slidePuzzelNode puzzleState;

	// Copy the initial state into a working state
	// copy the tile positions
	for (int i = 0; i < arraySize; i++)
	{
		puzzleState.tilePositions[i] = slidePuzzelArray[i];
	}

	// Create the string representation of the tile state
	puzzleState.positions = createStringRepresentation(puzzleState.tilePositions);

	// assign the depthcout of zero to the initial state
	puzzleState.depth = depthCount;

	// Calculate the number of misplaced tiles for the state
	puzzleState.heurisiticInfo = calculateMisplacedTiles(puzzleState.tilePositions);

	// put the starting node in the map
	visitedMap.insert(pair<string, int>(puzzleState.positions, mapCount));
	mapCount++;

	// Add the first state to the queue
	nodeQueue.push(puzzleState);

	// while loop to check and see if the puzzel was solved
	while (!solved)
	{
		// make sure the queue is not empty
		if (nodeQueue.empty())
		{
			// Inform the user that the goal state could not be found
			system("CLS");
			cout << "There are no more moves to make, the puzzle was unsolvable from the starting position." << endl;
			cout << nodesExpanded << " nodes were expanded in this search\n" << endl;
			cout << "Enter 1 to Search the same puzzle again." << endl;
			cout << "Enter 2 to Return to the Main Menu" << endl;
			viewResults = checkInput(5);
			system("CLS");
			visitedMap.clear();

			if (viewResults == 1)
			{
				chooseSearch(slidePuzzelArray);
			}
			return;
		}

		// Evaluate the first node to see if it is in the solved state.
		if (nodeQueue.top().positions == "123456780")
		{
			solved = true;

			int printBoard[arraySize];
			for (int i = 0; i < arraySize; i++)
			{
				printBoard[i] = nodeQueue.top().tilePositions[i];
			}

			// Inform the user that the search is complete and display the results
			system("CLS");
			cout << "Search Complete." << endl;
			printPuzzelBoard(printBoard);
			cout << "This goal state was found at depth level " << nodeQueue.top().depth << "." << endl;
			cout << nodesExpanded << " nodes were expanded in this search." << endl;

			// Allow the user to see the steps taken to get to the result
			cout << "\nEnter 1 to see the steps to solve the puzzle" << endl;
			cout << "Enter 2 to Search the same puzzle again." << endl;
			cout << "Enter 3 to Return to the Main Menu" << endl;
			viewResults = checkInput(4);
			cin.ignore(10000, '\n');
			system("CLS");
			visitedMap.clear();

			if (viewResults == 1)
			{
				printResults(nodeQueue.top(), slidePuzzelArray);
			}
			else if (viewResults == 2)
			{
				chooseSearch(slidePuzzelArray);
			}

			return;
		}

		// Copy the front of the queue to an array
		for (int i = 0; i < arraySize; i++)
		{
			currentFrontArray[i] = nodeQueue.top().tilePositions[i];
		}
		
		// clear the moves from the last node
		currentStateMoves.clear();
		
		if (nodeQueue.top().moveOrder.empty() == false)
		{
			// Hold on to the current state's moves
			for (int i = 0; i < nodeQueue.top().moveOrder.size(); i++)
			{
				currentStateMoves.push_back(nodeQueue.top().moveOrder[i]);
			}
		}
		
		// Save the front of the queue's depth
		depthCount = nodeQueue.top().depth;

		// save the front of the queue's heuristic info
		currentHeurisitcInfo = nodeQueue.top().heurisiticInfo;
		
		// pop the stack
		nodeQueue.pop();

		// Else statement to move on from the current node if it is not the solved puzzel
		if (solved != true)
		{
			// Generate Moves depending on position of the 0
			generateMoves(currentFrontArray, moves);
			
			// Increment nodes expanded (just created children
			nodesExpanded++;

			// for loop to perform each move
			for (int i = 0; i < moves.size(); i++)
			{
				//initialize tempArray
				initializePuzzelBoard(tempArray);

				// make the move and store it in the new node
				makeMove(tempArray, moves[i], currentFrontArray);

				// generate the string for the new node
				moveState = createStringRepresentation(tempArray);

				// Add the new node to the queue only if it's not already there
				if (stateVisited(moveState) == false)
				{
					// write the move into the map
					visitedMap.insert(pair<string, int>(moveState, mapCount));

					// Increment mapCount
					mapCount++;

					// Create new node
					slidePuzzelNode generatedState;

					// Set the depth count of the new node
					generatedState.depth = depthCount + 1;

					// Save the string representation of the state
					generatedState.positions = moveState;

					// Add the tempArray to the new node
					for (int i = 0; i < arraySize; i++)
					{
						generatedState.tilePositions[i] = tempArray[i];
					}

					// calculate the number of misplaced tiles for the generated state
					generatedState.heurisiticInfo = calculateMisplacedTiles(generatedState.tilePositions) + currentHeurisitcInfo;
					
					// only add the current moves if we are passed the initial state
					if (nodesExpanded > 0)
					{
						// Add the current node's path to new node
						for (int i = 0; i < currentStateMoves.size(); i++)
						{
							generatedState.moveOrder.push_back(currentStateMoves[i]);
						}

						// Add the move performed into movePath of the node
						generatedState.moveOrder.push_back(moves[i]);
					}
						
					// push the new state to the queue
					nodeQueue.push(generatedState);
				}
			}

			// clear the moves
			moves.clear();
		}
	}
	return;
}

// Function to perform a search based on the Manhattan Distance
void manhattanDistanceSearch(int slidePuzzelArray[arraySize])
{
	// Create Queue
	priority_queue<slidePuzzelNode> nodeQueue;

	// Integer to hold how many nodes were expanded
	int nodesExpanded = 0;

	// Vector to be used to store the moves to be done
	vector<int> moves;

	// temporary vector to hold onto moves while generating children
	vector<int> currentStateMoves;

	// Count to hold the depth of the node
	int depthCount = 0;

	// integer to hold the top of the queue's heuristic info
	int currentHeuristicInfo = 0;

	// Integer to hold user input to view results
	int viewResults = 0;

	// boolean to hold if the puzzle was solved
	bool solved = false;

	// array to hold a temporary state
	int tempArray[arraySize] = { 0 };

	// string to hold the string representaiton of a generated state
	string moveState;

	// integer to accompany the strings in the map
	int mapCount = 1;

	// array to hold the the goal state
	int goalArray[arraySize] = { 1, 2, 3, 4, 5, 6, 7, 8, 0 };

	// array to hold onto the tileArray in the front of the queue
	int currentFrontArray[arraySize];

	// Create the first state.
	slidePuzzelNode puzzleState;

	// Copy the initial state into a working state
	// copy the tile positions
	for (int i = 0; i < arraySize; i++)
	{
		puzzleState.tilePositions[i] = slidePuzzelArray[i];
	}

	// Create the string representation of the tile state
	puzzleState.positions = createStringRepresentation(puzzleState.tilePositions);

	// assign the depthcout of zero to the initial state
	puzzleState.depth = depthCount;

	// Calculate the number of misplaced tiles for the state
	puzzleState.heurisiticInfo = calculateManhattanDistance(puzzleState.tilePositions);

	// put the starting node in the map
	visitedMap.insert(pair<string, int>(puzzleState.positions, mapCount));
	mapCount++;

	// Add the first state to the queue
	nodeQueue.push(puzzleState);

	// while loop to check and see if the puzzel was solved
	while (!solved)
	{
		// make sure the queue is not empty
		if (nodeQueue.empty())
		{
			// Inform the user that the goal state could not be found
			system("CLS");
			cout << "There are no more moves to make, the puzzle was unsolvable from the starting position." << endl;
			cout << nodesExpanded << " nodes were expanded in this search.\n" << endl;
			cout << "Enter 1 to Search the same puzzle again." << endl;
			cout << "Enter 2 to Return to the Main Menu" << endl;
			viewResults = checkInput(5);
			system("CLS");
			visitedMap.clear();

			if (viewResults == 1)
			{
				chooseSearch(slidePuzzelArray);
			}
			return;
		}

		// Evaluate the first node to see if it is in the solved state.
		if (nodeQueue.top().positions == "123456780")
		{
			solved = true;

			int printBoard[arraySize];
			for (int i = 0; i < arraySize; i++)
			{
				printBoard[i] = nodeQueue.top().tilePositions[i];
			}

			// Inform the user that the search is complete and display the results
			system("CLS");
			cout << "Search Complete." << endl;
			printPuzzelBoard(printBoard);
			cout << "This goal state was found at depth level " << nodeQueue.top().depth << "." << endl;
			cout << nodesExpanded << " nodes were expanded in this search.\n" << endl;

			// Allow the user to see the steps taken to get to the result
			cout << "\nEnter 1 to see the steps to solve the puzzle" << endl;
			cout << "Enter 2 to Search the same puzzle again." << endl;
			cout << "Enter 3 to Return to the Main Menu" << endl;
			viewResults = checkInput(4);
			cin.ignore(10000, '\n');
			system("CLS");
			visitedMap.clear();

			if (viewResults == 1)
			{
				printResults(nodeQueue.top(), slidePuzzelArray);
			}
			else if (viewResults == 2)
			{
				chooseSearch(slidePuzzelArray);
			}

			return;
		}

		// Copy the front of the queue to an array
		for (int i = 0; i < arraySize; i++)
		{
			currentFrontArray[i] = nodeQueue.top().tilePositions[i];
		}

		// clear the moves from the last node
		currentStateMoves.clear();

		if (nodeQueue.top().moveOrder.empty() != true)
		{
			// Hold on to the current state's moves
			for (int i = 0; i < nodeQueue.top().moveOrder.size(); i++)
			{
				currentStateMoves.push_back(nodeQueue.top().moveOrder[i]);
			}
		}

		// Save the front of the queue's depth
		depthCount = nodeQueue.top().depth;

		// Save the front of the queue's heurisitc data
		currentHeuristicInfo = nodeQueue.top().heurisiticInfo;

		// pop the stack
		nodeQueue.pop();

		// Else statement to move on from the current node if it is not the solved puzzel
		if (solved != true)
		{
			// Generate Moves depending on position of the 0
			generateMoves(currentFrontArray, moves);

			// Increment nodes expanded (just created children
			nodesExpanded++;

			// for loop to perform each move
			for (int i = 0; i < moves.size(); i++)
			{
				//initialize tempArray
				initializePuzzelBoard(tempArray);

				// make the move and store it in the new node
				makeMove(tempArray, moves[i], currentFrontArray);

				// generate the string for the new node
				moveState = createStringRepresentation(tempArray);

				// Add the new node to the queue only if it's not already there
				if (stateVisited(moveState) == false)
				{
					// write the move into the map
					visitedMap.insert(pair<string, int>(moveState, mapCount));

					// Increment mapCount
					mapCount++;

					// Create new node
					slidePuzzelNode generatedState;

					// Set the depth count of the new node
					generatedState.depth = depthCount + 1;

					// Save the string representation of the state
					generatedState.positions = moveState;

					// Add the tempArray to the new node
					for (int i = 0; i < arraySize; i++)
					{
						generatedState.tilePositions[i] = tempArray[i];
					}

					// calculate the number of misplaced tiles for the generated state
					generatedState.heurisiticInfo = calculateManhattanDistance(generatedState.tilePositions) + currentHeuristicInfo;
			
					// Add the current node's path to new node
					for (int i = 0; i < currentStateMoves.size(); i++)
					{
						generatedState.moveOrder.push_back(currentStateMoves[i]);
					}

					// Add the move performed into movePath of the node
					generatedState.moveOrder.push_back(moves[i]);

					// push the new state to the queue
					nodeQueue.push(generatedState);
				}
			}

			// clear the moves
			moves.clear();
		}
	}
	return;
}



//*************** FUNCTIONS TO CREATE PUZZLE BOARDS ********************

// Function to create a random puzzel
void createRandomPuzzel(int slidePuzzelArray[arraySize])
{
	// random number to be given to the array
	int randomTilePosition;
	int randomPuzzel[arraySize];
	bool filled[9] = { false };

	// for loop to cylce through the puzzel board
	for (int i = 0; i < 9; i++)
	{
		// generate a random tile position
		randomTilePosition = rand() % 9;

		// If that position already has a tile
		while (filled[randomTilePosition] == true)
		{
			// generate a new position until a non filled one is found
			randomTilePosition = rand() % 9;
		}

		// Set the tile  number to the randomPosition
		randomPuzzel[randomTilePosition] = i;

		// set the filled array to true in the randomly generate position
		filled[randomTilePosition] = true;
	}

	for (int i = 0; i < arraySize; i++)
	{
		slidePuzzelArray[i] = randomPuzzel[i];
	}

	return;
}

// Function to create a user made puzzel
void createUserPuzzel(int sliderPuzzelArray[arraySize])
{
	// Declare arrays
	bool filledArray[arraySize];
	int userInput[arraySize];
	int positionsArray[arraySize];
	int emptyTilePosition;
	int tileNumber;
	int column = 0;

	// initialize arrays
	for (int i = 0; i < arraySize; i++)
	{
		filledArray[i] = false;
		userInput[i] = -1;
		positionsArray[i] = i;
	}

	// CLear the console
	system("CLS");

	// Display the current board for the user
	for (int i = 0; i < arraySize; i++)
	{
		// Display the current board
		for (int j = 0; j < arraySize; j++)
		{
			if (userInput[j] > -1)
			{
				cout << userInput[j] << "\t";
				column++;
			}

			else if (userInput[j - 1] > -1 || j == 0)
			{
				cout << 'X' << "\t";
				column++;
			}

			else
			{
				cout << 'E' << "\t";
				column++;
			}

			if (column > 2)
			{
				cout << "\n";
				column = 0;
			}

		}

		if (filledArray[i] == false)
		{
			// Inform the user of what the outputs mean and what they are supposed to do
			cout << "In the Puzzel, E's currently do not have a numbered tile assigned to them, and the X is the current entry point." << endl;
			cout << "Please enter a number between 0 and 8 that you wish to appear at the X point." << endl;
			cout << "Note that 0 is the empty tile in the puzzel" << endl;
			tileNumber = createTile(userInput);
			cin.ignore(10000, '\n');

			// Check to make sure that the tileNumber is not already in the array
			if (tileInPuzzel(userInput, tileNumber) == false)
			{
				// Assign the tileNumber entered to the userInput array
				userInput[i] = tileNumber;
				filledArray[i] = true;
			}

			// clear the console
			system("CLS");
		}

		// Assing the user input to the main puzzel array
		for (int i = 0; i < arraySize; i++)
		{
			sliderPuzzelArray[i] = userInput[i];
		}
	}

	return;
}

// Function to check and see if a tile is in the puzzel
bool tileInPuzzel(int sliderPuzzelArray[arraySize], int checkedTile)
{
	// variable to return if the tile is in the puzzel
	bool tilePresent = false;

	// for loop to check each index of the array and see if the tile is in there.
	for (int i = 0; i < arraySize; i++)
	{
		// Return true if the checkedTile is in the sliderPuzzel
		if (sliderPuzzelArray[i] == checkedTile)
		{
			return true;
		}
	}

	return false;
}

// Function to make sure the user only enter a number into the tile once
int createTile(int userInput[arraySize])
{
	int n;
	bool alreadyEntered = false;

	while (true)
	{
		cin >> n;

		// make sure that the value entered is not already in the puzzle4
		for (int i = 0; i < arraySize; i++)
		{
			if (userInput[i] == n)
			{
				alreadyEntered = true;
			}
		}

		// break from the loop if the value entered is in range and not already in the puzzle
		if ((cin) && (n >= 0) && (n <= 8) && alreadyEntered == false)
		{
			break;
		}

		cin.clear();
		cin.ignore(1000, '\n');
		cout << "Invalid Entry: Pleaes enter a value between 0 and 8 that does not already occur in the board." << endl;
		alreadyEntered = false;

	}

	return n;
}



//******************* FUNCTIONS USED IN SEARCHES ***********************

// Function to choose the type of search the user wishes to perform on the board
void chooseSearch(int sliderPuzzelArray[arraySize])
{
	// Variables
	int searchChoice = 0;

	// Clear the console
	system("CLS");

	// Show the user their board
	printPuzzelBoard(sliderPuzzelArray);

	cout << "\nThis is the current puzzel to be solved.  What type of search would you like to perform?" << endl;
	cout << "1.\tBreadth First Search" << endl;
	cout << "2.\tDepth First Search" << endl;
	cout << "3.\tSearch Based on Total of Misplaced Tiles" << endl;
	cout << "4.\tSearch Based on the Manhattan Distance" << endl;
	searchChoice = checkInput(3);
	cin.ignore(10000, '\n');

	// Switch to call the currect search funtion
	switch (searchChoice)
	{
	case 1:
		system("CLS");
		cout << "SEARCHING..." << endl;
		breadthFirstSearch(sliderPuzzelArray);
		break;

	case 2:
		system("CLS");
		cout << "SEARCHING..." << endl;
		depthFirstSearch(sliderPuzzelArray);
		break;
	case 3:
		system("CLS");
		cout << "SEARCHING..." << endl;
		missingTilesSearch(sliderPuzzelArray);
		break;
	case 4:
		system("CLS");
		cout << "SEARCHING..." << endl;
		manhattanDistanceSearch(sliderPuzzelArray);
		break;
	}

	return;
}

// Function to reinitialize the puzzel for another run
void initializePuzzelBoard(int sliderPuzzelArray[arraySize])
{
	// For loop to cycle through the array and sign each index to zero
	for (int i = 0; i < arraySize; i++)
	{
		sliderPuzzelArray[i] = 0;
	}
	return;
}

// Function to generate moves depending on where the 0 is.
void generateMoves(int currentState[arraySize], vector<int> &moves)
{
	// varible to hold the postion of the empty tile
	int zeroPosition = -1;

	// for loop to find the zero postion
	for (int i = 0; i < arraySize; i++)
	{
		if (currentState[i] == 0)
		{
			zeroPosition = i;
		}
	}

	/*
	Generate the moves 
	Numbers being inserted into the vectors respond to moves:
	1 - Swap 0 and 1 Positions
	2 - Swap 0 and 3 Positions
	3 - Swap 1 and 2 Positions
	4 - Swap 1 and 4 Positions
	5 - Swap 2 and 5 Positions
	6 - Swap 3 and 4 Positions
	7 - Swap 3 and 6 Positions
	8 - Swap 4 and 5 Positions
	9 - Swap 4 and 7 Positions
	10 - Swap 5 and 8 Positions
	11 - Swap 6 and 7 Positions
	12 - Swap 7 and 8 Positions

	Positions:
	0		1		2
	3		4		5
	6		7		8

	*/
	switch (zeroPosition)
	{
	case 0:
		moves.push_back(1);		// 0 -> 1
		moves.push_back(2);		// 0 -> 3
		break;

	case 1:
		moves.push_back(3);		// 1 -> 2
		moves.push_back(4);		// 1 -> 4
		moves.push_back(1);		// 0 -> 1
		break;

	case 2:
		moves.push_back(5);		// 2 -> 5
		moves.push_back(3);		// 1 -> 2
		break;

	case 3:
		moves.push_back(2);		// 0 -> 3
		moves.push_back(6);		// 3 -> 4
		moves.push_back(7);		// 3 -> 6
		break;

	case 4:
		moves.push_back(4);		// 1 -> 4
		moves.push_back(8);		// 4 -> 5
		moves.push_back(9);		// 4 -> 7
		moves.push_back(6);		// 3 -> 4
		break;

	case 5:
		moves.push_back(5);		// 2 -> 5
		moves.push_back(10);	// 5 -> 8
		moves.push_back(8);		// 4 -> 5
		break;

	case 6:
		moves.push_back(7);		// 3 -> 6
		moves.push_back(11);	// 6 -> 7
		break;

	case 7:
		moves.push_back(9);		// 4 -> 7
		moves.push_back(12);	// 7 -> 8
		moves.push_back(11);	// 6 -> 7
		break;

	case 8:
		moves.push_back(10);	// 5 -> 8
		moves.push_back(12);	// 7 -> 8
		break;
	}

	return;
}

// Function to make make moves on the puzzel board
void makeMove(int nextState[arraySize], int moveNumber, int currentState[arraySize])
{
	// Variable to hold onto the current value
	int temp = 0;

	// copy the current state into the next state
	for (int i = 0; i < arraySize; i++)
	{
		nextState[i] = currentState[i];
	}

	// Make the move on the nextState array
	switch (moveNumber)
	{
	case 1:			//  0 -> 1
		temp = nextState[0];
		nextState[0] = nextState[1];
		nextState[1] = temp;
		break;

	case 2:			// 0 -> 3
		temp = nextState[0];
		nextState[0] = nextState[3];
		nextState[3] = temp;
		break;

	case 3:			// 1 -> 2
		temp = nextState[1];
		nextState[1] = nextState[2];
		nextState[2] = temp;
		break;

	case 4:			// 1 -> 4
		temp = nextState[1];
		nextState[1] = nextState[4];
		nextState[4] = temp;
		break;

	case 5:			// 2 -> 5
		temp = nextState[2];
		nextState[2] = nextState[5];
		nextState[5] = temp;
		break;

	case 6:			// 3 -> 4
		temp = nextState[3];
		nextState[3] = nextState[4];
		nextState[4] = temp;
		break;

	case 7:			// 3 -> 6
		temp = nextState[3];
		nextState[3] = nextState[6];
		nextState[6] = temp;
		break;

	case 8:			// 4 -> 5
		temp = nextState[4];
		nextState[4] = nextState[5];
		nextState[5] = temp;
		break;

	case 9:			// 4 -> 7
		temp = nextState[4];
		nextState[4] = nextState[7];
		nextState[7] = temp;
		break;

	case 10:		// 5 -> 8
		temp = nextState[5];
		nextState[5] = nextState[8];
		nextState[8] = temp;
		break;

	case 11:		// 6 -> 7
		temp = nextState[6];
		nextState[6] = nextState[7];
		nextState[7] = temp;
		break;

	case 12:		// 7 -> 8
		temp = nextState[7];
		nextState[7] = nextState[8];
		nextState[8] = temp;
		break;
	}

	return;
}

// function to create a string out of a puzzel board
string createStringRepresentation(int currentState[arraySize])
{
	// string variable to hold the completed string
	string puzzelArrayString;

	for (int i = 0; i < arraySize; i++)
	{
		puzzelArrayString += to_string(currentState[i]);

	}

	return puzzelArrayString;

}

// Function to see if the current state was already visited
bool stateVisited(string currentPosition)
{
	map<string, int>::iterator it;

	it = visitedMap.find(currentPosition);

	if (it == visitedMap.end())
	{
		return false;
	}

	return true;
}

// Function to print the puzzel board
void printPuzzelBoard(int sliderPuzzelArray[arraySize])
{
	// column counter
	int column = 0;

	// print the board
	for (int i = 0; i < arraySize; i++)
	{
		cout << sliderPuzzelArray[i] << "\t";
		column++;

		if (column > 2)
		{
			cout << "\n";
			column = 0;
		}
	}
}

// Function to print the results of the search
void printResults(slidePuzzelNode endingNode, int startingBoard[arraySize])
{
	// integer to hold the user choice
	int samePuzzle = 0;

	// Variable to hold onto the current value
	int temp = 0;

	// Create a node to work with
	slidePuzzelNode workingNode;

	for (int i = 0; i < arraySize; i++)
	{
		workingNode.tilePositions[i] = startingBoard[i];
	}

	system("CLS");

	// print the starting point.
	printPuzzelBoard(workingNode.tilePositions);

	cout << "This is the starting point." << endl;
	cout << "Press Enter to make the first move" << endl;
	cin.get();
	system("CLS");


	// for loop to make the moves through the puzzel
	for (int i = 0; i < endingNode.moveOrder.size(); i++)
	{

		// Make the move on the nextState array
		switch (endingNode.moveOrder[i])
		{
		case 1:			//  0 -> 1
			temp = workingNode.tilePositions[0];
			workingNode.tilePositions[0] = workingNode.tilePositions[1];
			workingNode.tilePositions[1] = temp;

			// Print the new board after the move
			printPuzzelBoard(workingNode.tilePositions);
			cout << "Puzzel Board after Move " << i + 1 << ": Swap Position 0 and Position 1" << endl;

			if (i != endingNode.moveOrder.size() - 1)
			{
				cout << "Press Enter to perform the next move." << endl;
				cin.get();
				system("CLS");
			}

			else
			{
				cout << "This is the final move.\nPress Enter to continue.\n" << endl;
				cout << "Enter 1 to Search the same puzzle again." << endl;
				cout << "Enter 2 to Return to the main menu." << endl;
				samePuzzle = checkInput(5);

				if (samePuzzle == 1)
				{
					chooseSearch(startingBoard);
				}
				system("CLS");
			}

			break;

		case 2:			// 0 -> 3
			temp = workingNode.tilePositions[0];
			workingNode.tilePositions[0] = workingNode.tilePositions[3];
			workingNode.tilePositions[3] = temp;

			// Print the new board after the move
			printPuzzelBoard(workingNode.tilePositions);
			cout << "Puzzel Board after Move " << i + 1 << ": Swap Position 0 and Position 3" << endl;

			if (i != endingNode.moveOrder.size() - 1)
			{
				cout << "Press Enter to perform the next move." << endl;
				cin.get();
				system("CLS");
			}

			else
			{
				cout << "This is the final move.\nPress Enter to continue.\n" << endl;
				cout << "Enter 1 to Search the same puzzle again." << endl;
				cout << "Enter 2 to Return to the main menu." << endl;
				samePuzzle = checkInput(5);

				if (samePuzzle == 1)
				{
					chooseSearch(startingBoard);
				}
				system("CLS");
			}

			break;

		case 3:			// 1 -> 2
			temp = workingNode.tilePositions[1];
			workingNode.tilePositions[1] = workingNode.tilePositions[2];
			workingNode.tilePositions[2] = temp;

			// Print the new board after the move
			printPuzzelBoard(workingNode.tilePositions);
			cout << "Puzzel Board after Move " << i + 1 << ": Swap Position 1 and Position 2" << endl;

			if (i != endingNode.moveOrder.size() - 1)
			{
				cout << "Press Enter to perform the next move." << endl;
				cin.get();
				system("CLS");
			}

			else
			{
				cout << "This is the final move.\nPress Enter to continue.\n" << endl;
				cout << "Enter 1 to Search the same puzzle again." << endl;
				cout << "Enter 2 to Return to the main menu." << endl;
				samePuzzle = checkInput(5);

				if (samePuzzle == 1)
				{
					chooseSearch(startingBoard);
				}
				system("CLS");
			}


			break;

		case 4:			// 1 -> 4
			temp = workingNode.tilePositions[1];
			workingNode.tilePositions[1] = workingNode.tilePositions[4];
			workingNode.tilePositions[4] = temp;

			// Print the new board after the move
			printPuzzelBoard(workingNode.tilePositions);
			cout << "Puzzel Board after Move " << i + 1 << ": Swap Position 1 and Position 4" << endl;

			if (i != endingNode.moveOrder.size() - 1)
			{
				cout << "Press Enter to perform the next move." << endl;
				cin.get();
				system("CLS");
			}

			else
			{
				cout << "This is the final move.\nPress Enter to continue.\n" << endl;
				cout << "Enter 1 to Search the same puzzle again." << endl;
				cout << "Enter 2 to Return to the main menu." << endl;
				samePuzzle = checkInput(5);

				if (samePuzzle == 1)
				{
					chooseSearch(startingBoard);
				}
				system("CLS");
			}

			break;

		case 5:			// 2 -> 5
			temp = workingNode.tilePositions[2];
			workingNode.tilePositions[2] = workingNode.tilePositions[5];
			workingNode.tilePositions[5] = temp;

			// Print the new board after the move
			printPuzzelBoard(workingNode.tilePositions);
			cout << "Puzzel Board after Move " << i + 1 << ": Swap Position 2 and Position 5" << endl;

			if (i != endingNode.moveOrder.size() - 1)
			{
				cout << "Press Enter to perform the next move." << endl;
				cin.get();
				system("CLS");
			}

			else
			{
				cout << "This is the final move.\nPress Enter to continue.\n" << endl;
				cout << "Enter 1 to Search the same puzzle again." << endl;
				cout << "Enter 2 to Return to the main menu." << endl;
				samePuzzle = checkInput(5);

				if (samePuzzle == 1)
				{
					chooseSearch(startingBoard);
				}
				system("CLS");
			}

			break;

		case 6:			// 3 -> 4
			temp = workingNode.tilePositions[3];
			workingNode.tilePositions[3] = workingNode.tilePositions[4];
			workingNode.tilePositions[4] = temp;

			// Print the new board after the move
			printPuzzelBoard(workingNode.tilePositions);
			cout << "Puzzel Board after Move " << i + 1 << ": Swap Position 3 and Position 4" << endl;

			if (i != endingNode.moveOrder.size() - 1)
			{
				cout << "Press Enter to perform the next move." << endl;
				cin.get();
				system("CLS");
			}

			else
			{
				cout << "This is the final move.\nPress Enter to continue.\n" << endl;
				cout << "Enter 1 to Search the same puzzle again." << endl;
				cout << "Enter 2 to Return to the main menu." << endl;
				samePuzzle = checkInput(5);

				if (samePuzzle == 1)
				{
					chooseSearch(startingBoard);
				}
				system("CLS");
			}

			break;

		case 7:			// 3 -> 6
			temp = workingNode.tilePositions[3];
			workingNode.tilePositions[3] = workingNode.tilePositions[6];
			workingNode.tilePositions[6] = temp;

			// Print the new board after the move
			printPuzzelBoard(workingNode.tilePositions);
			cout << "Puzzel Board after Move " << i + 1 << ": Swap Position 3 and Position 6" << endl;

			if (i != endingNode.moveOrder.size() - 1)
			{
				cout << "Press Enter to perform the next move." << endl;
				cin.get();
				system("CLS");
			}

			else
			{
				cout << "This is the final move.\nPress Enter to continue.\n" << endl;
				cout << "Enter 1 to Search the same puzzle again." << endl;
				cout << "Enter 2 to Return to the main menu." << endl;
				samePuzzle = checkInput(5);

				if (samePuzzle == 1)
				{
					chooseSearch(startingBoard);
				}
				system("CLS");
			}

			break;

		case 8:			// 4 -> 5
			temp = workingNode.tilePositions[4];
			workingNode.tilePositions[4] = workingNode.tilePositions[5];
			workingNode.tilePositions[5] = temp;

			// Print the new board after the move
			printPuzzelBoard(workingNode.tilePositions);
			cout << "Puzzel Board after Move " << i + 1 << ": Swap Position 4 and Position 5" << endl;

			if (i != endingNode.moveOrder.size() - 1)
			{
				cout << "Press Enter to perform the next move." << endl;
				cin.get();
				system("CLS");
			}

			else
			{
				cout << "This is the final move.\nPress Enter to continue.\n" << endl;
				cout << "Enter 1 to Search the same puzzle again." << endl;
				cout << "Enter 2 to Return to the main menu." << endl;
				samePuzzle = checkInput(5);

				if (samePuzzle == 1)
				{
					chooseSearch(startingBoard);
				}
				system("CLS");
			}

			break;

		case 9:			// 4 -> 7
			temp = workingNode.tilePositions[4];
			workingNode.tilePositions[4] = workingNode.tilePositions[7];
			workingNode.tilePositions[7] = temp;

			// Print the new board after the move
			printPuzzelBoard(workingNode.tilePositions);
			cout << "Puzzel Board after Move " << i + 1 << ": Swap Position 4 and Position 7" << endl;

			if (i != endingNode.moveOrder.size() - 1)
			{
				cout << "Press Enter to perform the next move." << endl;
				cin.get();
				system("CLS");
			}

			else
			{
				cout << "This is the final move.\nPress Enter to continue.\n" << endl;
				cout << "Enter 1 to Search the same puzzle again." << endl;
				cout << "Enter 2 to Return to the main menu." << endl;
				samePuzzle = checkInput(5);

				if (samePuzzle == 1)
				{
					chooseSearch(startingBoard);
				}
				system("CLS");
			}
			break;

		case 10:		// 5 -> 8
			temp = workingNode.tilePositions[5];
			workingNode.tilePositions[5] = workingNode.tilePositions[8];
			workingNode.tilePositions[8] = temp;

			// Print the new board after the move
			printPuzzelBoard(workingNode.tilePositions);
			cout << "Puzzel Board after Move " << i + 1 << ": Swap Position 5 and Position 8" << endl;

			if (i != endingNode.moveOrder.size() - 1)
			{
				cout << "Press Enter to perform the next move." << endl;
				cin.get();
				system("CLS");
			}

			else
			{
				cout << "This is the final move.\nPress Enter to continue.\n" << endl;
				cout << "Enter 1 to Search the same puzzle again." << endl;
				cout << "Enter 2 to Return to the main menu." << endl;
				samePuzzle = checkInput(5);

				if (samePuzzle == 1)
				{
					chooseSearch(startingBoard);
				}
				system("CLS");
			}

			break;

		case 11:		// 6 -> 7
			temp = workingNode.tilePositions[6];
			workingNode.tilePositions[6] = workingNode.tilePositions[7];
			workingNode.tilePositions[7] = temp;

			// Print the new board after the move
			printPuzzelBoard(workingNode.tilePositions);
			cout << "Puzzel Board after Move " << i + 1 << ": Swap Position 6 and Position 7" << endl;

			if (i != endingNode.moveOrder.size() - 1)
			{
				cout << "Press Enter to perform the next move." << endl;
				cin.get();
				system("CLS");
			}

			else
			{
				cout << "This is the final move.\nPress Enter to continue.\n" << endl;
				cout << "Enter 1 to Search the same puzzle again." << endl;
				cout << "Enter 2 to Return to the main menu." << endl;
				samePuzzle = checkInput(5);

				if (samePuzzle == 1)
				{
					chooseSearch(startingBoard);
				}
				system("CLS");
			}

			break;

		case 12:		// 7 -> 8
			temp = workingNode.tilePositions[7];
			workingNode.tilePositions[7] = workingNode.tilePositions[8];
			workingNode.tilePositions[8] = temp;

			// Print the new board after the move
			printPuzzelBoard(workingNode.tilePositions);
			cout << "Puzzel Board after Move " << i + 1 << ": Swap Position 7 and Position 8" << endl;

			if (i != endingNode.moveOrder.size() - 1)
			{
				cout << "Press Enter to perform the next move." << endl;
				cin.get();
				system("CLS");
			}

			else
			{
				cout << "This is the final move.\nPress Enter to continue.\n" << endl;
				cout << "Enter 1 to Search the same puzzle again." << endl;
				cout << "Enter 2 to Return to the main menu." << endl;
				samePuzzle = checkInput(5);

				if (samePuzzle == 1)
				{
					chooseSearch(startingBoard);
				}
				system("CLS");
			}

			break;
		}

	}

	system("CLS");
	return;
}

// Function to calculate the number of misplaced tiles in a board
int calculateMisplacedTiles(int puzzleState[arraySize])
{
	// integer to hold the number of misplaced tiles
	int numberMisplaced = 0;

	for (int i = 1; i < arraySize - 1; i++)
	{
		if (puzzleState[i] != i)
		{
			numberMisplaced++;
		}
	}

	if (puzzleState[arraySize] != 0)
	{
		numberMisplaced++;
	}

	return numberMisplaced;
}

// Function to calculate the manhattan distance of a board
int calculateManhattanDistance(int puzzleState[arraySize])
{
	// integer to hold the distance
	int manhattanDistance = 0;

	// 2 dimensional representation of the array
	int initialState[3][3] = { 0 };
	int oneDimensionValue = 0;

	// tile number
	int tileNumber = 0;

	// Array to hold the goal index positions for each tile
	int goalRowIndex[9] = {2, 0, 0, 0, 1, 1, 1, 2, 2 };
	int goalColIndex[9] = {2, 0, 1, 2, 0, 1, 2, 0, 1 };

	// convert the inputted one dimensional array to a two dimensional array
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			initialState[i][j] = oneDimensionValue;
			oneDimensionValue++;
		}
	}

	// search the puzzleboard and calculate the manhattan distance for each tile
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			// pull tile from the position of on the board
			tileNumber = initialState[i][j];

			// calculate the correct distance
			switch (tileNumber)
			{
			case 0:
				// YOu do not include the empty space in the calculation
				break;
			case 1:
				manhattanDistance += (abs(i - goalRowIndex[1]) + abs(j - goalColIndex[1]));
				break;
			case 2:
				manhattanDistance += (abs(i - goalRowIndex[2]) + abs(j - goalColIndex[2]));
				break;
			case 3:
				manhattanDistance += (abs(i - goalRowIndex[3]) + abs(j - goalColIndex[3]));
				break;
			case 4:
				manhattanDistance += (abs(i - goalRowIndex[4]) + abs(j - goalColIndex[4]));
				break;
			case 5:
				manhattanDistance += (abs(i - goalRowIndex[5]) + abs(j - goalColIndex[5]));
				break;
			case 6:
				manhattanDistance += (abs(i - goalRowIndex[6]) + abs(j - goalColIndex[6]));
				break;
			case 7:
				manhattanDistance += (abs(i - goalRowIndex[7]) + abs(j - goalColIndex[7]));
				break;
			case 8:
				manhattanDistance += (abs(i - goalRowIndex[8]) + abs(j - goalColIndex[8]));
				break;
			}

		}
	}

	return manhattanDistance;
}


//*************** FUNCTIONS TO CHECK FOR ERRORS ******************************

// Function to check the input from the user
int checkInput(int location)
{
	int n;

	switch (location)
	{
	case 1:				// main menue selection
		while (true)
		{
			cin >> n;

			// break from the loop if the value entered is in range and not already in the puzzle
			if ((cin) && (n > 0) && (n <= 4))
			{
				break;
			}

			// Fix cin if someone bad was entered
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "Invalid Entry: Please enter a value between 1 and 3.\n" << flush;
		}

		return n;

	case 3:			// Select which search

		while (true)
		{
			cin >> n;

			// break from the loop if the value entered is in range and not already in the puzzle
			if ((cin) && (n > 0) && (n <= 4))
			{
				break;
			}

			// Fix cin if someone bad was entered
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "Invalid Entry: Please enter a value between 1 and 4.\n" << flush;
		}

		return n;

		break;

	case 4:		// view results
		while (true)
		{
			cin >> n;

			// break from the loop if the value entered is in range and not already in the puzzle
			if ((cin) && (n > 0) && (n <= 3))
			{
				break;
			}

			// Fix cin if someone bad was entered
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "Invalid Entry: Please enter a value between 1 and 2.\n" << flush;
		}

		return n;

		break;

	case 5:
		while (true)
		{
			cin >> n;

			// break from the loop if the value entered is in range and not already in the puzzle
			if ((cin) && (n > 0) && (n <= 2))
			{
				break;
			}

			// Fix cin if someone bad was entered
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "Invalid Entry: Please enter a value between 1 and 2.\n" << flush;
		}

		return n;

		break;


	}

}



//******************** FUNCTIONS MAKE TESTING RUNS ******************************

// Function to start the test runs of each search
void testRuns()
{
	// create a output stream
	ofstream output;

	// create 10 puzzles
	int puzzleOne[arraySize];
	int puzzleTwo[arraySize];
	int puzzleThree[arraySize];
	int puzzleFour[arraySize];
	int puzzleFive[arraySize];
	int puzzleSix[arraySize];
	int puzzleSeven[arraySize];
	int puzzleEight[arraySize];
	int puzzleNine[arraySize];
	int puzzleTen[arraySize];
	
	// generate 10 random puzzles
	createRandomPuzzel(puzzleOne);
	createRandomPuzzel(puzzleTwo);
	createRandomPuzzel(puzzleThree);
	createRandomPuzzel(puzzleFour);
	createRandomPuzzel(puzzleFive);
	createRandomPuzzel(puzzleSix);
	createRandomPuzzel(puzzleSeven);
	createRandomPuzzel(puzzleEight);
	createRandomPuzzel(puzzleNine);
	createRandomPuzzel(puzzleTen);

	cout << "Creating the output file.\n" << endl;
	// open the output file
	output.open("Search Results.csv");


	//***************************** BREADTH FIRST SEARCHES*******************************
	output << "Breadth First Searches:" << endl;
	output << "Starting Puzzle State:" << "," << "Solution Found:" << "," << "Solution Depth:" << "," << "Path:" << "," << "Nodes Expanded:" << "," << "Search Time (Microseconds):" << "\n";

	system("CLS");
	cout << "Starting Breadth First Searches\n" << endl;

	// Run the Breadth first tests
	cout << "Search One.......";
	breadthFirstTestRun(puzzleOne, output);
	cout << "......Complete\n" << endl;
	cout << "Search Two.......";
	breadthFirstTestRun(puzzleTwo, output);
	cout << "......Complete\n" << endl;
	cout << "Search Three.....";
	breadthFirstTestRun(puzzleThree, output);
	cout << "......Complete\n" << endl;
	cout << "Search Four......";
	breadthFirstTestRun(puzzleFour, output);
	cout << "......Complete\n" << endl;
	cout << "Search Five......";
	breadthFirstTestRun(puzzleFive, output);
	cout << "......Complete\n" << endl;
	cout << "Search Six.......";
	breadthFirstTestRun(puzzleSix, output);
	cout << "......Complete\n" << endl;
	cout << "Search Seven.....";
	breadthFirstTestRun(puzzleSeven, output);
	cout << "......Complete\n" << endl;
	cout << "Search Eight.....";
	breadthFirstTestRun(puzzleEight, output);
	cout << "......Complete\n" << endl;
	cout << "Search Nine......";
	breadthFirstTestRun(puzzleNine, output);
	cout << "......Complete\n" << endl;
	cout << "Search Ten.......";
	breadthFirstTestRun(puzzleTen, output);
	cout << "......Complete\n" << endl;

	output << "\n" << " " << "," << " " << "," << " " << "," << " " << "," << "Average:" << "," << "Average:" << endl;
	output << " " << " " << "," << " " << "," << "Puzzle Positions" << "," << "0 1 2" << "," << "=AVERAGE(E3:E12)" << "," << "=AVERAGE(F3:F12)" << endl;
	output << " " << "," << " " << "," << " for Reference" << "," << "3 4 5" << endl;
	output << " " << "," << " " << "," << "with Path" << "," << "6 7 8" << endl;
	   	 

	//***************************** DEPTH FIRST SEARCHES*******************************
	system("CLS");
	output << "\nDepth First Searches" << endl;
	output << "Starting Puzzle State:" << "," << "Solution Found:" << "," << "Solution Depth:" << "," << "Path:" << "," << "Nodes Expanded:" << "," << "Search Time (Microseconds):" << "\n";

	cout << "Starting Depth First Searches\n" << endl;

	// Run the Breadth first tests
	cout << "Search One.......";
	depthFirstTestRun(puzzleOne, output);
	cout << "......Complete\n" << endl;
	cout << "Search Two.......";
	depthFirstTestRun(puzzleTwo, output);
	cout << "......Complete\n" << endl;
	cout << "Search Three.....";
	depthFirstTestRun(puzzleThree, output);
	cout << "......Complete\n" << endl;
	cout << "Search Four......";
	depthFirstTestRun(puzzleFour, output);
	cout << "......Complete\n" << endl;
	cout << "Search Five......";
	depthFirstTestRun(puzzleFive, output);
	cout << "......Complete\n" << endl;
	cout << "Search Six.......";
	depthFirstTestRun(puzzleSix, output);
	cout << "......Complete\n" << endl;
	cout << "Search Seven.....";
	depthFirstTestRun(puzzleSeven, output);
	cout << "......Complete\n" << endl;
	cout << "Search Eight.....";
	depthFirstTestRun(puzzleEight, output);
	cout << "......Complete\n" << endl;
	cout << "Search Nine......";
	depthFirstTestRun(puzzleNine, output);
	cout << "......Complete\n" << endl;
	cout << "Search Ten.......";
	depthFirstTestRun(puzzleTen, output);
	cout << "......Complete\n" << endl;

	output << "\n" << " " << "," << " " << "," << " " << "," << " " << "," << "Average:" << "," << "Average:" << endl;
	output << " " << " " << "," << " " << "," << "Puzzle Positions" << "," << "0 1 2" << "," << "=AVERAGE(E21:E30)" << "," << "=AVERAGE(F21:F30)" << endl;
	output << " " << "," << " " << "," << " for Reference" << "," << "3 4 5" << endl;
	output << " " << "," << " " << "," << "with Path" << "," << "6 7 8" << endl;


	//**************************** MISPLACED TILES SEARCHES*******************************
	system("CLS");
	output << "\nMisplaced Tiles A* Searches" << endl;
	output << "Starting Puzzle State:" << "," << "Solution Found:" << "," << "Solution Depth:" << "," << "Path:" << "," << "Nodes Expanded:" << "," << "Search Time (Microseconds):" << "\n";

	cout << "Starting Misplaced Tiles A* Searches\n" << endl;

	// Run the Breadth first tests
	cout << "Search One.......";
	missingTilesTestRun(puzzleOne, output);
	cout << "......Complete\n" << endl;
	cout << "Search Two.......";
	missingTilesTestRun(puzzleTwo, output);
	cout << "......Complete\n" << endl;
	cout << "Search Three.....";
	missingTilesTestRun(puzzleThree, output);
	cout << "......Complete\n" << endl;
	cout << "Search Four......";
	missingTilesTestRun(puzzleFour, output);
	cout << "......Complete\n" << endl;
	cout << "Search Five......";
	missingTilesTestRun(puzzleFive, output);
	cout << "......Complete\n" << endl;
	cout << "Search Six.......";
	missingTilesTestRun(puzzleSix, output);
	cout << "......Complete\n" << endl;
	cout << "Search Seven.....";
	missingTilesTestRun(puzzleSeven, output);
	cout << "......Complete\n" << endl;
	cout << "Search Eight.....";
	missingTilesTestRun(puzzleEight, output);
	cout << "......Complete\n" << endl;
	cout << "Search Nine......";
	missingTilesTestRun(puzzleNine, output);
	cout << "......Complete\n" << endl;
	cout << "Search Ten.......";
	missingTilesTestRun(puzzleTen, output);
	cout << "......Complete\n" << endl;

	output << "\n" << " " << "," << " " << "," << " " << "," << " " << "," << "Average:" << "," << "Average:" << endl;
	output << " " << " " << "," << " " << "," << "Puzzle Positions" << "," << "0 1 2" << "," << "=AVERAGE(E39:E48)" << "," << "=AVERAGE(F39:F48)" << endl;
	output << " " << "," << " " << "," << " for Reference" << "," << "3 4 5" << endl;
	output << " " << "," << " " << "," << "with Path" << "," << "6 7 8" << endl;




	//**************************** MANHATTAN DISTANCE SEARCHES*******************************
	system("CLS");
	output << "\nMANHATTAN DISTANCE A* Searches" << endl;
	output << "Starting Puzzle State:" << "," << "Solution Found:" << "," << "Solution Depth:" << "," << "Path:" << "," << "Nodes Expanded:" << "," << "Search Time (Microseconds):" << "\n";

	cout << "Starting Manhattan Distance A* Searches\n" << endl;

	// Run the Breadth first tests
	cout << "Search One.......";
	manhattanDistanceTestRun(puzzleOne, output);
	cout << "......Complete\n" << endl;
	cout << "Search Two.......";
	manhattanDistanceTestRun(puzzleTwo, output);
	cout << "......Complete\n" << endl;
	cout << "Search Three.....";
	manhattanDistanceTestRun(puzzleThree, output);
	cout << "......Complete\n" << endl;
	cout << "Search Four......";
	manhattanDistanceTestRun(puzzleFour, output);
	cout << "......Complete\n" << endl;
	cout << "Search Five......";
	manhattanDistanceTestRun(puzzleFive, output);
	cout << "......Complete\n" << endl;
	cout << "Search Six.......";
	manhattanDistanceTestRun(puzzleSix, output);
	cout << "......Complete\n" << endl;
	cout << "Search Seven.....";
	manhattanDistanceTestRun(puzzleSeven, output);
	cout << "......Complete\n" << endl;
	cout << "Search Eight.....";
	manhattanDistanceTestRun(puzzleEight, output);
	cout << "......Complete\n" << endl;
	cout << "Search Nine......";
	manhattanDistanceTestRun(puzzleNine, output);
	cout << "......Complete\n" << endl;
	cout << "Search Ten.......";
	manhattanDistanceTestRun(puzzleTen, output);
	cout << "......Complete\n" << endl;

	output << "\n" << " " << "," << " " << "," << " " << "," << " " << "," << "Average:" << "," << "Average:" << endl;
	output << " " << " " << "," << " " << "," << "Puzzle Positions" << "," << "0 1 2" << "," << "=AVERAGE(E57:E66)" << "," << "=AVERAGE(F57:F66)" << endl;
	output << " " << "," << " " << "," << " for Reference" << "," << "3 4 5" << endl;
	output << " " << "," << " " << "," << "with Path" << "," << "6 7 8" << endl;

	// close the output file
	output.close();

	cout << "\nThe searches have been complete.\nCheck the file (Search Results.csv) for results." << endl;
	cout << "Press Enter to continue..." << endl;
	cin.get();
	system("CLS");
	return;
}

// Function to perform a breadth first search
void breadthFirstTestRun(int slidePuzzelArray[arraySize], ofstream &output)
{

	// Declare start and finish variables to be used for the the timings
	auto start = chrono::high_resolution_clock::now();
	auto stop = chrono::high_resolution_clock::now();
	
	// Declare variable to hold the time it took to search
	auto searchTime = chrono::duration_cast<chrono::milliseconds>(stop - start);

	// Create Queue
	queue<slidePuzzelNode> nodeQueue;

	// Vector to be used to store the moves to be done
	vector<int> moves;

	// Count to hold the depth of the node
	int depthCount = 0;

	// count of nodes expanded
	int nodesExpanded = 0;

	// Integer to hold user input to view results
	int viewResults = 0;

	// boolean to hold if the puzzle was solved
	bool solved = false;

	// array to hold a temporary state
	int tempArray[arraySize] = { 0 };

	// string to hold the string representaiton of a generated state
	string moveState;

	// integer to accompany the strings in the map
	int mapCount = 1;

	// array to hold the the goal state
	int goalArray[arraySize] = { 1, 2, 3, 4, 5, 6, 7, 8, 0 };

	// array to hold onto the tileArray in the front of the queue
	int currentFrontArray[arraySize];

	// Start the timer
	start = chrono::high_resolution_clock::now();


	// Create the first state.
	slidePuzzelNode puzzleState;

	// Copy the initial state into a working state
	// copy the tile positions
	for (int i = 0; i < arraySize; i++)
	{
		puzzleState.tilePositions[i] = slidePuzzelArray[i];
	}

	// Create the string representation of the tile state
	puzzleState.positions = createStringRepresentation(puzzleState.tilePositions);

	// assign the depthcout of zero to the initial state
	puzzleState.depth = depthCount;

	// put the starting node in the map
	visitedMap.insert(pair<string, int>(puzzleState.positions, mapCount));
	mapCount++;

	// Add the first state to the queue
	nodeQueue.push(puzzleState);

	// while loop to check and see if the puzzel was solved
	while (!solved)
	{
		// make sure the queue is not empty
		if (nodeQueue.empty())
		{
			// stop the clock
			stop = chrono::high_resolution_clock::now();

			// calculate the time it took to finish
			searchTime = chrono::duration_cast<chrono::milliseconds>(stop - start);

			// output the results to the file
			output << puzzleState.positions << "," << "No" << "," << "N/A" << "," << "No Path Found" << "," << nodesExpanded << "," << searchTime.count() << endl;
			
			// clear the map
			visitedMap.clear();
			return;
		}

		// Evaluate the first node to see if it is in the solved state.
		if (nodeQueue.front().positions == "123456780")
		{
			solved = true;

			// stop the clock
			stop = chrono::high_resolution_clock::now();

			// calculate the time it took to finish
			searchTime = chrono::duration_cast<chrono::milliseconds>(stop - start);

			// output the results to the file
			output << puzzleState.positions << "," << "Yes" << "," << depthCount << ",";
			printMoves(nodeQueue.front().moveOrder, output);
			output << "," << nodesExpanded << "," << searchTime.count() << endl;

			// clear the map
			visitedMap.clear();
			return;
		}

		// Copy the front of the queue to an array
		for (int i = 0; i < arraySize; i++)
		{
			currentFrontArray[i] = nodeQueue.front().tilePositions[i];
		}


		// Else statement to move on from the current node if it is not the solved puzzel
		if (solved != true)
		{
			// Generate Moves depending on position of the 0
			generateMoves(currentFrontArray, moves);

			// increment nodes expanded
			nodesExpanded++;

			// for loop to perform each move
			for (int i = 0; i < moves.size(); i++)
			{
				//initialize tempArray
				initializePuzzelBoard(tempArray);

				// make the move and store it in the new node
				makeMove(tempArray, moves[i], currentFrontArray);

				// generate the string for the new node
				moveState = createStringRepresentation(tempArray);

				// Save the front of the queue's depth
				depthCount = nodeQueue.front().depth;

				// Add the new node to the queue only if it's not already there
				if (stateVisited(moveState) == false)
				{
					// write the move into the map
					visitedMap.insert(pair<string, int>(moveState, mapCount));

					// Increment mapCount
					mapCount++;

					// Create new node
					slidePuzzelNode generatedState;

					// Set the depth count of the new node
					generatedState.depth = depthCount + 1;

					// Save the string representation of the state
					generatedState.positions = moveState;

					// Add the tempArray to the new node
					for (int i = 0; i < arraySize; i++)
					{
						generatedState.tilePositions[i] = tempArray[i];
					}

					// Add the current node's path to new node
					for (int i = 0; i < nodeQueue.front().moveOrder.size(); i++)
					{
						generatedState.moveOrder.push_back(nodeQueue.front().moveOrder[i]);
					}

					// Add the move performed into movePath of the node
					generatedState.moveOrder.push_back(moves[i]);

					// push the new state to the queue
					nodeQueue.push(generatedState);
				}
			}

			// clear the moves
			moves.clear();

			// pop the stack
			nodeQueue.pop();
		}
	}
	return;
}

// Function to perform a depthFirst Search
void depthFirstTestRun(int slidePuzzleArray[arraySize], ofstream &output)
{

	// Declare start and finish variables to be used for the the timings
	auto start = chrono::high_resolution_clock::now();
	auto stop = chrono::high_resolution_clock::now();

	// Declare variable to hold the time it took to search
	auto searchTime = chrono::duration_cast<chrono::milliseconds>(stop - start);

	// create the stack
	stack<slidePuzzelNode> nodeStack;

	// Vector to be sued to store the moves to be done
	vector<int> moves;

	// integer to hold the depth 
	int depthCount = 0;

	// integer to hold the number of nodes expanded
	int nodesExpanded = 0;

	// boolean to hold if the puzzle has been solved
	bool solved = false;

	// array to hold an array
	int tempArray[arraySize] = { 0 };

	// string to hold the string representation of a state
	string moveState;

	// int to hold an integer to accompiany a string in the map
	int mapCount = 1;

	// array to hold the goal state
	int goalArray[arraySize] = { 1,2,3,4,5,6,7,8,0 };

	// arra to hold the tilePositions from the top of the stack
	int currentTopArray[arraySize] = { 0 };

	// Create the first state
	slidePuzzelNode puzzleState;

	// set the tile positions array
	for (int i = 0; i < arraySize; i++)
	{
		puzzleState.tilePositions[i] = slidePuzzleArray[i];
	}

	// create the string representation to be compared
	puzzleState.positions = createStringRepresentation(puzzleState.tilePositions);

	// Set the dpeth of the initial node
	puzzleState.depth = depthCount;

	// put the starting state in the map
	visitedMap.insert(pair<string, int>(puzzleState.positions, mapCount));

	// Increment the map counter
	mapCount++;

	// Add the initial state to the stack
	nodeStack.push(puzzleState);

	//while loop to create new states until there are no more viable ones or the goal is found
	while (!solved)
	{
		// Make sure the stack is not empty
		if (nodeStack.empty())
		{
			stop = chrono::high_resolution_clock::now();

			// calculate the time it took to finish
			searchTime = chrono::duration_cast<chrono::milliseconds>(stop - start);

			// output the results to the file
			output << puzzleState.positions << "," << "No" << "," << "N/A" << "," << "N/A" << "," << nodesExpanded << "," << searchTime.count() << endl;

			// clear the map
			visitedMap.clear();
			return;
		}

		// Evaluate the top of the stack and see if it is the goal state
		if (nodeStack.top().positions == "123456780")
		{
			solved = true;

			stop = chrono::high_resolution_clock::now();

			// calculate the time it took to finish
			searchTime = chrono::duration_cast<chrono::milliseconds>(stop - start);

			// output the results to the file
			output << puzzleState.positions << "," << "Yes" << "," << depthCount << "," << "N/A" << "," << nodesExpanded << "," << searchTime.count() << endl;	   			 

			// clear the map
			visitedMap.clear();
			return;
		}

		// copy the top of the stack to an array
		for (int i = 0; i < arraySize; i++)
		{
			currentTopArray[i] = nodeStack.top().tilePositions[i];
		}

		// copy the depth count of top of the stack
		depthCount = nodeStack.top().depth;

		// Pop the stack
		nodeStack.pop();

		// create the moves that are possible from this state
		if (solved != true)
		{
			// generate themoves depending on the position of the 0 in the initial state
			generateMoves(currentTopArray, moves);

			// incrmenet nodes expanded
			nodesExpanded++;

			// for loop to make each move and put it on the stack if a new state is generated
			for (int i = 0; i < moves.size(); i++)
			{
				// initialize the temp array
				initializePuzzelBoard(tempArray);

				// Make the move and store it in a new node
				makeMove(tempArray, moves[i], currentTopArray);

				// Generate the string for the new node
				moveState = createStringRepresentation(tempArray);

				// Add the new node to the stack only if it's not already there
				if (stateVisited(moveState) == false)
				{
					// Write the new state to the map
					visitedMap.insert(pair<string, int>(moveState, mapCount));

					// incremnet mapCount
					mapCount++;

					// create a new node
					slidePuzzelNode generatedState;

					// set the depthCOunt of the new node
					generatedState.depth = depthCount + 1;

					// set the state string
					generatedState.positions = moveState;

					// Add the tempAray to the new node
					for (int i = 0; i < arraySize; i++)
					{
						generatedState.tilePositions[i] = tempArray[i];
					}

					// Add the new state to the stack
					nodeStack.push(generatedState);
				}
			}
			// clear the moves
			moves.clear();
		}
	}
	return;
}

// Function to perform a search based on total misplaced tiles
void missingTilesTestRun(int slidePuzzelArray[arraySize], ofstream &output)
{
	
	// Declare start and finish variables to be used for the the timings
	auto start = chrono::high_resolution_clock::now();
	auto stop = chrono::high_resolution_clock::now();

	// Declare variable to hold the time it took to search
	auto searchTime = chrono::duration_cast<chrono::milliseconds>(stop - start);

	// Create Queue
	priority_queue<slidePuzzelNode> nodeQueue;

	// Vector to be used to store the moves to be done
	vector<int> moves;

	// temporary vector to hold onto moves while generating children
	vector<int> currentStateMoves;

	// Count to hold the depth of the node
	int depthCount = 0;

	// count to hold nodes expanded
	int nodesExpanded = 0;

	// Integer to hold user input to view results
	int viewResults = 0;

	// boolean to hold if the puzzle was solved
	bool solved = false;

	// array to hold a temporary state
	int tempArray[arraySize] = { 0 };

	// string to hold the string representaiton of a generated state
	string moveState;

	// integer to accompany the strings in the map
	int mapCount = 1;

	// array to hold the the goal state
	int goalArray[arraySize] = { 1, 2, 3, 4, 5, 6, 7, 8, 0 };

	// array to hold onto the tileArray in the front of the queue
	int currentFrontArray[arraySize];

	// Integer to hold the top of the queue's heuristic info
	int currentHeuristicInfo = 0;

	// Create the first state.
	slidePuzzelNode puzzleState;

	// Copy the initial state into a working state
	// copy the tile positions
	for (int i = 0; i < arraySize; i++)
	{
		puzzleState.tilePositions[i] = slidePuzzelArray[i];
	}

	// Create the string representation of the tile state
	puzzleState.positions = createStringRepresentation(puzzleState.tilePositions);

	// assign the depthcout of zero to the initial state
	puzzleState.depth = depthCount;

	// Calculate the number of misplaced tiles for the state
	puzzleState.heurisiticInfo = calculateMisplacedTiles(puzzleState.tilePositions);

	// put the starting node in the map
	visitedMap.insert(pair<string, int>(puzzleState.positions, mapCount));
	mapCount++;

	// Add the first state to the queue
	nodeQueue.push(puzzleState);

	// while loop to check and see if the puzzel was solved
	while (!solved)
	{
		// make sure the queue is not empty
		if (nodeQueue.empty())
		{

			stop = chrono::high_resolution_clock::now();

			// calculate the time it took to finish
			searchTime = chrono::duration_cast<chrono::milliseconds>(stop - start);

			// output the results to the file
			output << puzzleState.positions << "," << "No" << "," << "N/A" << "," << "No Path Found" << "," << nodesExpanded << "," << searchTime.count() << endl;
			
			// Clear the map
			visitedMap.clear();
			return;
		}

		// Evaluate the first node to see if it is in the solved state.
		if (nodeQueue.top().positions == "123456780")
		{
			solved = true;
			
			// stop the clock
			stop = chrono::high_resolution_clock::now();

			// calculate the time it took to finish
			searchTime = chrono::duration_cast<chrono::milliseconds>(stop - start);

			// output the results to the file
			output << puzzleState.positions << "," << "Yes" << "," << depthCount << ",";
			printMoves(nodeQueue.top().moveOrder, output);
			output << "," << nodesExpanded << "," << searchTime.count() << endl;
			
			// clear the map
			visitedMap.clear();
			return;
		}

		// Copy the front of the queue to an array
		for (int i = 0; i < arraySize; i++)
		{
			currentFrontArray[i] = nodeQueue.top().tilePositions[i];
		}

		// clear the moves from the last node
		currentStateMoves.clear();

		if (nodeQueue.top().moveOrder.empty() != true)
		{
			// Hold on to the current state's moves
			for (int i = 0; i < nodeQueue.top().moveOrder.size(); i++)
			{
				currentStateMoves.push_back(nodeQueue.top().moveOrder[i]);
			}
		}

		// Save the front of the queue's depth
		depthCount = nodeQueue.top().depth;

		// hold on to the front of the queue's hueristic info
		currentHeuristicInfo = nodeQueue.top().heurisiticInfo;

		// pop the stack
		nodeQueue.pop();

		// Else statement to move on from the current node if it is not the solved puzzel
		if (solved != true)
		{
			// Generate Moves depending on position of the 0
			generateMoves(currentFrontArray, moves);

			// increment nodes expanded
			//nodesExpanded++;

			// Increment nodes expanded (just created children
			nodesExpanded++;

			// for loop to perform each move
			for (int i = 0; i < moves.size(); i++)
			{
				//initialize tempArray
				initializePuzzelBoard(tempArray);

				// make the move and store it in the new node
				makeMove(tempArray, moves[i], currentFrontArray);

				// generate the string for the new node
				moveState = createStringRepresentation(tempArray);

				// Add the new node to the queue only if it's not already there
				if (stateVisited(moveState) == false)
				{
					// write the move into the map
					visitedMap.insert(pair<string, int>(moveState, mapCount));

					// Increment mapCount
					mapCount++;

					// Create new node
					slidePuzzelNode generatedState;

					// Set the depth count of the new node
					generatedState.depth = depthCount + 1;

					// Save the string representation of the state
					generatedState.positions = moveState;

					// Add the tempArray to the new node
					for (int i = 0; i < arraySize; i++)
					{
						generatedState.tilePositions[i] = tempArray[i];
					}

					// calculate the number of misplaced tiles for the generated state
					generatedState.heurisiticInfo = calculateMisplacedTiles(generatedState.tilePositions) + currentHeuristicInfo;


					// Add the current node's path to new node
					for (int i = 0; i < currentStateMoves.size(); i++)
					{
						generatedState.moveOrder.push_back(currentStateMoves[i]);
					}

					// Add the move performed into movePath of the node
					generatedState.moveOrder.push_back(moves[i]);

					// push the new state to the queue
					nodeQueue.push(generatedState);
				}
			}

			// clear the moves
			moves.clear();
		}
	}
	return;
}

// Function to perform a search based on the Manhattan Distance
void manhattanDistanceTestRun(int slidePuzzelArray[arraySize], ofstream &output)
{
	// Declare start and finish variables to be used for the the timings
	auto start = chrono::high_resolution_clock::now();
	auto stop = chrono::high_resolution_clock::now();

	// Declare variable to hold the time it took to search
	auto searchTime = chrono::duration_cast<chrono::milliseconds>(stop - start);
	// Create Queue
	priority_queue<slidePuzzelNode> nodeQueue;

	// Integer to hold how many nodes were expanded
	int nodesExpanded = 0;

	// Vector to be used to store the moves to be done
	vector<int> moves;

	// temporary vector to hold onto moves while generating children
	vector<int> currentStateMoves;

	// Count to hold the depth of the node
	int depthCount = 0;

	// integer to hold onto the top of the queue's heurisitic info
	int currentHeuristicInfo = 0;

	// Integer to hold user input to view results
	int viewResults = 0;

	// boolean to hold if the puzzle was solved
	bool solved = false;

	// array to hold a temporary state
	int tempArray[arraySize] = { 0 };

	// string to hold the string representaiton of a generated state
	string moveState;

	// integer to accompany the strings in the map
	int mapCount = 1;

	// array to hold the the goal state
	int goalArray[arraySize] = { 1, 2, 3, 4, 5, 6, 7, 8, 0 };

	// array to hold onto the tileArray in the front of the queue
	int currentFrontArray[arraySize];

	// Create the first state.
	slidePuzzelNode puzzleState;

	// Copy the initial state into a working state
	// copy the tile positions
	for (int i = 0; i < arraySize; i++)
	{
		puzzleState.tilePositions[i] = slidePuzzelArray[i];
	}

	// Create the string representation of the tile state
	puzzleState.positions = createStringRepresentation(puzzleState.tilePositions);

	// assign the depthcout of zero to the initial state
	puzzleState.depth = depthCount;

	// Calculate the number of misplaced tiles for the state
	puzzleState.heurisiticInfo = calculateManhattanDistance(puzzleState.tilePositions);

	// put the starting node in the map
	visitedMap.insert(pair<string, int>(puzzleState.positions, mapCount));
	mapCount++;

	// Add the first state to the queue
	nodeQueue.push(puzzleState);

	// while loop to check and see if the puzzel was solved
	while (!solved)
	{
		// make sure the queue is not empty
		if (nodeQueue.empty())
		{

			stop = chrono::high_resolution_clock::now();

			// calculate the time it took to finish
			searchTime = chrono::duration_cast<chrono::milliseconds>(stop - start);

			// output the results to the file
			output << puzzleState.positions << "," << "No" << "," << "N/A" << "," << "No Path Found" << "," << nodesExpanded << "," << searchTime.count() << endl;

			// Clear the map
			visitedMap.clear();
			return;
		}

		// Evaluate the first node to see if it is in the solved state.
		if (nodeQueue.top().positions == "123456780")
		{
			solved = true;

			// stop the clock
			stop = chrono::high_resolution_clock::now();

			// calculate the time it took to finish
			searchTime = chrono::duration_cast<chrono::milliseconds>(stop - start);

			// output the results to the file
			output << puzzleState.positions << "," << "Yes" << "," << depthCount << ",";
			printMoves(nodeQueue.top().moveOrder, output);
			output << "," << nodesExpanded << "," << searchTime.count() << endl;

			// clear the map
			visitedMap.clear();
			return;
		}

		// Copy the front of the queue to an array
		for (int i = 0; i < arraySize; i++)
		{
			currentFrontArray[i] = nodeQueue.top().tilePositions[i];
		}

		// clear the moves from the last node
		currentStateMoves.clear();

		if (nodeQueue.top().moveOrder.empty() != true)
		{
			// Hold on to the current state's moves
			for (int i = 0; i < nodeQueue.top().moveOrder.size(); i++)
			{
				currentStateMoves.push_back(nodeQueue.top().moveOrder[i]);
			}
		}

		// Save the front of the queue's depth
		depthCount = nodeQueue.top().depth;

		// save the front of the queue's heurisitc info
		currentHeuristicInfo = nodeQueue.top().heurisiticInfo;

		// pop the stack
		nodeQueue.pop();

		// Else statement to move on from the current node if it is not the solved puzzel
		if (solved != true)
		{
			// Generate Moves depending on position of the 0
			generateMoves(currentFrontArray, moves);

			// Increment nodes expanded (just created children
			nodesExpanded++;

			// for loop to perform each move
			for (int i = 0; i < moves.size(); i++)
			{
				//initialize tempArray
				initializePuzzelBoard(tempArray);

				// make the move and store it in the new node
				makeMove(tempArray, moves[i], currentFrontArray);

				// generate the string for the new node
				moveState = createStringRepresentation(tempArray);

				// Add the new node to the queue only if it's not already there
				if (stateVisited(moveState) == false)
				{
					// write the move into the map
					visitedMap.insert(pair<string, int>(moveState, mapCount));

					// Increment mapCount
					mapCount++;

					// Create new node
					slidePuzzelNode generatedState;

					// Set the depth count of the new node
					generatedState.depth = depthCount + 1;

					// Save the string representation of the state
					generatedState.positions = moveState;

					// Add the tempArray to the new node
					for (int i = 0; i < arraySize; i++)
					{
						generatedState.tilePositions[i] = tempArray[i];
					}

					// calculate the number of misplaced tiles for the generated state
					generatedState.heurisiticInfo = calculateManhattanDistance(generatedState.tilePositions) + currentHeuristicInfo;

					// Add the current node's path to new node
					for (int i = 0; i < currentStateMoves.size(); i++)
					{
						generatedState.moveOrder.push_back(currentStateMoves[i]);
					}

					// Add the move performed into movePath of the node
					generatedState.moveOrder.push_back(moves[i]);

					// push the new state to the queue
					nodeQueue.push(generatedState);
				}
			}

			// clear the moves
			moves.clear();
		}
	}
	return;
}

// Function to print a description of the moves made to the file
void printMoves(vector<int> moves, ofstream &output)
{

	/*
Generate the moves
Numbers being inserted into the vectors respond to moves :
1 - Swap 0 and 1 Positions
2 - Swap 0 and 3 Positions
3 - Swap 1 and 2 Positions
4 - Swap 1 and 4 Positions
5 - Swap 2 and 5 Positions
6 - Swap 3 and 4 Positions
7 - Swap 3 and 6 Positions
8 - Swap 4 and 5 Positions
9 - Swap 4 and 7 Positions
10 - Swap 5 and 8 Positions
11 - Swap 6 and 7 Positions
12 - Swap 7 and 8 Positions

*/
	// Check each move in the vector
	for (int i = 0; i < moves.size(); i++)
	{
		switch (moves[i])
		{
		// Print the actual swap being performed based on the move number
		case 1:
			if (i != moves.size() - 1)
			{
				output << "( 0 -> 1 ) - ";
			}

			else
			{
				output << "(0 -> 1)";
			}
			break;
		case 2:
			if (i != moves.size() - 1)
			{
				output << "( 0 -> 3 ) - ";
			}

			else
			{
				output << "(0 -> 3)";
			}
			break;
		case 3:
			if (i != moves.size() - 1)
			{
				output << "( 1 -> 2 ) - ";
			}

			else
			{
				output << "(1 -> 2)";
			}
			break;
		case 4:
			if (i != moves.size() - 1)
			{
				output << "( 1 -> 4 ) - ";
			}

			else
			{
				output << "(1 -> 4)";
			}
			break;
		case 5:
			if (i != moves.size() - 1)
			{
				output << "( 2 -> 5 ) - ";
			}

			else
			{
				output << "(2 -> 5)";
			}
			break;
		case 6:
			if (i != moves.size() - 1)
			{
				output << "( 3 -> 4 ) - ";
			}

			else
			{
				output << "(3 -> 4)";
			}
			break;
		case 7:
			if (i != moves.size() - 1)
			{
				output << "( 3 -> 6 ) - ";
			}

			else
			{
				output << "(3 -> 6)";
			}
			break;
		case 8:
			if (i != moves.size() - 1)
			{
				output << "( 4 -> 5 ) - ";
			}

			else
			{
				output << "(4 -> 5)";
			}
			break;
		case 9:
			if (i != moves.size() - 1)
			{
				output << "( 4 -> 7 ) - ";
			}

			else
			{
				output << "(4 -> 7)";
			}
			break;
		case 10:
			if (i != moves.size() - 1)
			{
				output << "( 5 -> 8 ) - ";
			}

			else
			{
				output << "(5 -> 8)";
			}
			break;
		case 11:
			if (i != moves.size() - 1)
			{
				output << "( 6 -> 7 ) - ";
			}

			else
			{
				output << "(6 -> 7)";
			}
			break;
		case 12:
			if (i != moves.size() - 1)
			{
				output << "( 7 -> 8 ) - ";
			}

			else
			{
				output << "(7 -> 8)";
			}
			break;
		}
	}
	return;
}




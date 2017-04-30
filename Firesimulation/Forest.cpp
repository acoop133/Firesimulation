#include "Forest.h"

Forest::Forest(Parameters params)
{
	std::system("cls");
	this->params = params;
	
	int xAxis = params.forestSize[0].first;
	int yAxis = params.forestSize[0].second;
	
	int xFireStart = params.fireStartCoordinate[0].first;
	int yFireStart = params.fireStartCoordinate[0].second;
	untouchedTreeVector = new vector<Tree*>(); // initiate vector
	burningTreeVector = new vector<Tree*>();
	for (int y = 0; y < yAxis; y++)
	{
		for (int x = 0; x < xAxis; x++)
		{

			if (y == 0 || y == yAxis-1)
			{

				forestMap[x][y] = boundry;  // North and south boundary layers
			}
			else if (x == 0 || x == xAxis-1)
			{
				forestMap[x][y] = boundry;  // East and west boundary layers
			}
			else if (x == xFireStart && y == yFireStart)  // always have a tree in center
			{
				forestMap[x][y] = untouched;
			}
			else
			{
				if (params.mapGenMode == fullMap)
				{
					forestMap[x][y] = untouched;
				}
				else
				{
					if (rand() % 100 < params.randomGenProbability) // make map unique 
					{
						forestMap[x][y] = untouched;  // Everything else
					}
				}

			}

		}
	}
	generateTrees();

}
Forest::~Forest() {}

void Forest::displayForest()
{
	// 2d coordinate reference alphabet
 char alphabetArray[26] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T'};
	// 2d coordinate reference alphabet

	int xAxis = params.forestSize[0].first;
	int yAxis = params.forestSize[0].second;

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	for (int y = 0; y < yAxis; y++)
	{
		
		for (int x = 0; x < xAxis; x++)
		{
			//if (x == -1 && y == -1) // vertical
			//{
			//	std::cout << alphabetArray[y - 3 + 2];
			//	x + 1;
			//}
			//std::cout << " ";
			//if (y == -1 && x > 1) // horizontal
			//{

			//		std::cout << alphabetArray[x - 2];

			//	y + 1; // return y to 0
			//}
			//else {

			std::cout << " ";

				if (forestMap[x][y] == untouched) {
					SetConsoleTextAttribute(hConsole, 10); // green
				}
				else if (forestMap[x][y] == burning)
				{
					SetConsoleTextAttribute(hConsole, 12); // red
				}
				else if (forestMap[x][y] == boundry)
				{
					SetConsoleTextAttribute(hConsole, 15); // white
				}
				else if (forestMap[x][y] == dead)
				{
					SetConsoleTextAttribute(hConsole, 14); // white
				}
				std::cout << forestMap[x][y];
			//}
			
		}
		std::cout << std::endl;
	}

}


void Forest::generateTrees()
{
	int xAxis = params.forestSize[0].first;
	int yAxis = params.forestSize[0].second;


	for (int y = 0; y < yAxis; y++)
	{
		for (int x = 0; x < xAxis; x++)
		{
			if (forestMap[x][y] == untouched)
			{
				Tree* tree = new Tree(x, y, this);  // new tree
				untouchedTreeVector->push_back(tree);  // add tree to list
			}
		}
	}



}

void Forest::updateMap()
{

	// increment and delete any dead trees in burning vector  and update map

	for (unsigned int i = 0; i < burningTreeVector->size(); i++)
	{
		(*burningTreeVector)[i]->cycleState();

		Tree* temp = (*burningTreeVector)[i];
		int X = temp->pos_x;
		int Y = temp->pos_y;


		if (temp->state == burning)
		{

			temp->location->forestMap[X][Y] = temp->state;  // update state on map

		}
		else if (temp->state == dead)
		{
			temp->location->forestMap[X][Y] = temp->state;  // update state on map
			burningTreeVector->erase(burningTreeVector->begin() + i);

			i--; // -1 to i to  compensate delete
			// delete object
		}
	}

}
void Forest::surroundingTreeFate(Tree tree)
{
	
	int dir = 0; // Set direction to zero
	double setFireProbability = tree.dryness;
	// declare 8 cells around tree
	int temp[8][2] = { { -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };
	// move cells into vector
	vector<pair<int,int>> dirAry = vector<pair<int, int>>();
	for ( unsigned int i = 0; i < 8; i++) 
	{
		
		dirAry.push_back(make_pair( temp[i][0] , temp[i][1]));
	}
	

	
	if (params.windSpeed == low)
	{
		setFireProbability = setFireProbability * 1.1;
	}
	else if (params.windSpeed == high)
	{
		setFireProbability = setFireProbability * 1.4;

	}

	
	//This while loop scans through the Direction Array to help determine
	// the probability of neighboring trees being on fire
	while (dir < dirAry.size())
	{
		double probAfterCalc=0;

		int X = tree.pos_x + dirAry[dir].first;
		int Y = tree.pos_y + dirAry[dir].second;
		
		int dX = dirAry[dir].first;
		int dY = dirAry[dir].second;
		if (forestMap[X][Y] == untouched)
		{
			if (params.windSpeed == none)
			{
				probAfterCalc = setFireProbability;
			}
			else if (params.windSpeed == low)
			{
				// lessen opposite directions chance to set afire
				double divide = 1.2;
				probAfterCalc = setFireProbability;
				switch (params.windDirection)
				{
				case north:
				{
					if ((dX == -1 && dY == 0) || (dX == -1 && dY == 1) || (dX == 0 && dY == 1) ||
						(dX == 1 && dY == 1) || (dX == 1 && dY == 0))
					{
						probAfterCalc = setFireProbability / divide;
					}
					break;
				}
				case south:
				{
					if ((dX == -1 && dY == 0) || (dX == -1 && dY == -1) || (dX == 0 && dY == -1) ||
						(dX == 1 && dY == -1) || (dX == 1 && dY == 0))
					{
						probAfterCalc = setFireProbability / divide;
					}
					break;
				}
				case east:
				{
					if ((dX == 0 && dY == 1) || (dX == -1 && dY == 1) || (dX == -1 && dY == 0) ||
						(dX == -1 && dY == -1) || (dX == 0 && dY == -1))
					{
						probAfterCalc = setFireProbability / divide;
					}
					break;
				}
				case west:
				{
					if ((dX == 0 && dY == -1) || (dX == 1 && dY == -1) || (dX == 1 && dY == 0) ||
						(dX == 1 && dY == 1) || (dX == 0 && dY == 1))
					{
						probAfterCalc = setFireProbability / divide;
					}
					break;
				}
				case northEast:
				{
					if ((dX == -1 && dY == -1) || (dX == -1 && dY == 0) || (dX == -1 && dY == 1) ||
						(dX == 0 && dY == 1) || (dX == 1 && dY == 1))
					{
						probAfterCalc = setFireProbability / divide;
					}
					break;
				}
				case southEast:
				{
					if ((dX == -1 && dY == -1) || (dX == -1 && dY == 0) || (dX == -1 && dY == 1) ||
						(dX == 0 && dY == -1) || (dX == 1 && dY == -1))
					{
						probAfterCalc = setFireProbability / divide;
					}
					break;
				}
				case southWest:
				{
					if ((dX == -1 && dY == -1) || (dX == 0 && dY == -1) || (dX == 1 && dY == -1) ||
						(dX == 1 && dY == 0) || (dX == 1 && dY == 1))
					{
						probAfterCalc = setFireProbability / divide;
					}
					break;
				}
				case northWest:
				{
					if ((dX == 1 && dY == -1) || (dX == 1 && dY == 0) || (dX == 1 && dY == 1) ||
						(dX == 0 && dY == 1) || (dX == -1 && dY == 1))
					{
						probAfterCalc = setFireProbability / divide;
					}
					break;
				}
				}
			}
			else if (params.windSpeed == high)
			{ 
				// lessen opposite directions chance to set afire
				double divide = 11;
				probAfterCalc = setFireProbability;
				switch (params.windDirection)
				{
				case north:
				{
					if ((dX == -1 && dY == 0) || (dX == -1 && dY == 1) || (dX == 0 && dY == 1) ||
						(dX == 1 && dY == 1) || (dX == 1 && dY == 0))
					{
						probAfterCalc = setFireProbability / divide;
					}
					break;
				}
				case south:
				{
					if ((dX == -1 && dY == 0) || (dX == -1 && dY == -1) || (dX == 0 && dY == -1) ||
						(dX == 1 && dY == -1) || (dX == 1 && dY == 0))
					{
						probAfterCalc = setFireProbability / divide;
					}
					break;
				}
				case east:
				{
					if ((dX == 0 && dY == 1) || (dX == -1 && dY == 1) || (dX == -1 && dY == 0) ||
						(dX == -1 && dY == -1) || (dX == 0 && dY == -1))
					{
						probAfterCalc = setFireProbability / divide;
					}
					break;
				}
				case west:
				{
					if ((dX == 0 && dY == -1) || (dX == 1 && dY == -1) || (dX == 1 && dY == 0) ||
						(dX == 1 && dY == 1) || (dX == 0 && dY == 1))
					{
						probAfterCalc = setFireProbability / divide;
					}
					break;
				}
				case northEast:
				{
					if ((dX == -1 && dY == -1) || (dX == -1 && dY == 0) || (dX == -1 && dY == 1) || 
						(dX == 0 && dY == 1) || (dX == 1 && dY == 1))
					{
						probAfterCalc = setFireProbability / divide;
					}
					break;
				}
				case southEast:
				{
					if ((dX == -1 && dY == -1) || (dX == -1 && dY == 0) || (dX == -1 && dY == 1) ||
						(dX == 0 && dY == -1) || (dX == 1 && dY == -1))
					{
						probAfterCalc = setFireProbability / divide;
					}
					break;
				}
				case southWest:
				{
					if ((dX == -1 && dY == -1) || (dX == 0 && dY == -1) || (dX == 1 && dY == -1) ||
						(dX == 1 && dY == 0) || (dX == 1 && dY == 1))
					{
						probAfterCalc = setFireProbability / divide;
					}
					break;
				}
				case northWest:
				{
					if ((dX == 1 && dY == -1) || (dX == 1 && dY == 0) || (dX == 1 && dY == 1) ||
						(dX == 0 && dY == 1) || (dX == -1 && dY == 1))
					{
						probAfterCalc = setFireProbability / divide;
					}
					break;
				}
				}


			}
			// calculate fate

			if (rand() % 100 < probAfterCalc)
			{
				// get tree object and set afire

				for (int i = 0; i < untouchedTreeVector->size(); i++)
				{


					if ((*untouchedTreeVector)[i]->pos_x == X && (*untouchedTreeVector)[i]->pos_y == Y)
					{

						(*burningTreeVector).push_back((*untouchedTreeVector)[i]->clone());
						(*untouchedTreeVector).erase((*untouchedTreeVector).begin() + i);

					}
				}
			}

		}
		dir++;

	}
}

Tree::Tree(int x, int y, Forest* forest)
{
	location = forest;
	pos_x = x;
	pos_y = y;
	state = untouched;

	switch (forest->params.weather)
	{
	case drought: { dryness = 90; break; }
	case sunny: { dryness = 50; break; }
	case lightRain: { dryness = 40; break; }
	case heavyRain: { dryness = 20; break; }

	}

}

Tree::~Tree(){}


void Tree::cycleState()
{

	if (lifePoints > 0)
	{
		lifePoints--;
	}

	if (lifePoints == 1)
	{
		state = burning;

	}
	if (lifePoints == 0)
	{
		state = dead;
	}




}

Tree* Tree::clone()
{
	Tree* newTree = new Tree(pos_x, pos_y, location);
	newTree->state = state;
	newTree->lifePoints = lifePoints;

	return newTree;
}
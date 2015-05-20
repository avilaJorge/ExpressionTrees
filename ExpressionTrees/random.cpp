/*Random.cpp
*Implements the Random Number interface
*/
#include "random.h"
#include <stdlib.h>
#include <windows.h>
#include <cmath>

/*Prototypes for "private functions"
*Function: initRandomSeed
*Usage: initRandomSeed()
*Seeds the random number generator, if it has not been
*previously seeded.
*/
void initRandomSeed();

/*Function: randomInteger
Implementation notes = how did I solve the problem?
*/
int randomInteger(int a, int b)
{
	initRandomSeed();
	double normal = rand() / (double(RAND_MAX) + 1);
	double scaled = normal*(double(b) - a + 1);
	return int(floor(a + scaled));
}

/*Function: randomReal
Similar to randomInt - no conversion to int at the end
*/
double randomReal(double a, double b)
{
	initRandomSeed();
	double normal = rand() / (double(RAND_MAX) + 1);
	double scaled = normal*(double(b) - a);
	return a + scaled;
}

/*Function: randomChance
*/
bool randomChance(double percent)
{
	initRandomSeed();
	return randomReal(0, 1) < percent;
}

/*Function: initRandomSeed
*/
void initRandomSeed()
{
	static bool initialized = false;

	if (!initialized)
	{
		srand(GetTickCount());
		initialized = true;
	}
}

void generateRandomVector(std::vector<int>& data, int n)
{
	initRandomSeed();
	//preallocate space
	data.resize(n);

	for (int i = 0; i < data.size(); i++)
	{
		data[i] = rand();
	}

	/*for (int& element : data)
	{
	element = rand();
	}*/
}
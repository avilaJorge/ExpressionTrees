/*Random.h
*Exports functionality for generating random integers and
*real numbers within a specified range. Also exports a function
*for generating true with some specified probability.
*/
#ifndef _RANDOM_H
#define _RANDOM_H
#include <vector>

/*Function: randomInteger
*Usage: number = randomInteger(low,high);
*Description: Returns a pseudo-random integer between
*low and high, inclusive.
*/
int randomInteger(int a, int b);

/*Function: randomReal
*Usage: realNumber = randomReal(low, high)
*Description: Returns a pseudo-random double value
*in the range [low,high).
*/
double randomReal(double a, double b);

/*Function: randomChance
*Usage: if(randomChance(percent)) ...
*Description: Returns true with probability percent, and
*false with probability 1 - percent.
*/
bool randomChance(double percent);

/*Function: generateRandomVector
*Usage: generateRandomVector(data, n)
*Description: Fills data with n random integers, in the full range
*of the 32-bit int data type.  Any data that was previously in data
*is destroyed.
*/
void generateRandomVector(std::vector<int>& data, int n);

#endif
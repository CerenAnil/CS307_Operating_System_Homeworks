/*
g++ -o pthread3.out pthread3.cpp -lpthread
*/

#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

int matrixM[2][50];
int seatNum = 0;
int turn = 1;
bool full = false;

void *reserve(void *param)
{
	int *myTurn = (int*)param;
	while (!full)
	{
		while (!full&&turn != *myTurn);
		if (full)
			return NULL;
		int randNum1 = rand() % 2; //random row
		int randNum2 = rand() % 50; //random column

		cout << "ENTERING CRITICAL ZONE" << *myTurn << endl;
		while (!full &&matrixM[randNum1][randNum2] != 0)
		{
			randNum1 = rand() % 2;
			randNum2 = rand() % 50;
			matrixM[randNum1][randNum2];
		}

		matrixM[randNum1][randNum2] = *myTurn;

		if (randNum1 + 1 % 2 == 0) //calculate seat number
		{
			seatNum = (2 * (randNum2 + 1) - 1);
		}
		else
			seatNum = (2 * (randNum2 + 1));

		cout << "Seat Number " << seatNum << " are reserved by TravelAgency" << *myTurn << endl;
		cout << "EXITING CRITICAL ZONE" << *myTurn << endl << endl << endl;

		if (turn == 1)
			turn = 2;
		else if (turn == 2)
			turn = 3;
		else if (turn == 3)
			turn = 1;
	}
	if (full)
		return NULL;
}



bool isFull() //returns false if plane is not full
{
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 50; j++) {
			if (matrixM[i][j] == 0)
			{
				return false;
			}
		}
	}
	return true;
}

int main()
{
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 50; j++) {
			matrixM[i][j] = 0;
		}
	}

	pthread_t TravelAgency1, TravelAgency2, TravelAgency3;
	int TAI1 = 1, TAI2 = 2, TAI3 = 3;

	pthread_create(&TravelAgency1, NULL, reserve, (void *)& TAI1);
	pthread_create(&TravelAgency2, NULL, reserve, (void *)& TAI2);
	pthread_create(&TravelAgency3, NULL, reserve, (void *)& TAI3);

	while (isFull() == false);
	full = true;

	pthread_join(TravelAgency1, NULL);
	pthread_join(TravelAgency2, NULL);
	pthread_join(TravelAgency3, NULL);


	for (int i = 0; i < 2; i++) { //print plane
		for (int j = 0; j < 50; j++) {
			cout << matrixM[i][j] << " ";
		}
		cout << "\n";
	}
	return 0;
}

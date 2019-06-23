#include <iostream>
#include <cstdlib>
#include <time.h>
#include "mpi.h"

using namespace std;

void shellSort(int* arr, int n)
{
	int gap;
	for (gap = 1; gap < n / 3; gap = gap * 3 + 1);
	for (; gap > 0; gap = (gap - 1) / 3)
	{
		for (int i = gap; i < n; i++)
		{
			int temp = arr[i];
			int j;
			for (j = i; j >= gap && arr[j - gap] > temp; j -= gap)
				arr[j] = arr[j - gap];

			arr[j] = temp;
		}
	}
}

int main(int argc, char **argv) {
	int size, rank, arraySize = 1000000;
	int *array = new int[arraySize];
	MPI_Status status;


	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	srand(time(NULL));

	if (rank == 0) {
		for (int i = 0; i < arraySize; i++) {
			array[i] = rand() % 100000 + 1;
			cout << "array[" << i << "] = " << array[i] << '\n';
		}
	}

	int *subArray;
	int n = size > 1 ? arraySize / (size - 1) : arraySize;
	double begin;
	double end;

	if (rank == 0) {
		begin = clock();

		for (int i = 1; i < size; i++) {
			MPI_Send(array + n * (i - 1), n, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		int k = arraySize - n * (size - 1);
		subArray = new int[k];

		for (int i = n * (size - 1); i < arraySize; i++) {
			subArray[i - n * (size - 1)] = array[i];
		}

		shellSort(subArray, k);

		int *rArray = new int[arraySize];

		for (int i = 0; i < k; i++) {
			rArray[i] = subArray[i];
		}

		for (int i = 1; i < size; i++) {
			MPI_Recv(rArray + n * (i - 1) + k, n, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
		}

		shellSort(rArray, arraySize);
		end = clock();

		for (int i = 0; i < arraySize; i++)
			cout << "sorted array[" << i << "] = " << rArray[i] << '\n';

		cout << "Time elapsed " << (end - begin) / CLOCKS_PER_SEC << "seconds";
	}
	else
	{
		subArray = new int[n];
		MPI_Recv(subArray, n, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		shellSort(subArray, n);
		MPI_Send(subArray, n, MPI_INT, 0, 1, MPI_COMM_WORLD);
	}

	MPI_Finalize();

	return 0;
}

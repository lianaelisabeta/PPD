
#include <iostream>
#include "mpi.h"
#include <fstream>
#include <stdio.h>  
using namespace std;
int main()
{
	
	float mycoef = 1;
	float valoare = 0;
	int stop = 0;
	
	float myvalue;
	//porneste fork
	MPI_Status status;
	int rc = MPI_Init(NULL, NULL);
	if (rc != MPI_SUCCESS) {
		std::cout << "Error Mpi fork";
		MPI_Abort(MPI_COMM_WORLD, rc);
	}

	int N,id;
	MPI_Comm_size(MPI_COMM_WORLD, &N);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	
	

	if (id == 0) { //e procesul master
		float el;
		float coef[100];


		ifstream in;
		in.open("polinom.txt");
		if (in.fail())
		{
			cout << "Can't open file" << endl;
			MPI_Abort(MPI_COMM_WORLD, 1);
		}

		int j = 0;
		while (in >> el) {
			cout << "el= " << el<<" ";
			coef[j] = el;
			j++;
		}

		in.close();

		
		
		myvalue = mycoef;
		mycoef = coef[0];
		cout << "Id: " << id << " Coef: " << mycoef << endl;
		//trm fiecarui proces coeficientul citit corespunzator + tag 0 pt ca e coeficient
		for (int i = 1; i < j; i++) {

			MPI_Send(&coef[i], 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
			//cout << coef[i] << " ";
		}

		

	}

	else { // e un proces intermediar
		MPI_Recv(&mycoef, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
		cout << "Id: " << id << " Coef: " << mycoef << endl;
	}

	if (id == 0) {
		ifstream invalori;
		invalori.open("input.txt");
		if (invalori.fail())
		{
			cout << "Can't open file" << endl;
			MPI_Abort(MPI_COMM_WORLD, 1);
		}

		int idx = 1;
		float val;

		while (invalori >> val) {
			valoare = val;
			cout << "id: " << id << " val= " << valoare << " ";
			MPI_Bcast(&valoare, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
			MPI_Send(&myvalue, 1, MPI_FLOAT, 1, 1, MPI_COMM_WORLD);
			
			
		}

		invalori.close();
		int stops[100];
		for (int i1 = 0; i1 < N; i1++) {
			stops[i1] = 1;
		}

		MPI_Scatter(&stops, 1, MPI_INT, &stop, 1, MPI_INT, 0, MPI_COMM_WORLD);
		


	}
	else if (id == 1) {
		while (stop != 1)
		{
			MPI_Bcast(&valoare, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
			cout << "ID: " << id << " Receive x =" << valoare << endl;
			MPI_Recv(&myvalue, 1, MPI_FLOAT, 0, 1, MPI_COMM_WORLD, &status);
			cout << "ID: " << id << " Receive myvalue =" << myvalue << endl;
			myvalue = myvalue * valoare + mycoef;
			MPI_Send(&myvalue, 1, MPI_FLOAT, id + 1, 1, MPI_COMM_WORLD);
		}
	}
	else if(id==N-1) {
		while (stop != 1)
		{
			MPI_Bcast(&valoare, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
			cout << "ID: " << id << " Receive x =" << valoare << endl;
			MPI_Recv(&myvalue, 1, MPI_FLOAT, id - 1, 1, MPI_COMM_WORLD, &status);
			cout << "ID: " << id << " Receive myvalue =" << myvalue << endl;
			ofstream out("output.txt",std::ofstream::app);

			out << myvalue << ",";

			out.close();
		}
	}
	else {
		while (stop != 1)
		{
			MPI_Bcast(&valoare, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
			cout << "ID: " << id << " Receive x =" << valoare << endl;
			MPI_Recv(&myvalue, 1, MPI_FLOAT, id - 1, 1, MPI_COMM_WORLD, &status);
			cout << "ID: " << id << " Receive myvalue =" << myvalue << endl;
			myvalue = myvalue * valoare + mycoef;
			MPI_Send(&myvalue, 1, MPI_FLOAT, id + 1, 1, MPI_COMM_WORLD);
		}
	}
	

	
	MPI_Finalize();
    
	return 0;
}

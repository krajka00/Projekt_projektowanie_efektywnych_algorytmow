#include<iostream>					
#include<vector>
#include<time.h>					
#include<stdlib.h>					
#include<fstream>					
#include<sstream>
#include<cstring>
#include<algorithm>					
#include<random>

using namespace std;

#define INIT_FILE "DataInit.ini"	

int N;										
vector < vector<int> > adjacencyMatrix;		
vector <int> outcome;						
int minSum;
int numberOfFirstVertex = 0;				


vector <string> fileNameV;
vector <int> numberOfTestsV;
vector <int> solutionV;
vector <string> traceV;
string outputFileName;


void wczytajPlikIni(string FileName)
{
	fstream file;

	file.open(FileName.c_str(), ios::in);
	string line;

	string fileName;
	int numberOfTests;
	int solution;
	string trace;
	string outputFileExtension = ".csv";

	if (file.good() == true)
	{
		while (getline(file, line))
		{
			stringstream sstream(line);
			sstream >> fileName;

			if (fileName.find(outputFileExtension) != string::npos) 
			{
				outputFileName = fileName;
				break;
			}
			fileNameV.push_back(fileName);
			sstream >> numberOfTests;
			numberOfTestsV.push_back(numberOfTests);
			sstream >> solution;
			solutionV.push_back(solution);
			getline(sstream, trace);        
			traceV.push_back(trace);
		}
	}
	else  cout << "Wystapil problem podczas otwierania pliku ini"<<"\n";
}

void odczytajDane(string fileName)
{
	fstream file;

	file.open(fileName.c_str(), ios::in);
	string line;
	int value;
	vector <int> row;

	int rowCounter = 0;

	if (file.good() == true)
	{
		while (getline(file, line))
		{
			stringstream sstream(line);
			if (rowCounter == 0)	
			{
				sstream >> value;
				N = value;
				rowCounter++;
				continue;
			}

			row.resize(0);	// clear
			adjacencyMatrix.push_back(row);

			for (int j = 0; j < N; j++)	
			{
				sstream >> value;
				adjacencyMatrix[rowCounter - 1].push_back(value);
			}
			rowCounter++;
		}
	}
	else  cout << "Wystapil problem podczas otwierania pliku testu:" << fileName << " :(";
}

string odwiezoneWektoryToString(vector <int> vectorOfVisitedVertexes)
{
	string visitedString = "";

	for (int i = vectorOfVisitedVertexes.size() - 1; i >= 0; i--)		
	{
		if (i != vectorOfVisitedVertexes.size() - 1 && i != 0) visitedString += to_string(vectorOfVisitedVertexes[i]) + " ";
		else if (i == vectorOfVisitedVertexes.size() - 1) visitedString += "[" + to_string(vectorOfVisitedVertexes[i]) + " ";
		else visitedString += to_string(vectorOfVisitedVertexes[i]) + "]";
	}

	return visitedString;
}

void wydrukujTrase(string msg, vector<int> solution, int cost)
{
	cout << msg + ": ";
	for (int i = 0; i < solution.size(); i++)
	{
		cout << solution[i] << " ";
	}
	cout << " Cost: " << cost << "\n";
}


int obliczKosztRozwiazania(vector<int> solution)
{

	int cost = 0;

	for (int i = 0; i < solution.size() - 1; i++)
	{
		cost += adjacencyMatrix[solution[i]][solution[i + 1]];
	}

	cost += adjacencyMatrix[numberOfFirstVertex][solution[0]];					
	cost += adjacencyMatrix[solution[solution.size() - 1]][numberOfFirstVertex];	

	return cost;
}


vector<int> rozwiazanieInvert(vector<int> oldSolution) 
{
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0, 1);	

	int randIndexI = (int)(dis(gen) * 10000) % oldSolution.size();
	int randIndexJ = (int)(dis(gen) * 10000) % oldSolution.size();

	// cout <<"\n i: "<< randIndexI << " j: " << randIndexJ << "\n";

	if (randIndexI == randIndexJ) return oldSolution; 

	if (randIndexI < randIndexJ)
	{
		while (randIndexI < randIndexJ)
		{
			swap(oldSolution[randIndexI], oldSolution[randIndexJ]);
			randIndexI++;
			randIndexJ--;
		}
	}
	else
	{
		while (randIndexJ < randIndexI)
		{
			swap(oldSolution[randIndexJ], oldSolution[randIndexI]);
			randIndexJ++;
			randIndexI--;
		}
	}

	return oldSolution;	
}


vector<int> Rozwiazanie2zmiana(vector<int> oldSolution)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0, 1);	
	int newSolutionCost;


	int randIndexI = (int)(dis(gen) * 10000) % oldSolution.size();	
	int randIndexJ = (int)(dis(gen) * 10000) % oldSolution.size();

	if (randIndexI == randIndexJ) return oldSolution;
	swap(oldSolution[randIndexI], oldSolution[randIndexJ]);

	return oldSolution;

}

float nowaTempGeo(float staraTemperatura, float temp)	{
	return temp * staraTemperatura;
}

float nowaTempLog(float staraTemperatura, int n)	
{
	return staraTemperatura / log(n);
}

// Random solution, checked
vector<int> rozwiazaniePoczatkowe()
{
	vector<int> wynik;
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0, 1);	
	int randIndexI, randIndexJ;

	for (int i = 1; i < N; i++) wynik.push_back(i);

	for (int i = 0; i < N; i++)
	{
		randIndexI = (int)(dis(gen) * 10000) % wynik.size();	
		randIndexJ = (int)(dis(gen) * 10000) % wynik.size();
		swap(wynik[randIndexI], wynik[randIndexJ]);
	}
	return wynik;
}

float obliczTempPocz(int kosztRozwiazaniaPocz, float alpha)
{
	return kosztRozwiazaniaPocz * alpha;
}

int obliczDlugoscTrasy(int rozmiarInstancji, float alpha)
{
	return (int)rozmiarInstancji * alpha;
}

void symulowaneWyzarzanie(clock_t czasStartu)
{
	vector<int> stareRozwiazanie, noweRozwiazanie;													
	float aktTemp;
	int dlugoscEry, calkDlugoscEry = 0, kosztStaregoRozw, kosztNowegoRozw, delta;
	clock_t stop;
	double time = 0;

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(0, 1);												
	float randomValue;

	stareRozwiazanie = rozwiazaniePoczatkowe();													
	kosztStaregoRozw = obliczKosztRozwiazania(stareRozwiazanie);
	float alpha = 0.999;
	float beginningTemperature = obliczTempPocz(kosztStaregoRozw, N);		
	//cout << beginningTemperature<<"\n";
	aktTemp = beginningTemperature;
	dlugoscEry = obliczDlugoscTrasy(N, 6);											

	while (aktTemp > pow(10, -9) && time < 400000)							
	{
		
		for (int i = 0; i < dlugoscEry; i++)												
		{
			noweRozwiazanie = Rozwiazanie2zmiana(stareRozwiazanie);
			//noweRozwiazanie = rozwiazanieInvert(oldSolution);
			kosztNowegoRozw = obliczKosztRozwiazania(noweRozwiazanie);
			delta = kosztNowegoRozw - kosztStaregoRozw;
			if (delta < 0)																
			{
				stareRozwiazanie = noweRozwiazanie;
				kosztStaregoRozw = kosztNowegoRozw;
			}
			else
			{
				randomValue = dis(gen);													
				if (randomValue < exp(-delta / aktTemp))						
				{
					stareRozwiazanie = noweRozwiazanie;
					kosztStaregoRozw = kosztNowegoRozw;
				}
			}
		}
		calkDlugoscEry += dlugoscEry;
		aktTemp = nowaTempGeo(aktTemp, alpha);
		//aktTemp = nowaTempLog(currentTemperature, eraLengthSum);

		stop = clock();
		time = (double)(stop - czasStartu) / (CLOCKS_PER_SEC / 1000);
	}

	outcome.push_back(numberOfFirstVertex);
	for (int i = 0; i < stareRozwiazanie.size(); i++) outcome.push_back(stareRozwiazanie[i]);
	outcome.push_back(numberOfFirstVertex);

	minSum = kosztStaregoRozw;
}



void hardClear()
{
	adjacencyMatrix.resize(0);									
	outcome.resize(0);
}

void softClear()		
{
	outcome.resize(0);
}



int main()
{
	wczytajPlikIni(INIT_FILE);
	ofstream myfile;
	myfile.open(outputFileName);
	clock_t start, stop;
	double czas, sredni_czas;
	string trace;

	hardClear();

	for (int i = 0; i < fileNameV.size(); i++)
	{
		myfile << fileNameV[i] << ";" << numberOfTestsV[i] << ";" << solutionV[i] << ";" << traceV[i];

		odczytajDane(fileNameV[i]);	

		myfile << "\n";
		sredni_czas = 0;
		for (int j = 0; j < numberOfTestsV[i]; j++)
		{
			cout << j << "\n";
			start = clock();
			symulowaneWyzarzanie(start);
			stop = clock();
			czas = (double)(stop - start) / (CLOCKS_PER_SEC / 1000);
			sredni_czas = sredni_czas + czas;
			trace = odwiezoneWektoryToString(outcome);

			myfile << czas << ";" << minSum << ";" << trace << "\n";
			softClear();
		}
		sredni_czas = sredni_czas / numberOfTestsV[i];
		myfile << "Sredni czas" << ";" << sredni_czas << "\n";

		hardClear();  

		myfile << "\n";
	}

	myfile.close();
	return 0;
}
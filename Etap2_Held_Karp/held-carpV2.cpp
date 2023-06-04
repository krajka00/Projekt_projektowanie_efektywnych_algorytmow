#include<iostream>			// wejscie, wyjscie
#include<vector>
#include<time.h>            // mierzenie czasu
#include<stdlib.h>          // mierzenie czasu
#include<fstream>			// wejscie/wyjscie do operacji na plikach
#include<sstream>
#include<cstring>
#include<algorithm>			// std::sort
#include <unordered_map>	// mapa
using namespace std;

#define INIT_FILE "Data.ini"

struct myTypeForMap
{
	vector<int> subset;
	int actualVertex;

	bool operator==(const myTypeForMap& other) const
	{
		return subset == other.subset && actualVertex == other.actualVertex;
	}

};
class MyHashFunction {
public:

	size_t operator()(const myTypeForMap& p) const
	{
		size_t size = 0;

		for (int i = 0; i < p.subset.size(); i++) size ^= hash<int>()(p.subset[i]);
		size ^= hash<int>()(p.actualVertex);

		return size;
	}
};


unordered_map<myTypeForMap, int, MyHashFunction> mapCostSum;		// klucz pierwszy, wartość druga, klasa z funkcją haszujaca
unordered_map<myTypeForMap, int, MyHashFunction> mapParents;		// klucz pierwszy, wartość druga, klasa z funkcją haszujaca


int numbOfCity;										// liczba miast
vector < vector<int> > adjacencyMatrix;		// macierz sasiedztwa
vector < vector<int> > subsets;				// wszystkie podzbiory zbioru N - elementowego [ od 1 do N-1], zakladam, ze 0 jest poczatkowym wierzcholkiem
vector <int> road;						// trasa znalezionego rozwiazania

int minimalSum = 999999999;						// minimum globalne (sumy wag krawedzi)
int firstVertex = 0;				// number wierzcholka, od ktorego rozpoczynamy

vector <string> fileNameV;
vector <int> numberOfIteration;
vector <int> solutionV;
vector <string> traceV;

string outputFileName;





void makeSubsets(int noOfVertexesNow, int maxNoOfVertexes)	// podzbior miast bez poczatkowego
{
	if (noOfVertexesNow > maxNoOfVertexes) return;

	vector<int> subset;
	if (noOfVertexesNow == 1)		// jeden wierzcholek "1"
	{
		subsets.push_back(subset);		// pusty subset
		subset.push_back(1);			// subset z "1"        
		subsets.push_back(subset);
		subset.resize(0);
		if (maxNoOfVertexes == 1) return;
		else
		{
			makeSubsets(noOfVertexesNow + 1, maxNoOfVertexes);
			return;
		}
	}

	// cout << noOfVertexesNow;

	int numbOfCities = subsets.size();
	for (int i = 0; i < numbOfCities; i++)
	{
		subset = subsets[i];
		subset.push_back(noOfVertexesNow);
		subsets.push_back(subset);
	}
	subset.resize(0);
	makeSubsets(noOfVertexesNow + 1, maxNoOfVertexes);
}


int getCostOfRoad(vector<int> actualSubset, int prevVertex)
{
	actualSubset.erase(find(actualSubset.begin(), actualSubset.end(), prevVertex));  

	myTypeForMap myMap;
	myMap.subset = actualSubset;
	myMap.actualVertex = prevVertex;

	return  mapCostSum.at(myMap);
}

string vectorOfVisitedVertexesToString(vector <int> vectorOfVisitedVertexes)
{
	string visitedToString = "";

	for (int i = vectorOfVisitedVertexes.size() - 1; i >= 0; i--)		// odczytujemy od tylu bo wierzcholki byly pushowane od tylu
	{
		if (i != vectorOfVisitedVertexes.size() - 1 && i != 0) visitedToString += to_string(vectorOfVisitedVertexes[i]) + " ";
		else if (i == vectorOfVisitedVertexes.size() - 1) visitedToString += "[" + to_string(vectorOfVisitedVertexes[i]) + " ";
		else visitedToString += to_string(vectorOfVisitedVertexes[i]) + "]";
	}

	return visitedToString;
}


void HeldKarp()
{
	int actualSum;
	makeSubsets(1, numbOfCity - 1);
	sort(subsets.begin(), subsets.end(), [](const vector<int>& a, const vector<int>& b) { return a.size() < b.size(); });	// sortuje podzbiory po rozmiarze

	for (int i = 0; i < subsets.size(); i++)		//  Dla wszystkich podzbiorów wykonaj
	{
		for (int j = 1; j < numbOfCity; j++)					//  Dla wszystkich wierzcholkow wykonaj:
		{
			if (find(subsets[i].begin(), subsets[i].end(), j) != subsets[i].end()) continue; // Jezeli set zawiera wierzcholek to continue
			int localMinimum = 99999999999;
			int parentOfCurrently = 0;

			for (int k = 0; k < subsets[i].size(); k++)	// iteruj po kazdym elemencie z tego podzbioru, zeby skonczyc na tym wierzcholku; nie wykona sie jak jest pusty
			{
				int actualCost = adjacencyMatrix[subsets[i][k]][j] + getCostOfRoad(subsets[i], subsets[i][k]);
				if (actualCost < localMinimum)
				{
					localMinimum = actualCost;
					parentOfCurrently = subsets[i][k];
				}
			}

			if (subsets[i].size() == 0)
			{
				localMinimum = adjacencyMatrix[0][j];
				parentOfCurrently = 0;
			}

			myTypeForMap myMap;
			myMap.actualVertex = j;
			myMap.subset = subsets[i];

			mapParents.insert(make_pair(myMap, parentOfCurrently));
			mapCostSum.insert(make_pair(myMap, localMinimum));
		}
	}

	myTypeForMap myMap;
	vector<int> subset = subsets[subsets.size() - 1];
	myMap.subset = subset;

	for (int i = 1; i < numbOfCity; i++)		// Dodajemu ostatnią krawędź do 0
	{
		actualSum = adjacencyMatrix[i][firstVertex] + getCostOfRoad(myMap.subset, i);
		if (minimalSum > actualSum)
		{
			minimalSum = actualSum;
			myMap.actualVertex = i;
		}
	}
//Zwracamy droge
	road.push_back(firstVertex);
	road.push_back(myMap.actualVertex);

	int parentVertex = 0;

	subset.erase(find(subset.begin(), subset.end(), myMap.actualVertex));
	myMap.subset = subset;

	while (road.size() != numbOfCity)
	{
		parentVertex = mapParents.at(myMap);
		myMap.actualVertex = parentVertex;
		subset.erase(find(subset.begin(), subset.end(), myMap.actualVertex));
		myMap.subset = subset;
		road.push_back(parentVertex);
	}

	road.push_back(firstVertex);
}

void readIniFile(string FileName)
{
	fstream inFile;

	inFile.open(FileName.c_str(), ios::in);
	string line;

	string fileName;
	int numberOfTest;
	int solution;
	string trace;
	string outputFileExtension = ".csv";

	if (inFile.good() == true)
	{
		while (getline(inFile, line))
		{
			stringstream strStream(line);
			strStream >> fileName;

			if (fileName.find(outputFileExtension) != string::npos) 
			{
				outputFileName = fileName;
				break;
			}
			fileNameV.push_back(fileName);
			strStream >> numberOfTest;
			numberOfIteration.push_back(numberOfTest);
			strStream >> solution;
			solutionV.push_back(solution);
			getline(strStream, trace);        
			traceV.push_back(trace);
		}
	}
	else  cout << "Problem z otwarciem pliku wejsciowego";
}

void readTestFile(string fileName)
{
	fstream outFile;

	outFile.open(fileName.c_str(), ios::in);
	string line;
	int value;
	vector <int> actualRow;

	int rowCounter = 0;

	if (outFile.good() == true)
	{
		while (getline(outFile, line))
		{
			stringstream sstream(line);
			if (rowCounter == 0)	
			{
				sstream >> value;
				numbOfCity = value;
				rowCounter++;
				continue;
			}

			actualRow.resize(0);	
			adjacencyMatrix.push_back(actualRow);

			for (int j = 0; j < numbOfCity; j++)	
			{
				sstream >> value;
				adjacencyMatrix[rowCounter - 1].push_back(value);
			}
			rowCounter++;
		}
	}
	else  cout << "Problem z utworzeniem pliku wyjsciowego" << fileName ;
}
void hardClear()
{
	minimalSum = 999999999;
	adjacencyMatrix.resize(0);									
	subsets.resize(0);
	road.resize(0);
	mapParents.clear();
	mapCostSum.clear();
}

void softClear()		
{
	minimalSum = 999999999;
	subsets.resize(0);
	road.resize(0);
	mapParents.clear();
	mapCostSum.clear();
}
int main()
{
	readIniFile(INIT_FILE);
	ofstream myfile;
	myfile.open(outputFileName);		// otwarcie pliku csv 
	clock_t start, stop;
	double czas;
	string trace;

	hardClear();

	for (int i = 0; i < fileNameV.size(); i++)
	{
		myfile << fileNameV[i] << ";" << numberOfIteration[i] << ";" << solutionV[i] << ";" << traceV[i];

		readTestFile(fileNameV[i]);	// wczytanie danych testowych

		myfile << "\n";

		for (int j = 0; j < numberOfIteration[i]; j++)
		{
			start = clock();
			HeldKarp();
			stop = clock();
			czas = (double)(stop - start) / (CLOCKS_PER_SEC / 1000);

			trace = vectorOfVisitedVertexesToString(road);

			myfile << czas << ";" << minimalSum << ";" << trace << "\n";
			softClear();
		}

		hardClear();  

		myfile << "\n";
	}

	myfile.close();


	return 0;
}
import itertools 
import pandas as pd
import time 

def Brute_force_func(cities, matrix):

  start = time.time()                                                                   #Rozppoczecie mierzenia czasu

  min_distance = 99999                                                                  #Zmienna do zapisywania minimalnego dystansu
  min_road = []                                                                         #Zmienna do zapamietania minimalnej sciezki 
  for value in itertools.permutations ([i for i in range(cities)]):   #Przejscie po wszystkich permutacjach sciezki
    temp = 0                                                                            #Zmienna do tymczasowego przechowywania kosztu
    for j in range (len(value)-1) :                                                     #Przejscie po miastach w sciezce
      temp += matrix[value[j]][value[j+1]]                                              #Zwiekszenie kosztu tymczasowego zwiazanego z przejsciem
    temp += matrix[value[-1]][value[0]]                                                 #Dodanie kosztu przejscia z ostatniego miasta do poczatkowego
    if temp < min_distance:                                                             #Sprawdzenie czy Nowy dystans jest mniejszy niz dotychczasowy
      min_distance = temp                                                               #Jezeli tak to podmiana wszystkich danych               
      min_road = value
    
  duration = time.time()-start                                                          #Koniec pomiaru czasu
  return  str(duration), str(min_road), str(min_distance)                               #Zwrot czasu trwania, minimalnej sciezki i dystansu
if __name__ == "__main__":
    iniFile = pd.read_csv("Brute_force.INI", sep = " ")                             #Wczytywanie pliku INI

    for i in range (len(iniFile)):                                                  #Wykonywanie dopoki nie przejdziemy po wszystkich danych z pliku INI
        with open ("test_brute_force.csv",'a') as fileResult:                       #Otwieranie pliku wynikowego
            print(f'Aktualnie linia: {i}')                                  
            filename = iniFile.iloc[i]["nazwa_instancji"]                           #Wczytywanie nazwy z pliku INI
            iloscPowt = iniFile.iloc[i]["ilosc_powtorzen"]                          #Wczytanie ilosci powtorzen z pliku INI
            fileResult.write( ' '.join(map(str,iniFile.values[i:i+1][0]))+ '\n')    #Zapisanie danych do porownania do pliku wynikowego
            duration = 0                                                            #Przypisanie czasu trwania operacji = 0
        
            f = open(str (filename), "r")                                                 #Otwarcie pliku z danymi 
            tablica = f.readlines()                                                 #Wczytanie danych do tablicy 
            cities = int(tablica[0])                                                #Stwozenie listy miast
            matrix = {i:[] for i in range (cities)}                                 #Stwozenie slownika z kosztami przejazdow 
            for i in range (0 , len(tablica)-1):                                    #Wypelnienie slownika kosztami przejazdow
                for j in tablica[i + 1].split():
                   matrix [i].append(int (j))
        
            for j in range (iloscPowt):                                             #Wykonywanie dopoki nie powtorzymy zadana ilosc razy
                result = Brute_force_func(cities, matrix)                           #Wywolanie funkcji Brute_force
                duration += float(result[0])                                        #Zapis czasu wykonanania
                fileResult.write(' '.join(result) + '\n')                           #Zapisanie danych do pliku wynikowego
            fileResult.write('Sredni wynik= '+str(duration/iloscPowt) + '\n')       #Zapisanie sredniego czasu do pliku wynikowego
            fileResult.close()                                                      #Zamkniecie pliku wynikowego
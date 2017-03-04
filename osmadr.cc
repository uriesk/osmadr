#include "libosmadr.h"
#include "overpass.h"
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
  std::string adss = printAddressesInCommun("Weitersfelden");
  //std::cout << adss << std::endl;
  AddressData test1(adss);
  //std::cout << getIDFromOpenDataCSV("Unterweißenbach", "/home/andreas/Documents/Adresse_Relationale_Tabellen-Stichtagsdaten/GEMEINDE.csv") << std::endl;
  //std::cout << getIDFromOpenDataCSV("Ebenort", "/home/andreas/Documents/Adresse_Relationale_Tabellen-Stichtagsdaten/STRASSE.csv") << std::endl;
  //std::cout << getNameFromOpenDataCSV("082779", "/home/andreas/Documents/Adresse_Relationale_Tabellen-Stichtagsdaten/STRASSE.csv") << std::endl;
  std::string adsb = printAddressesInCommunOpendata("Weitersfelden", "/home/andreas/Documents/Adresse_Relationale_Tabellen-Stichtagsdaten/GEMEINDE.csv", "/home/andreas/Documents/Adresse_Relationale_Tabellen-Stichtagsdaten/STRASSE.csv", "/home/andreas/Documents/Adresse_Relationale_Tabellen-Stichtagsdaten/ADRESSE.csv");
  //std::cout << adsb << std::endl;
  AddressData test2(adsb);
  
  printDifferences(test2, test1, 1);
  //std::cout << adss << std::endl;
  //std::string buildings = printBuildingsInCommun("Unterweißenbach");
  //std::cout << buildings << std::endl;
  //OSMData test = buildings; 
  return 0;
}

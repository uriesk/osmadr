#include "libosmadr.h"
#include "overpass.h"
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
  std::string adss = printAddressesInCommun("Unterweißenbach");
  std::cout << adss << std::endl;
  //std::string buildings = printBuildingsInCommun("Unterweißenbach");
  //std::cout << buildings << std::endl;
  //OSMData test = buildings; 
  return 0;
}

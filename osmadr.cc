#include "libosmadr.h"
#include "overpass.h"
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
  std::vector<OSMObject*> test;
  Way testnode;
  test.push_back(&testnode);
  std::string buildings = printBuildingsInCommun("Unterweißenbach");
  //std::cout << buildings << std::endl;
  loadOSMData(buildings); 
  return 1;
}

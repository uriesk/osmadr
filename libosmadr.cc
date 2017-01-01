#include <iostream>
#include <string>
#include "libosmadr.h"
#include "overpass.h"

double mean(double a, double b)
{
  return (a+b) / 2;
}

std::string printAddressesInCommun(const char* name)
{
  std::string request = (std::string)OVERPASS_ADDRESS_REQUEST;
  return overpassRequest(OVERPASS_HOST, OVERPASS_URL, request);
}

std::string printBuildingsInCommun(const char* name)
{
  std::string request = (std::string)OVERPASS_BUILDING_REQUEST;
  std::cout << request << std::endl;
  return overpassRequest(OVERPASS_HOST, OVERPASS_URL, request);
}

OSMData::OSMData(std::string osm_data)
{
  std::cout << "Construct OSMData" << std::endl;
  unsigned int strcnt;
  strcnt = 0;

  while((strcnt = loadXMLElement(osm_data, osm_vector, strcnt, 0)));

  for(std::vector<OSMObject*>::iterator iter=osm_vector.begin(); iter != osm_vector.end(); iter++)
    (*iter)->refResolution(osm_vector);

  for(std::vector<OSMObject*>::iterator iter=osm_vector.begin(); iter != osm_vector.end(); iter++)
  {
    (*iter)->whatAreYou();
    (*iter)->printID();
  }
}

OSMData::~OSMData()
{
  for(std::vector<OSMObject*>::iterator iter=osm_vector.begin(); iter != osm_vector.end(); iter++)
    delete *iter;
  osm_vector.clear();
}

void Relation::refResolution(std::vector<OSMObject*> osm_vector)
{
  members.clear();
  std::vector<OSMObject*>::iterator itero;

  for(std::vector<unsigned long long>::iterator iter=refs.begin(); iter != refs.end(); iter++)
    for(itero=osm_vector.begin(); (*itero)->id == *iter; itero++);
  
  members.push_back(*itero);
}

void Way::refResolution(std::vector<OSMObject*> osm_vector)
{
  nodes.clear();
  std::vector<OSMObject*>::iterator itero;

  for(std::vector<unsigned long long>::iterator iter=refs.begin(); iter != refs.end(); iter++)
    for(itero=osm_vector.begin(); (*itero)->id == *iter; itero++);
  
  nodes.push_back((Node*)(*itero));
}

unsigned int loadXMLElement(std::string osm_data, std::vector<OSMObject*> &osm_vector, unsigned int strcnt = 0, unsigned int level = 0)
{
  std::cout << std::endl << "Enter loadXMLElement with strcnt = "<< strcnt << " on Level = " << level << std::endl;
  unsigned int a;
  unsigned int b;
  unsigned int aa;
  unsigned int bb;
  unsigned int cc;
  std::string element;
  std::string key;
  std::string value;
  a = osm_data.find('<', strcnt) + 1;
  //ignore <? tags
  if( osm_data[a] == '?' )
    return osm_data.find('>', strcnt) + 1;
  //check if end of string is reached (no more < found)
  if( a == std::string::npos + 1)
  {
    std::cout << "Reached End!" << std::endl;
    return 0;
  }
  //get name of element
  b = osm_data.find_first_of(" /><", a);
  element = osm_data.substr(a, b - a);
  std::cout << "ELE: " << element << std::endl;

  //interpret different element types
  if(!(element.compare("way")))
  {
    std::cout << "Its a Way!\n";
    osm_vector.push_back(new Way);
  }
  else if(!(element.compare("node")))
  {
    std::cout << "Its a Node!\n";
    osm_vector.push_back(new Node);
  }
  else if(!(element.compare("relation")))
  {
    std::cout << "Its a Relation!\n";
    osm_vector.push_back(new Relation);
  }
  else if(!(element.compare("tag")))
  {
    std::cout << "Its a Tag!\n";
    (osm_vector.back()->tags).push_back(new Tag);
  }
  else if(!(element.compare("nd")))
  {
    //just one ref tag
    std::cout << "Its a nd!\n";
  }
  else if(!(element.compare("member")))
  {
    //type, ref and role
    std::cout << "Its a Member!\n";
  }


  //read XML Attributes in start-tag
  strcnt = b;
  while(true)
  {
    aa = osm_data.find_first_not_of(" \n\"", strcnt);
    //check if end of start-tag is reached
    if( osm_data[aa] == '/' || osm_data[aa] == '>')
    {
      strcnt = aa;
      break;
    }
    bb = osm_data.find('=', strcnt);
    //check for strange behaviour
    if( bb > osm_data.find(' ', aa + 1))
    {
      std::cout << "Failure\n";
      break; //failure
    }
    //strip " from value
    if(osm_data[bb + 1] == '"')
    {
      cc = osm_data.find('"', bb + 2);
      value = osm_data.substr(bb + 2, cc - bb - 2);
    }
    else
    {
      cc = osm_data.find_first_of(" /><\n", bb);
      value = osm_data.substr(bb + 1, cc - bb - 1);
    }
    key = osm_data.substr(aa, bb - aa);
    //std::cout << "end: " << end_of_starttag << "aa, bb, cc " << aa << " " << bb << " " << cc << std::endl;
    std::cout << "Key: " << key << " Value: " << value << std::endl;

    //interpret by key
    if(!(key.compare("id")))
      osm_vector.back()->id = atoi(value.c_str());
    else if(!(key.compare("lon")))
      ((Node*)osm_vector.back())->lon = ::atof(value.c_str());
    else if(!(key.compare("lat")))
      ((Node*)osm_vector.back())->lat = ::atof(value.c_str());
    else if(!(key.compare("k")))
      (osm_vector.back()->tags).back()->name = value;
    else if(!(key.compare("v")))
      (osm_vector.back()->tags).back()->value = value;
    else if(!(key.compare("ref")))
      {if(osm_vector.back()->addRef(atoi(value.c_str()))) std::cout << "Failure\n";}
    else if(!(key.compare("type")))
      {if(osm_vector.back()->addType(value)) std::cout << "Failure\n";}
    else if(!(key.compare("role")))
      {if(osm_vector.back()->addRole(value)) std::cout << "Failure\n";}

    strcnt = cc;
  }


  //std::cout << "Wenn " << osm_data[strcnt] << " und " << osm_data[strcnt + 1] << std::endl;
  if( osm_data[strcnt] == '/' && osm_data[strcnt + 1] == '>')
    return strcnt;

  unsigned int end_of_element = osm_data.find((std::string)"</" + element + ">", strcnt);
  while(osm_data.find('<', strcnt) != end_of_element)
  {
    //Yes, it's recursive
    if(!(strcnt = loadXMLElement(osm_data, osm_vector, strcnt, level + 1)))
    {
      //failure if end of string is reached before every tag is closed
      std::cout << "Failure\n";
      return 0;
    }
  }

  return end_of_element + 2 + element.length();
}


int Way::addRef(unsigned long long ref)
{
  refs.push_back(ref);
  return 0;
}

int Relation::addRef(unsigned long long ref)
{
  refs.push_back(ref);
  return 0;
}

int Relation::addType(std::string type)
{
  types.push_back(type);
  return 0;
}

int Relation::addRole(std::string role)
{
  roles.push_back(role);
  return 0;
}

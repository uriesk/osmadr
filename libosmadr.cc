#include <iostream>
#include <fstream>
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

std::string getIDFromOpenDataCSV(const char* name, const char* csv_file)
{
  //CSV in the form "ID";..;"Elementname";..
  std::string line;
  std::string search_string = (std::string)"\"" + name + "\"";
  std::ifstream csv(csv_file);
  if(csv.is_open())
  {
    while(getline(csv,line))
    {
      if(line.find(search_string) != std::string::npos)
      {
        unsigned int length = line.find_first_of('\"',1);
        if(length == std::string::npos)
          continue;
        csv.close();
        return line.substr(1,length - 1);
      }
    }
    csv.close();
  }
  return "";
}

std::string getNameFromOpenDataCSV(const char* id, const char* csv_file, unsigned int pos)
{
  //ID has to be the first Element, Seperator is '\;', Value and ID have to be in quotes
  std::string line;
  std::string search_string = (std::string)"\"" + id + "\"";
  std::ifstream csv(csv_file);
  if(csv.is_open())
  {
    while(getline(csv,line))
    {
      if(line.find(search_string) == 0)
      {
        csv.close();
        return printElementFromLine(line, pos, ';');
      }
    }
    csv.close();
  }
  return "";
}

// Prints Element Nr.#pos from csv-line
std::string printElementFromLine(std::string line, unsigned int pos, const char seperator)
{
  unsigned int position_begin = 0;
  unsigned int position_end = 0;
  for(unsigned int cnt = 0; cnt < pos; cnt++)
  {
    position_begin = line.find_first_of(seperator, position_begin);
    if(position_begin == std::string::npos)
      return "";
    position_begin++;
  }
  position_end = line.find_first_of(seperator, position_begin);
  if(position_end == std::string::npos)
    return "";
  if(line[position_begin] == '\"')
    return line.substr(position_begin + 1, position_end - position_begin - 2);
  else
    return line.substr(position_begin, position_end - position_begin);
}

std::string printAddressesInCommunOpendata(const char* name, const char* gemeinde_csv,
                                               const char* strasse_csv, const char* addresse_csv)
{
  std::string id = getIDFromOpenDataCSV(name, gemeinde_csv);
  if(id == "")
    return "";

  std::string return_str = "";
  std::string line;
  std::string search_string = (std::string)"\"" + id + "\"";
  std::ifstream csv(addresse_csv);
  if(csv.is_open())
  {
    while(getline(csv,line))
    {
      if(line.find(search_string) != std::string::npos)
      {
        std::string street_id = printElementFromLine(line, 4);
        std::string street_name = getNameFromOpenDataCSV(street_id.c_str(), strasse_csv);
        return_str.append((std::string)name + '|');
        return_str.append(printElementFromLine(line, 3) + '|');
        return_str.append(street_name + '|');
        return_str.append(printElementFromLine(line, 7));
        return_str.append(printElementFromLine(line, 8) + '|');
        return_str.append(printElementFromLine(line, 15) + '|');
        return_str.append(printElementFromLine(line, 16) + '\n');
      }
    }
    csv.close();
  }
  return return_str;
}

std::string printBuildingsInCommun(const char* name)
{
  std::string request = (std::string)OVERPASS_BUILDING_REQUEST;
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

Address::Address(std::string ci, std::string plz, std::string st, std::string hn)
{
  city = ci;
  postcode = plz;
  street = st;
  housenumber = hn;
}

void AddressData::add(std::string ci, std::string plz, std::string st, std::string hn)
{
  Address addr(ci, plz, st, hn);
  addresses.push_back(addr);
}

AddressData::AddressData(std::string csv_data, const char seperator)
{
  unsigned int seperator_positions[7];
  unsigned int cnt;
  unsigned int charcnt;
  charcnt = 0;

  while(true)
  {
    seperator_positions[0] = charcnt;
    cnt = 1;
    for(; csv_data[charcnt] != '\n' && charcnt < csv_data.length(); charcnt++)
    {
      if(csv_data[charcnt] == seperator)
      {
        if(cnt < 6)
          seperator_positions[cnt] = charcnt;
        cnt++;
      }
    }
    if(cnt >= 4)
    {
      seperator_positions[cnt] = charcnt;
      //create Address
      this->add(csv_data.substr(seperator_positions[0],seperator_positions[1]-seperator_positions[0]),
                csv_data.substr(seperator_positions[1]+1,seperator_positions[2]-seperator_positions[1]-1),
                csv_data.substr(seperator_positions[2]+1,seperator_positions[3]-seperator_positions[2]-1),
                csv_data.substr(seperator_positions[3]+1,seperator_positions[4]-seperator_positions[3]-1));
    }
    else 
      std::cout << "Failure\n";
    if(cnt >= 5)
    {
      this->addresses.back().lat = ::atof(csv_data.substr(seperator_positions[4]+1,seperator_positions[5]-seperator_positions[4]-1).c_str());
      this->addresses.back().lon = ::atof(csv_data.substr(seperator_positions[5]+1,seperator_positions[6]-seperator_positions[5]-1).c_str());
    }
    if(charcnt >= csv_data.length()-1)
      break;
    charcnt++;
  }
}

void AddressData::print()
{
  for(std::vector<Address>::iterator iter=addresses.begin(); iter != addresses.end(); iter++)
    iter->print();
}

void Address::print()
{
  char seperator = ' ';
  std::cout << city << seperator;
  std::cout << postcode << seperator;
  std::cout << street << seperator;
  std::cout << housenumber;
  if(lat != 0 && lon != 0)
  {
    std::cout << seperator << std::fixed << std::setprecision(2) << lat << seperator;
    std::cout << std::fixed << std::setprecision(2) << lon << seperator;
  }
  std::cout << std::endl;
}

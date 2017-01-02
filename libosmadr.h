#include <vector>
#include <string>
#include <iostream>

#define OVERPASS_HOST "overpass-api.de"
#define OVERPASS_URL "/api/interpreter?data="

#define OVERPASS_ADDRESS_REQUEST "[out:csv(\"addr:city\", \"addr:postcode\", \"addr:street\", \"addr:housenumber\";false ;\"|\")]; \
( area[name=\"" + name  + "\"][boundary=\"administrative\"]; )->.a; \
( \
  node \
    [\"addr:housenumber\"] \
    (area.a); \
  way \
    [\"addr:housenumber\"] \
    (area.a); \
  rel \
    [\"addr:housenumber\"] \
    (area.a); \
); \
(._;); \
out tags;"

#define OVERPASS_BUILDING_REQUEST " \
( area[name=\"" + name + "\"][boundary=\"administrative\"]; )->.a; \
( \
  way \
    [\"building\"] \
    [!\"addr:housenumber\"] \
    [!\"addr:street\"] \
    (area.a); \
  rel \
    [\"building\"] \
    [!\"addr:housenumber\"] \
    [!\"addr:street\"] \
    (area.a); \
); \
(._;>;); \
out;"

double mean(double, double);
std::string printAddressesInCommun(const char* name);
std::string printBuildingsInCommun(const char* name);

typedef struct _Tag_
{
  std::string name;
  std::string value;
} Tag;

class OSMObject
{
  public:
    virtual ~OSMObject(){};
    std::vector<Tag*> tags;
    unsigned long long id;
    virtual void whatAreYou(){std::cout<<"OSMObject\n";}
    virtual int addRef(unsigned long long){return 1;}
    virtual int addType(std::string){return 1;}
    virtual int addRole(std::string){return 1;}
    virtual void refResolution(std::vector<OSMObject*> osm_vector){};
    void printID(){std::cout<<id<<std::endl;}
};

class OSMData
{
  public:
    OSMData(std::string xmldata);
    ~OSMData();
  private:
    std::vector<OSMObject*> osm_vector;
};

class Node : public OSMObject
{
  public:
    long double lat;
    long double lon;
    std::string lat_string;
    std::string lon_string;
    void whatAreYou(){std::cout<<"Node\n";}
};

class Way : public OSMObject
{
  public:
    std::vector<unsigned long long> refs;
    std::vector<Node*> nodes;
    void refResolution(std::vector<OSMObject*> osm_vector);
    void whatAreYou(){std::cout<<"Way\n";}

    int addRef(unsigned long long ref);
};

typedef struct _Member_
{
  std::string role;
  unsigned long long ref;
  OSMObject* object; 
} Member;

class Relation : public OSMObject
{
  public:
    //std::vector<Member> members;
    std::vector<OSMObject*> members;
    std::vector<unsigned long long> refs;
    std::vector<std::string> types;
    std::vector<std::string> roles;
    void refResolution(std::vector<OSMObject*> osm_vector);
    void whatAreYou(){std::cout<<"Relation\n";}

    int addRef(unsigned long long ref);
    int addType(std::string type);
    int addRole(std::string role);
};

class Address
{
  public:
    Address(std::string, std::string, std::string, std::string);
    long double lat;
    long double lon;
    void print();
  private:
    std::string city;
    std::string postcode;
    std::string street;
    std::string housenumber;
};

class AddressData
{
  public:
    std::vector<Address> addresses;
    AddressData(std::string csv_data, const char seperator = '|');
    void add(std::string, std::string, std::string, std::string);
    void print();
};

unsigned int loadXMLElement(std::string osm_data, std::vector<OSMObject*>&, unsigned int, unsigned int);


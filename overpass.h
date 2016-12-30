#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
//for url encoding
#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
//crap
#include <iostream>
int create_tcp_socket();
char *get_ip(const char *host);
std::string build_get_query(const char *host, const char *page, std::string data);
std::string overpassRequest(const char* overpass_host, const char* overpass_url, std::string overpass_data);
std::string url_encode(const std::string &value);

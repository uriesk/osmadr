#include "overpass.h"

#define HOST "coding.debuntu.org"
#define PAGE "/"
#define PORT 80
#define USERAGENT "HTMLGET 1.0"

std::string overpassRequest(const char* overpass_host, const char* overpass_url, std::string overpass_request)
{
  struct sockaddr_in *remote;
  int sock;
  int tmpres;
  char *ip;
  const char *get;
  char buf[BUFSIZ+1];
  std::string answer("");
  
  sock = create_tcp_socket();
  ip = get_ip(overpass_host);
  fprintf(stderr, "IP is %s\n", ip); 
  remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
  remote->sin_family = AF_INET;
  tmpres = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
  if( tmpres < 0)  
  {
    perror("Can't set remote->sin_addr.s_addr");
    exit(1);
  }else if(tmpres == 0)
  {
    fprintf(stderr, "%s is not a valid IP address\n", ip);
    exit(1);
  }
  remote->sin_port = htons(PORT);

  if(connect(sock, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0){
    perror("Could not connect");
    exit(1);
  }
  get = build_get_query(overpass_host, overpass_url, overpass_request).c_str();
  fprintf(stderr, "Query is:\n<<START>>\n%s<<END>>\n", get);
  
  //Send the query to the server
  int sent = 0;
  while(sent < strlen(get))
  { 
    tmpres = send(sock, get+sent, strlen(get)-sent, 0);
    if(tmpres == -1){
      perror("Can't send query");
      exit(1);
    }
    sent += tmpres;
  }
  //now it is time to receive the page
  memset(buf, 0, sizeof(buf));
  int htmlstart = 0;
  char * htmlcontent;
  while((tmpres = recv(sock, buf, BUFSIZ, 0)) > 0){
    if(htmlstart == 0)
    {
      /* Under certain conditions this will not work.
      * If the \r\n\r\n part is splitted into two messages
      * it will fail to detect the beginning of HTML content
      */
      htmlcontent = strstr(buf, "\r\n\r\n");
      if(htmlcontent != NULL){
        htmlstart = 1;
        htmlcontent += 4;
      }
    }else{
      htmlcontent = buf;
    }
    if(htmlstart){
      answer += htmlcontent;
      //fprintf(stdout, htmlcontent);
    }
 
    memset(buf, 0, tmpres);
  }
  if(tmpres < 0)
  {
    perror("Error receiving data");
  }
  //free(get);
  free(remote);
  free(ip);
  close(sock);
  return answer;
}


int create_tcp_socket()
{
  int sock;
  if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
    perror("Can't create TCP socket");
    exit(1);
  }
  return sock;
}


char *get_ip(const char *host)
{
  struct hostent *hent;
  int iplen = 15; //XXX.XXX.XXX.XXX
  char *ip = (char *)malloc(iplen+1);
  memset(ip, 0, iplen+1);
  if((hent = gethostbyname(host)) == NULL)
  {
    herror("Can't get IP");
    exit(1);
  }
  if(inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, iplen) == NULL)
  {
    perror("Can't resolve host");
    exit(1);
  }
  return ip;
}

std::string build_get_query(const char *host, const char *page, std::string request)
{
  //char *query;
  //const char *dataenc = url_encode(request).c_str();
  //std::cout << request << std::endl;
  //std::cout << dataenc << "  LENGHT: " << strlen(dataenc) <<std::endl;
  std::string tp2 = (std::string)"GET /" + page + url_encode(request) + " HTTP/1.0\r\nHost: " + host + "\r\nUser-Agent: " + USERAGENT + "\r\n\r\n";
  //const char *tpl = "GET /%s%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
  // -5 is to consider the %s %s %s in tpl and the ending \0
  //query = (char *)malloc(strlen(host)+strlen(page)+strlen(dataenc)+strlen(USERAGENT)+strlen(tpl)-6);
  //sprintf(query, tpl, page, dataenc, host, USERAGENT);
  //std::cout << "QUERY:" << query << std::endl;
  return tp2;
}

std::string url_encode(const std::string &value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char) c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}

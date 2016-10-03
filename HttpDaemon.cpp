#include "HttpDaemon.hpp"

#define PORT                 80
#define MAX_CLIENTS          3

void info_html_generator (const char* url, const char* query, httppage_request_t* http_request);
void join_html_generator (const char* url, const char* query, httppage_request_t* http_request);
void set_temp_html_generator (const char* url, const char* query, httppage_request_t* http_request);
void set_duty_html_generator (const char* url, const char* query, httppage_request_t* http_request);
void file_not_found_generator (const char* url, const char* query, httppage_request_t* http_request);

HTTPPage pages[] = 
{
  HTTPPage("/" , HTTP_MIME_TEXT_HTML, info_html_generator),
  HTTPPage("/join" , HTTP_MIME_JSON, join_html_generator),
  HTTPPage("/set-temp" , HTTP_MIME_TEXT_HTML, set_temp_html_generator),
  HTTPPage("/set-duty" , HTTP_MIME_TEXT_HTML, set_duty_html_generator),
  HTTPPage("/404.html" , HTTP_MIME_TEXT_HTML, file_not_found_generator),
};
 
uint8_t pagecount = sizeof(pages) / sizeof(HTTPPage);
AdafruitHTTPServer httpserver(pagecount);
 
void HttpDaemonClass::setup() 
{
  Serial.println("Adding Pages to HTTP Server");
  httpserver.addPages(pages, pagecount);
 
  Serial.println("Starting HTTP Server...");
  httpserver.begin(PORT, MAX_CLIENTS);
  Serial.print("Started, view at http://");  
  Serial.print(IPAddress(Feather.localIP())); 
  Serial.println("/");
  Serial.println();
}

void info_html_generator (const char* url, const char* query, httppage_request_t* http_request)
{
  (void) url;
  (void) query;
  (void) http_request;
 
  httpserver.print("<b>Bootloader</b> : ");
  httpserver.print( Feather.bootloaderVersion() );
  httpserver.print("<br>");
 
  httpserver.print("<b>WICED SDK</b> : ");
  httpserver.print( Feather.sdkVersion() );
  httpserver.print("<br>");
 
  httpserver.print("<b>FeatherLib</b> : ");
  httpserver.print( Feather.firmwareVersion() );
  httpserver.print("<br>");
 
  httpserver.print("<b>Arduino API</b> : "); 
  httpserver.print( Feather.arduinoVersion() );
  httpserver.print("<br>");
}
 
void join_html_generator (const char* url, const char* query, httppage_request_t* http_request)
{
  (void) url;
  (void) query;
  (void) http_request;
 
  Brewhouse.toJson(httpserver);
}
  
void set_temp_html_generator (const char* url, const char* query, httppage_request_t* http_request)
{
  (void) url;
  (void) query;
  (void) http_request;
 
  
}
  
void set_duty_html_generator (const char* url, const char* query, httppage_request_t* http_request)
{
  (void) url;
  (void) query;
  (void) http_request;
 
  
}
  
void file_not_found_generator (const char* url, const char* query, httppage_request_t* http_request)
{
  (void) url;
  (void) query;
  (void) http_request;
 
  httpserver.print("<html><body>");
  httpserver.print("<h1>Error 404 File Not Found!</h1>");
  httpserver.print("<br>");
  
  httpserver.print("Available pages are:");
  httpserver.print("<br>");
  
  httpserver.print("<ul>");
  for(int i = 0; i < pagecount; i++)
  {
    httpserver.print("<li>");
    httpserver.print(pages[i].url);
    httpserver.print("</li>");
  }
  httpserver.print("</ul>");
  
  httpserver.print("</body></html>");
}

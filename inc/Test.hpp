
#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "./HttpRequest.hpp"
#include "./AllHeaders.hpp"
#include <filesystem>

void httpGet();
void httpPost();
void httpPut();
void httpDelete();
void httpHead();
void httpPatch();
void httpOptions();
void httpConnect();
int chunkedGet();
int chunkedPost();
int serverMain(int argc, char **argv);
int httpMain();

#endif

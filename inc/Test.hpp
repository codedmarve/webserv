
#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <map>
#include "./HttpRequestParser.hpp"

void httpGet();
void testWithCurl();
void testMultipleGet();

#endif

#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <curl/curl.h>
#include "./HttpRequestParser.hpp"

void testMultipleGet();
void testHttpPostRequests();
void testHttpPutRequests();
void testMultipleHttpDelete();
void testMultipleHttpHead();
void testMultipleHttpPatch();
void testMultipleHttpOptions();
void testMultipleHttpConnect();


#endif
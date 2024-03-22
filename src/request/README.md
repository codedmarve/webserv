# HTTP Request Parser
This is a C++ class for parsing HTTP requests. It parses the requestline, headers, and the body of an HTTP request. It supports parsing both regular body content and chunked transfer encoding.

### Features
- Request Line Parsing: Parses the method, URI, and protocol from the request line.
- Header Parsing: Parses the headers from the HTTP request.
- Content-Length Parsing: Handles requests with a Content-Length header to parse the body.
- Chunked Body Parsing: Handles requests with chunked transfer encoding to parse the body.
- URI Validation: Validates the URI components according to RFC standards.
- Method Validation: Validates the HTTP method.
- Header Validation: Checks for valid header formats.
- Regular Body Parsing:  Parses regular body based on Content Length.
- IPv6 Support: Supports parsing of IPv6 addresses.
- Protocol Validation: Validates the HTTP protocol version.
- Print Parsed Request: Utility function to print the parsed HTTP request for debugging.
### Usage

#### Initialization
```bash
    #include "HttpRequestParser.hpp"
    HttpRequestParser parser;
```
#### Parsing Request
To parse an HTTP request, call the parseRequest function with the buffer containing the HTTP request:
```bash
    std::string httpRequest = "GET /index.html HTTP/1.1\r\n"
                            "Host: www.example.com\r\n"
                            "Content-Length: 10\r\n"
                            "\r\n"
                            "HelloWorld";

    int status = parser.parseRequest(httpRequest);
```
This will print the request METHOD, URI, PROTOCOL, HEADERS and BODY, solely for for debugging purposes.


The parseRequest function returns an HTTP status code to indicate the parsing status:

200: Successfully parsed the request.
400: Bad Request. The request format is invalid.
401: Unauthorized. Invalid authority in the URI.
402: Bad Request. Invalid host in the URI.
403: Bad Request. Invalid port in the URI.
404: Not Found. Invalid path in the URI.
405: Method Not Allowed. Invalid characters in the query.
406: Not Acceptable. Invalid characters in the fragment.
501: Not Implemented. Unsupported HTTP method.
505: HTTP Version Not Supported.


### HTTP Error code reference
100: Continue

101: Switching Protocols

102: Processing

103: Early Hints

200: OK

201: Created

202: Accepted

203: Non-Authoritative Information

204: No Content

205: Reset Content

206: Partial Content

207: Multi-Status

208: Already Reported

226: IM Used

300: Multiple Choices

301: Moved Permanently

302: Found

303: See Other

304: Not Modified

305: Use Proxy

306: Switch Proxy

307: Temporary Redirect

308: Permanent Redirect

400: Bad Request

401: Unauthorized

402: Payment Required

403: Forbidden

404: Not Found

405: Method Not Allowed

406: Not Acceptable

407: Proxy Authentication Required

408: Request Timeout

409: Conflict

410: Gone

411: Length Required

412: Precondition Failed

413: Payload Too Large

414: URI Too Long

415: Unsupported Media Type

416: Range Not Satisfiable

417: Expectation Failed

418: I'm a teapot

421: Misdirected Request

422: Unprocessable Entity

423: Locked

424: Failed Dependency

425: Too Early

426: Upgrade Required

428: Precondition Required

429: Too Many Requests

431: Request Header Fields Too Large

451: Unavailable For Legal Reasons

500: Internal Server Error

501: Not Implemented

502: Bad Gateway

503: Service Unavailable

504: Gateway Timeout

505: HTTP Version Not Supported

506: Variant Also Negotiates

507: Insufficient Storage

508: Loop Detected

510: Not Extended

511: Network Authentication Required
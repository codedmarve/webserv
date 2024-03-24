1. Language Negotiation:
Purpose: Determine the language of the response based on client preferences.
Headers: Utilizes the Accept-Language header in the request.
Implementation:
Server sends the response in the language preferred by the client.
If the requested language is not available, the server can send a default language or an alternative, if specified (406 Not Acceptable if no match).
Example:
Client sends: Accept-Language: en-US, en;q=0.9, fr;q=0.5
Server responds with Content-Language: en-US if available, else Content-Language: en or a default language.
Function: negotiateLanguage
2. Charset Negotiation:
Purpose: Determine the character encoding of the response.
Headers: Utilizes the Accept-Charset header in the request.
Implementation:
Server sends the response with the charset preferred by the client.
If the requested charset is not available, the server can send a default charset or an alternative, if specified (406 Not Acceptable if no match).
Example:
Client sends: Accept-Charset: utf-8, iso-8859-1;q=0.5
Server responds with Content-Type: text/html; charset=utf-8 if available, else a default charset.
Function: negotiateCharset
3. Content-Type Negotiation:
Purpose: Determine the media type (MIME type) of the response content.
Headers: Utilizes the Accept header in the request.
Implementation:
Server sends the response in the format preferred by the client.
If the requested format is not available, the server can send a default format or an alternative, if specified (406 Not Acceptable if no match).
Example:
Client sends: Accept: application/json, text/html;q=0.9
Server responds with Content-Type: application/json if available, else a default format.
Function: negotiateContentType
4. Content-Encoding Negotiation:
Purpose: Determine if the response should be compressed (e.g., gzip).
Headers: Utilizes the Accept-Encoding header in the request.
Implementation:
Server compresses the response if the client supports compression.
If compression is not supported, the server sends the response uncompressed.
Example:
Client sends: Accept-Encoding: gzip, deflate
Server compresses the response with gzip if supported.
Function: negotiateContentEncoding
5. Cache-Control Negotiation:
Purpose: Control caching behavior of the response.
Headers: Utilizes the Cache-Control header in the request.
Implementation:
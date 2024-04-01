Key: server.location_/[0].allow_methods
Value(s): 
  DELETE
  POST
  GET

Key: server.location_/[0].autoindex
Value(s): 
  off

Key: server.location_/[1].allow_methods
Value(s): 
  DELETE
  POST
  GET

Key: server.location_/[1].autoindex
Value(s): 
  off

Key: server.location_/[2].allow_methods
Value(s): 
  DELETE
  POST
  GET

Key: server.location_/[2].autoindex
Value(s): 
  off

Key: server.location_/cgi[0].autoindex
Value(s): 
  off

Key: server.location_/cgi[0].cgi
Value(s): 
  .sh
  sh

Key: server.location_/cgi[0].client_max_body_size
Value(s): 
  500

Key: server.location_/cgi[0].index
Value(s): 
  index.sh

Key: server.location_/cgi[0].limit_except
Value(s): 
  GET
  HEAD
  POST

Key: server.location_/cgi[0].root
Value(s): 
  webserv_tester/pythonTest/sh

Key: server[0].host
Value(s): 
  127.0.0.1

Key: server[0].index
Value(s): 
  abc/hehe.html

Key: server[0].listen
Value(s): 
  127.0.0.6:8005
  8004
  8005

Key: server[0].root
Value(s): 
  docs/fusion_web
  0

what i want is if key contains location_
save differently like
location:
    path:
        key: value
else use the renameKeysAtIndex function




Index: 0
Renamed Key: <"allow_methods">
Location: <"/">
Value:
  DELETE
  POST
  GET
Renamed Key: <"autoindex">
Location: <"/">
Value:
  off
Renamed Key: <"autoindex">
Location: <"/cgi">
Value:
  off
Renamed Key: <"cgi">
Location: <"/cgi">
Value:
  .sh
  sh
Renamed Key: <"client_max_body_size">
Location: <"/cgi">
Value:
  500
Renamed Key: <"index">
Location: <"/cgi">
Value:
  index.sh
Renamed Key: <"limit_except">
Location: <"/cgi">
Value:
  GET
  HEAD
  POST
Renamed Key: <"root">
Location: <"/cgi">
Value:
  webserv_tester/pythonTest/sh
Renamed Key: <"host">
Location: <"">
Value:
  127.0.0.1
Renamed Key: <"index">
Location: <"">
Value:
  abc/hehe.html
Renamed Key: <"listen">
Location: <"">
Value:
  127.0.0.6:8005
  8004
  8005
Renamed Key: <"root">
Location: <"">
Value:
  docs/fusion_web
  0
Index: 1
Renamed Key: <"allow_methods">
Location: <"/">
Value:
  DELETE
  POST
  GET
Renamed Key: <"autoindex">
Location: <"/">
Value:
  off
Renamed Key: <"index">
Location: <"">
Value:
  abc/hehe.html
Renamed Key: <"root">
Location: <"">
Value:
  docs/fusion_web/
Renamed Key: <"server_name">
Location: <"">
Value:
  www.domain.com
  www.domain2.com
Index: 2
Renamed Key: <"allow_methods">
Location: <"/">
Value:
  DELETE
  POST
  GET
Renamed Key: <"autoindex">
Location: <"/">
Value:
  off
Renamed Key: <"index">
Location: <"">
Value:
  abc/hehe.html
Renamed Key: <"root">
Location: <"">
Value:
  docs/fusion_web2/
Renamed Key: <"server_name">
Location: <"">
Value:
  www.domain.com
  www.domain4.com
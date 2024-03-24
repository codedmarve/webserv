#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <map>
#include <cstring>
#include <zlib.h> // For gzip compression

class Response {
public:
  int GET() {
    // Check if file exists
    if (!file_.exists()) {
      logError("File Not Found: " + file_.getPath());
      return 404; // File Not Found
    }

    std::string status_line = "HTTP/1.1 200 OK\r\n";

    // Set Date header
    headers_["Date"] = getCurrentDateTime();

    // Check if autoindex is enabled and file is a directory
    if (config_.getAutoindex() && file_.is_directory()) {
      headers_["Content-Type"] = "text/html"; // MIME type for HTML
      body_ = file_.autoIndex(config_.getRequestTarget());
    } else {
      // Determine MIME type based on file extension
      std::string mime_type = getMimeType(file_.getMimeExtension());
      if (mime_type.empty()) {
        mime_type = "application/octet-stream"; // Default MIME type
      }
      headers_["Content-Type"] = mime_type;

      body_ = file_.getContent();
    }

    // Set Content-Length header
    headers_["Content-Length"] = std::to_string(body_.length());

    // Content Compression
    if (shouldCompress()) {
      headers_["Content-Encoding"] = "gzip";
      body_ = compressBody(body_);
    }

    // Security Headers
    headers_["X-Content-Type-Options"] = "nosniff";
    headers_["X-Frame-Options"] = "DENY";
    headers_["Content-Security-Policy"] = "default-src 'self'";

    // Construct the full response
    std::ostringstream response_stream;
    response_stream << status_line;
    for (std::map<std::string, std::string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it) {
      response_stream << it->first << ": " << it->second << "\r\n";
    }
    response_stream << "\r\n"; // End of headers
    response_stream << body_;

    // Update the response string
    response_ = response_stream.str();

    return 200;
  }

private:
  std::map<std::string, std::string> headers_;
  std::string body_;
  std::string response_;

  // Assume these are defined somewhere
  File file_;
  Config config_;

  void logError(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
    // Optionally, log to a file or external service
  }

  std::string getCurrentDateTime() {
    char buffer[80];
    std::time_t now = std::time(NULL);
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&now));
    return buffer;
  }

  bool shouldCompress() {
    // Check client capabilities or preferences for compression
    // For simplicity, assume compression is always supported
    return true;
  }

  std::string compressBody(const std::string& input) {
    // Compress the body using gzip
    std::ostringstream compressed;
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
      logError("Failed to initialize zlib deflate.");
      return input; // Return uncompressed body
    }

    zs.next_in = (Bytef*)input.data();
    zs.avail_in = static_cast<uInt>(input.size());

    int ret;
    char outbuffer[32768];
    do {
      zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
      zs.avail_out = sizeof(outbuffer);

      ret = deflate(&zs, Z_FINISH);
      if (ret == Z_OK || ret == Z_STREAM_END) {
        compressed.write(outbuffer, sizeof(outbuffer) - zs.avail_out);
      } else {
        deflateEnd(&zs);
        logError("zlib deflate error.");
        return input; // Return uncompressed body
      }
    } while (zs.avail_out == 0);

    deflateEnd(&zs);

    return compressed.str();
  }
};

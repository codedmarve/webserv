#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <map>
#include <cstring>
#include <zlib.h> // For gzip compression

class Response {
public:
  

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

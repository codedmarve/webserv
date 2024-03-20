#include "../inc/HttpRequestParser.hpp"

// void HttpRequestParser::parseChunkedBody(const std::string &chunkedBody) {
//     size_t pos = 0;
//     while (pos < chunkedBody.size()) {
//         size_t chunkSizePos = chunkedBody.find("\r\n", pos);
//         if (chunkSizePos == std::string::npos)
//             throw std::invalid_argument("Invalid chunked body");

//         std::string chunkSizeStr = chunkedBody.substr(pos, chunkSizePos - pos);
//         size_t chunkSize;
//         std::istringstream iss(chunkSizeStr);
//         if (!(iss >> std::hex >> chunkSize))
//             throw std::invalid_argument("Invalid chunk size format");

//         pos = chunkSizePos + 2; // Move past "\r\n"

//         if (chunkSize == 0) {
//             // Last chunk
//             break;
//         }

//         size_t nextChunkPos = pos + chunkSize;
//         if (nextChunkPos > chunkedBody.size()) {
//             throw std::invalid_argument("Incomplete chunked body");
//         }

//         // Append this chunk to the body
//         body_ += chunkedBody.substr(pos, chunkSize);

//         pos = nextChunkPos + 2; // Move past chunk data and "\r\n"
//     }
// }



std::string HttpRequestParser::parseBody(const std::string& contentType) {

    if (request_.length() >= length_)
    {
        
    }


    if (isChunked_) {
        // Parse chunked body
        parseChunkedBody(body_);
    }

    // Additional validation or parsing logic based on content type
    if (contentType.find("application/json") != std::string::npos) {
        // JSON body
        validateJson(body_);
        return body_;
    } else if (contentType.find("application/xml") != std::string::npos) {
        // XML body
        validateXml(body_);
        return body_;
    } else if (contentType.find("application/x-www-form-urlencoded") != std::string::npos) {
        // Form data
        validateFormData(body_);
        return body_;
    } else if (contentType.find("text/plain") != std::string::npos) {
        // Plain text body
        validatePlainText(body_);
        return body_;
    } else if (contentType.find("multipart/form-data") != std::string::npos) {
        // Multipart data
        // validateMultipartData(body_);
        return body_;
    } else if (contentType.find("application/octet-stream") != std::string::npos) {
        // Binary data
        validateBinaryData(body_);
        return body_;
    } else {
        // Unsupported content type
        throw std::runtime_error("Unsupported content type: " + contentType);
    }
}

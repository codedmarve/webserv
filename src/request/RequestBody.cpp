#include "../../inc/HttpRequest.hpp"

int HttpRequest::parseBody()
{

    if (req_buffer_.length() >= length_)
    {
        body_.append(req_buffer_, 0, length_);
        req_buffer_.erase(0, length_);
        body_offset_ += req_buffer_.length();

        return (body_.length() == length_) ? 100 : 400;
    }
    return 200;
}

int HttpRequest::parseChunkedBody()
{
    size_t end;

    while ((end = req_buffer_.find("\r\n")) != std::string::npos)
    {
        if (chunk_status_ == CHUNK_SIZE)
        {
            std::string hex = req_buffer_.substr(0, end);
            int parseResult = parseChunkSize(hex);
            if (parseResult != 0)
                return parseResult;
            req_buffer_.erase(0, end + 2); // Remove processed chunk size
            chunk_status_ = CHUNK_BODY;
        }
        else if (chunk_status_ == CHUNK_BODY)
        {
            if (chunk_size_ == 0)
            {
                if (!req_buffer_.empty())
                    return parseChunkTrailer();
                return 100; // Finished parsing chunked body and trailers
            }

            if (req_buffer_.size() >= chunk_size_)
            {
                body_.append(req_buffer_, 0, chunk_size_);
                req_buffer_.erase(0, chunk_size_ + 2);
                chunk_size_ = 0;
                chunk_status_ = CHUNK_SIZE;
            }
            else
            {
                // Chunk size larger than available data in buffer
                body_.append(req_buffer_);
                chunk_size_ -= req_buffer_.size();
                req_buffer_.clear(); // Clear buffer after appending
            }
        }
    }
    return 200; // Incomplete chunk, need more data
}

int HttpRequest::parseChunkSize(const std::string &hex)
{
    // Convert hex chunk size to integer
    std::istringstream hexStream(hex);
    unsigned long tempChunkSize = 0;
    hexStream >> std::hex >> tempChunkSize;
    
    if (hexStream.fail() || hexStream.bad())
        return 400; // Bad Request: Invalid chunk size format

    // Ensure tempChunkSize is within the range of size_t
    if (tempChunkSize > static_cast<unsigned long>(std::numeric_limits<size_t>::max()))
        return 400;
    chunk_size_ = static_cast<size_t>(tempChunkSize);
    return 0;
}

int HttpRequest::parseChunkTrailer()
{
    size_t end;
    std::string line;

    while ((end = req_buffer_.find("\r\n")) != std::string::npos)
    {
        line = req_buffer_.substr(0, end);
        req_buffer_.erase(0, end + 2);

        // Check for an empty line, indicating the end of trailers
        if (line.empty())
        {
            break;
        }
        size_t separatorPos = line.find(':');
        if (separatorPos != std::string::npos)
        {
            std::string key = line.substr(0, separatorPos);
            std::string value = line.substr(separatorPos + 1);

            key = trim(key);
            value = trim(value);
            headers_[key] = value;
        }
        else
        {
            // If no separator found, it's an invalid format
            return 400; // Bad Request: Invalid header format
        }
    }

    return 100;
}

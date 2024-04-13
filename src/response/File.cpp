#include "../../inc/File.hpp"

File::File() : fd_(0) {}

File::File(std::string path) : fd_(0)
{
    set_path(path);
}

File::~File()
{
    // close();
    if (mimes_) {
        delete mimes_;
        mimes_ = NULL;
    }
}

#include <string>

std::string removeDupSlashes(std::string str)
{
    if (str.empty())
        return str;

    std::string::iterator it = str.begin();
    bool lastWasSlash = (*it == '/');

    it++;

    while (it != str.end())
    {
        if (*it == '/')
        {
            if (lastWasSlash)
            {
                it = str.erase(it);
            }
            else
            {
                lastWasSlash = true;
                it++;
            }
        }
        else
        {
            lastWasSlash = false;
            it++;
        }
    }

    if (!str.empty() && str[str.length() - 1] == '/')
        str.erase(str.length() - 1);

    return str;
}

void File::set_path(std::string path, bool negotiation)
{
    path_ = removeDupSlashes(path);

    (negotiation) ? parseExeNegotiation() : parseExe();
}

void File::parseExe()
{
    std::string file = path_.substr(path_.find_last_of("/") + 1);

    if (file.empty())
        return;

    file_name_full_ = file;

    size_t lastDotPos = file.find_last_of(".");
    if (lastDotPos != std::string::npos && lastDotPos != 0)
    {
        mime_ext_ = file.substr(lastDotPos + 1);

        std::transform(mime_ext_.begin(), mime_ext_.end(), mime_ext_.begin(), tolower);

        if (mime_ext_.length() > 15)
            mime_ext_ = mime_ext_.substr(0, 15);

        // Check for non-alphanumeric characters and replace them with "_"
        for (std::string::iterator it = mime_ext_.begin(); it != mime_ext_.end(); ++it)
        {
            if (!isalnum(*it))
                *it = '_';
        }
    }

    // Extract the file name without extension
    file_name_ = (lastDotPos != std::string::npos)
                     ? file.substr(0, lastDotPos)
                     : file;
}

void File::parseExeNegotiation()
{
    std::string file = path_.substr(path_.find_last_of("/") + 1);

    if (file.empty())
        return;

    file_name_full_ = file;

    size_t lastDotPos = file.find_last_of(".");
    mimes_ = new MimeTypes();

    if (lastDotPos != std::string::npos && lastDotPos != 0)
    {
        std::string fileExtension = file.substr(lastDotPos);
        std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), tolower);

        std::string mimeType = mimes_->getType(fileExtension);

        if (mimeType == "application/octet-stream")
        {
            while (lastDotPos != std::string::npos && lastDotPos != 0)
            {
                fileExtension = file.substr(lastDotPos + 1);
                std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), tolower);

                mimeType = mimes_->getType("." + fileExtension);

                if (mimeType != "application/octet-stream")
                    break;

                lastDotPos = file.find_last_of(".", lastDotPos - 1);
            }
        }

        mime_ext_ = mimeType;
    }

    file_name_ = (lastDotPos != std::string::npos)
                     ? file.substr(0, lastDotPos)
                     : file;
}

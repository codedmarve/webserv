#include "../../inc/File.hpp"

File::File() : fd_(0) {}

File::File(std::string path) : fd_(0)
{
    set_path(path);
}

File::~File()
{
    if (fd_ >= 0) {
        closeFile();
        std::cout << "File Closed: " << path_ << std::endl;
    }
    if (mimes_) {
        delete mimes_;
        mimes_ = NULL;
    }
}

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

void File::createFile(std::string &body) {
    ssize_t bytes_written = write(fd_, body.c_str(), body.length());
    if (bytes_written <= 0) {
        std::string errorStr = "write: ";
        errorStr += strerror(errno);
        std::cout << "Error: " << errorStr << std::endl;
    }

    close(fd_);
}



bool File::openFile(bool create) {
  close(fd_);

  int flags = create ? (O_CREAT | O_RDWR | O_TRUNC) : O_RDONLY;
  fd_ = open(path_.c_str(), flags, 0755);

  if (fd_ < 0) {
    std::string errorStr = "open: ";
    errorStr += strerror(errno);
    std::cout << "Error: " << errorStr << std::endl;
    return false;
  }

  return true;
}

void File::closeFile() {
    if (fd_ >= 0) {
        close(fd_);
        fd_ = -1;
        std::cout << "File Closed: " << path_ << std::endl;
    }
}

void File::appendFile(const std::string& body) {
    int flags = O_CREAT | O_WRONLY | O_APPEND;
    fd_ = open(path_.c_str(), flags, 0644);

    if (fd_ < 0) {
        std::cerr << "Error opening file for append: " << strerror(errno) << std::endl;
        return;
    }

    ssize_t bytes_written = write(fd_, body.c_str(), body.length());
    if (bytes_written <= 0) {
        std::cerr << "Error appending to file: " << strerror(errno) << std::endl;
    }

    closeFile();
}

bool File::deleteFile() {
    if (unlink(path_.c_str()) != 0) {
        std::cerr << "Error deleting file: " << strerror(errno) << std::endl;
        return false;
    }
    std::cout << "File deleted: " << path_ << std::endl;
    return true;
}


std::string File::listDirectory(std::string& target) {
    std::string body;
    std::vector<directory_listing> listing = getDirectoryListings(path_);

    body += generateHtmlHeader("Index of " + target);
    body += "<h1>Index of " + target + "</h1><hr><pre>";

    for (size_t i = 0; i < listing.size(); ++i) {
        body += formatListing(listing[i], target);
    }

    body += "</pre><hr>";
    body += generateHtmlFooter();

    return body;
}

std::string File::setWidth(size_t width, const std::string& str) {
    if (str.length() >= width) {
        return str.substr(0, width - 3) + "..>";
    }
    return str + std::string(width - str.length(), ' ');
}

std::string File::formatListing(const directory_listing& listing, const std::string& basePath) {
    std::string formatted;

    formatted += "<a href=\"" + basePath + "/" + listing.name_ + "\">" + listing.name_ + "</a>";
    formatted += setWidth(68 - listing.name_.length(), listing.date_);

    formatted += (listing.is_dir_) ? setWidth(20, "-") : setWidth(20, ftos(listing.size_));

    formatted += "\r\n";

    return formatted;
}

std::vector<directory_listing> File::getDirectoryListings(const std::string& dirPath) {
    std::vector<directory_listing> listings;

    DIR* dir = opendir(dirPath.c_str());
    if (!dir) {
        std::cerr << "Error opening directory: " << strerror(errno) << std::endl;
        return listings;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string fileName = entry->d_name;
        if (fileName == "." || fileName == "..")
            continue; // Skip current and parent directories

        directory_listing listing = createListing(fileName, dirPath);
        listings.push_back(listing);
    }

    closedir(dir);
    return listings;
}

directory_listing File::createListing(const std::string& fileName, const std::string& dirPath) {
    directory_listing listing;
    listing.name_ = fileName;

    std::string filePath = dirPath + "/" + fileName;
    struct stat fileStat;
    if (lstat(filePath.c_str(), &fileStat) != 0) {
        std::cerr << "Error getting file info: " << strerror(errno) << std::endl;
        return listing; // Return empty listing if stat fails
    }

    listing.is_dir_ = S_ISDIR(fileStat.st_mode);
    listing.size_ = fileStat.st_size;

    struct tm* timeinfo = localtime(&fileStat.st_mtime);
    char dateBuf[20];
    strftime(dateBuf, sizeof(dateBuf), "%d-%b-%Y %H:%M", timeinfo);
    listing.date_ = std::string(dateBuf);

    return listing;
}



std::string File::generateHtmlHeader(const std::string& title) {
    std::string header;
    header += "<!DOCTYPE html>\r\n";
    header += "<html>\r\n";
    header += "<head>\r\n";
    header += "<meta charset=\"UTF-8\">\r\n";
    header += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n";
    header += "<title>" + title + "</title>\r\n";
    header += "</head>\r\n";
    header += "<body>\r\n";
    return header;
}


std::string File::generateHtmlFooter() {
    std::string footer;
    footer += "</body>\r\n";
    footer += "</html>\r\n";
    return footer;
}

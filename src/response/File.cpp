#include "../../inc/File.hpp"

File::File() : fd_(0) {}

File::File(std::string path) : fd_(0)
{
    set_path(path);
}

File::~File()
{
    if (fd_ >= 0)
    {
        closeFile();
        std::cout << "File Closed: " << path_ << std::endl;
    }
    // if (mimes_)
    // {
    //     delete mimes_;
    //     mimes_ = NULL;
    // }
}


void File::set_path(std::string path, bool negotiation)
{
    path_ = removeDupSlashes(path);

    (negotiation) ? parseExtNegotiation() : parseExt();
}

void File::parseExt()
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

void File::parseExtNegotiation()
{
    std::string file = path_.substr(path_.find_last_of("/") + 1);

    if (file.empty())
        return;

    file_name_full_ = file;

    size_t lastDotPos = file.find_last_of(".");
    // mimes_ = new MimeTypes();

    if (lastDotPos != std::string::npos && lastDotPos != 0)
    {
        std::string fileExtension = file.substr(lastDotPos);
        std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), tolower);

        std::string mimeType = getMimeType(fileExtension);

        if (mimeType == "application/octet-stream")
        {
            while (lastDotPos != std::string::npos && lastDotPos != 0)
            {
                fileExtension = file.substr(lastDotPos + 1);
                std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), tolower);

                mimeType = getMimeType("." + fileExtension);

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

void File::createFile(std::string &body)
{
    ssize_t bytes_written = write(fd_, body.c_str(), body.length());
    if (bytes_written <= 0)
    {
        std::string errorStr = "write: ";
        errorStr += strerror(errno);
        std::cout << "Error: " << errorStr << std::endl;
    }

    close(fd_);
}

std::string File::getMimeType(const std::string ext)
{
    static MimeTypes mime;
    return mime.getType(ext);
}

std::string File::getStatusCode(int code)
{
    static HttpStatusCodes status_codes;
    return status_codes.getStatusCode(code);
}


bool File::openFile(bool create)
{
    close(fd_);

    int flags = create ? (O_CREAT | O_RDWR | O_TRUNC) : O_RDONLY;
    fd_ = open(path_.c_str(), flags, 0755);

    if (fd_ < 0)
    {
        std::string errorStr = "open: ";
        errorStr += strerror(errno);
        std::cout << "Error: " << errorStr << std::endl;
        return false;
    }

    return true;
}

void File::closeFile()
{
    if (fd_ >= 0)
    {
        close(fd_);
        fd_ = -1;
        std::cout << "File Closed: " << path_ << std::endl;
    }
}

void File::appendFile(const std::string &body)
{
    int flags = O_CREAT | O_WRONLY | O_APPEND;
    fd_ = open(path_.c_str(), flags, 0644);

    if (fd_ < 0)
    {
        std::cerr << "Error opening file for append: " << strerror(errno) << std::endl;
        return;
    }

    ssize_t bytes_written = write(fd_, body.c_str(), body.length());
    if (bytes_written <= 0)
    {
        std::cerr << "Error appending to file: " << strerror(errno) << std::endl;
    }

    closeFile();
}

bool File::deleteFile()
{
    if (unlink(path_.c_str()) != 0)
    {
        std::cerr << "Error deleting file: " << strerror(errno) << std::endl;
        return false;
    }
    std::cout << "File deleted: " << path_ << std::endl;
    return true;
}

bool File::checkFileExists(const std::string &filePath)
{
    struct stat statbuf;
    if (stat(filePath.c_str(), &statbuf) != 0)
    {
        std::cerr << "Error checking existence for " << filePath << ": " << strerror(errno) << std::endl;
        return false;
    }
    return true; // File exists
}

bool File::exists()
{
    return checkFileExists(path_);
}

bool File::exists(const std::string &path)
{
    return checkFileExists(path);
}

bool File::is_directory()
{
    struct stat statbuf;
    if (!checkFileExists(path_))
        return false;

    if (stat(path_.c_str(), &statbuf) != 0)
    {
        std::cerr << "Error getting file info for " << path_ << ": " << strerror(errno) << std::endl;
        return false;
    }
    return S_ISDIR(statbuf.st_mode);
}

std::string File::last_modified()
{
    struct stat fileStat;
    if (stat(path_.c_str(), &fileStat) != 0)
    {
        std::cerr << "File does not exist or stat failed" << std::endl;
        return "Unknown";
    }

    time_t modifiedTime = fileStat.st_mtime;

    return formatHttpDate(modifiedTime);
}

std::string File::listDir(std::string &target)
{
    std::string body;
    std::vector<directory_listing> listing = getDirListings(path_);

    body += genHtmlHeader("Index of " + target);
    body += "<h1>Index of " + target + "</h1><hr><pre>";

    for (size_t i = 0; i < listing.size(); ++i)
    {
        body += formatListing(listing[i], target);
    }

    body += "</pre><hr>";
    body += genHtmlFooter();

    return body;
}

std::string File::setWidth(size_t width, const std::string &str)
{
    if (str.length() >= width)
    {
        return str.substr(0, width - 3) + "..>";
    }
    return str + std::string(width - str.length(), ' ');
}

std::string File::formatListing(const directory_listing &listing, const std::string &basePath)
{
    std::string formatted;

    formatted += "<a href=\"" + basePath + "/" + listing.name_ + "\">" + listing.name_ + "</a>";
    formatted += setWidth(68 - listing.name_.length(), listing.date_);

    formatted += (listing.is_dir_) ? setWidth(20, "-") : setWidth(20, ftos(listing.size_));

    formatted += "\r\n";

    return formatted;
}
bool sortAutoListing(const directory_listing &a, const directory_listing &b)
{
    return a.name_ < b.name_;
}

std::vector<directory_listing> File::getDirListings(const std::string &dirPath)
{
    std::vector<directory_listing> listings;

    DIR *dir = opendir(dirPath.c_str());
    if (!dir)
    {
        std::cerr << "Error opening directory: " << strerror(errno) << std::endl;
        return listings;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string fileName = entry->d_name;
        if (fileName == "." || fileName == "..")
            continue; // Skip current and parent directories

        directory_listing listing = createListing(fileName, dirPath);
        listings.push_back(listing);
    }

    closedir(dir);
    std::sort(listings.begin(), listings.end(), sortAutoListing);
    return listings;
}

directory_listing File::createListing(const std::string &fileName, const std::string &dirPath)
{
    directory_listing listing;
    listing.name_ = fileName;

    std::string filePath = dirPath + "/" + fileName;
    struct stat fileStat;
    if (lstat(filePath.c_str(), &fileStat) != 0)
    {
        std::cerr << "Error getting file info: " << strerror(errno) << std::endl;
        return listing; // Return empty listing if stat fails
    }

    listing.is_dir_ = S_ISDIR(fileStat.st_mode);
    listing.size_ = fileStat.st_size;

    struct tm *timeinfo = localtime(&fileStat.st_mtime);
    char dateBuf[20];
    strftime(dateBuf, sizeof(dateBuf), "%d-%b-%Y %H:%M", timeinfo);
    listing.date_ = std::string(dateBuf);

    return listing;
}

std::string File::genHtmlHeader(const std::string &title)
{
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

std::string File::genHtmlFooter()
{
    std::string footer;
    footer += "</body>\r\n";
    footer += "</html>\r\n";
    return footer;
}

std::string File::find_index(std::vector<std::string> &indexes)
{
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(path_.c_str())))
    {
        while ((ent = readdir(dir)))
        {
            // Check if the current directory entry is an index file
            if (std::find(indexes.begin(), indexes.end(), ent->d_name) != indexes.end())
            {
                // Construct the full path to the index file
                std::string ret = path_ + "/" + std::string(ent->d_name);
                closedir(dir);
                return ret;
            }
        }
        closedir(dir);
    }
    else
    {
        std::cout << "opendir : " << strerror(errno) << " of " << path_ << std::endl;
    }

    return "";
}


void File::findMatchingFiles() {
  DIR *dir;
  struct dirent *ent;

  std::string path = path_.substr(0, path_.find_last_of("/"));
  
  // Clear matches_ before populating it again
  if (!matches_.empty())
    matches_.clear();

  if ((dir = opendir(path.c_str()))) {
    while ((ent = readdir(dir))) {
      std::string name(ent->d_name);
      // Check conditions for matching file names
      if (file_name_full_ != name && name.find(file_name_) != std::string::npos &&
          name.find(mime_ext_) != std::string::npos) {
        matches_.push_back(ent->d_name);
      }
    }
    closedir(dir);
  } else {
    std::cerr << "opendir : " << strerror(errno) << " of " << path_ << std::endl;
  }
}

std::vector<std::string> &File::getMatches() {
  return matches_;
}

std::string File::getContent() {
  std::stringstream final;
  char buf[4096 + 1];
  int ret;

  lseek(fd_, 0, SEEK_SET);
  while ((ret = read(fd_, buf, 4096)) != 0) {
    if (ret == -1) {
      std::cerr << "read : " << strerror(errno) << std::endl;
      return "";
    }
    buf[ret] = '\0';
    final << buf;
  }
  return final.str();
};

int &File::getFd() {
  return fd_;
}

std::string &File::getMimeExt() {
  return mime_ext_;
}


std::string &File::getFilePath() {
  return path_;
}

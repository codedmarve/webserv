#include "../../inc/File.hpp"

File::File() : fd_(0) {}

File::File(std::string path) : fd_(0)
{
    set_path(path);
}

File::~File()
{
    closeFile();
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
        mime_ext_ = file.substr(lastDotPos);

        std::transform(mime_ext_.begin(), mime_ext_.end(), mime_ext_.begin(), tolower);

        if (mime_ext_.length() > 15)
            mime_ext_ = mime_ext_.substr(0, 15);

        // Check for non-alphanumeric characters and replace them with "_"
        // for (std::string::iterator it = mime_ext_.begin(); it != mime_ext_.end(); ++it)
        // {
        //     if (!isalnum(*it))
        //         *it = '_';
        // }
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

    if (lastDotPos != std::string::npos && lastDotPos != 0)
    {
        std::string fileExtension = file.substr(lastDotPos);
        std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), tolower);

        std::string mimeType = getMimeType(fileExtension);

        if (mimeType == "application/octet-stream")
        {
            while (lastDotPos != std::string::npos && lastDotPos != 0)
            {
                fileExtension = file.substr(lastDotPos);
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
    if (fd_ <= 0)
        return;
    std::cout << "Closing file: " << path_ << std::endl;
    close(fd_);
    fd_ = -1;
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

bool File::is_file()
{
    struct stat fileStat;
    if (stat(path_.c_str(), &fileStat) != 0)
    {
        std::cerr << "Error getting file info: " << strerror(errno) << std::endl;
        return false;
    }

    return S_ISREG(fileStat.st_mode);
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
    body += "<div style=\"display: grid; grid-template-columns: 1fr 1fr 1fr; align-items: center; padding: 0px 20px; font-size: 1rem; font-weight:bold;\">";
    body += "<span>Directory</span>";
    body += "<span>Date</span>";
    body += "<span>Size</span>";
    body += "</div><hr>\r\n";

    for (size_t i = 0; i < listing.size(); ++i)
    {
        // std::cout << "Listing: " << listing[i].name_ << std::endl;
        // std::cout << "Is dir: " << listing[i].is_dir_ << std::endl;
        // std::cout << "target: " << target << std::endl;

        body += formatListing(listing[i], target);
    }

    body += "</pre><hr>";
    body += genHtmlFooter();

    return body;
}

std::string File::formatListing(const directory_listing &listing, const std::string &basePath)
{
    std::string formatted;

    std::string link = removeDupSlashes(basePath + "/" + listing.name_);
    formatted += "<div style=\"display: grid; grid-template-columns: 1fr 1fr 1fr; align-items: center; padding: 0px 20px;\">";
    formatted += "<a style=\"font-weight:bold;\" href=\"" + link + "\">" + listing.name_ + "</a>";
    formatted += "<span>" + listing.date_ + "</span>";
    formatted += "<span>" + (listing.is_dir_ ? "-" : ftos(listing.size_)) + "</span>";
    formatted += "</div>\r\n";

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
        // if (fileName == "." || fileName == "..")
        //     continue; // Skip current and parent directories

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
        return listing;
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

void File::print_dir_entry(struct dirent *ent) const
{
    std::cout << "Name: " << ent->d_name << std::endl;
    std::cout << "Inode: " << ent->d_ino << std::endl;
    std::cout << "Type: ";
    if (ent->d_type == DT_REG)
        std::cout << "Regular file" << std::endl;
    else if (ent->d_type == DT_DIR)
        std::cout << "Directory" << std::endl;
    else
        std::cout << "Other" << std::endl;
    std::cout << std::endl;
}

void File::print_file_info(const std::string &file) const
{
    struct stat file_stat;
    const std::string &filename = removeDupSlashes(path_ + file);
    if (stat(filename.c_str(), &file_stat) == 0)
    {
        std::cout << "File: " << filename << std::endl;
        std::cout << "Size: " << file_stat.st_size << " bytes" << std::endl;
        std::cout << "Permissions: " << (file_stat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO)) << std::endl;
        std::cout << "Last modified: " << ctime(&file_stat.st_mtime);
        (S_ISDIR(file_stat.st_mode) ? std::cout << "Type: Directory" : std::cout << "Type: Regular file") << std::endl;
    }
    else
        std::cout << "Failed to get information for " << filename << std::endl;
}

std::string File::find_index(std::vector<std::string> &indexes)
{
    DIR *dir;
    struct dirent *ent;

    dir = opendir(path_.c_str());
    if (dir)
    {
        while ((ent = readdir(dir)))
        {
            // print_dir_entry(ent);
            if (std::find(indexes.begin(), indexes.end(), ent->d_name) != indexes.end())
            {
                std::string ret = "/";
                ret += ent->d_name;
                // print_file_info(ret);
                closedir(dir);
                return ret;
            }
        }
        closedir(dir);
    }
    else
        std::cout << "opendir : " << strerror(errno) << " of " << path_ << std::endl;

    return "";
}

void File::findMatchingFiles()
{
    DIR *dir;
    struct dirent *ent;
    std::string path = path_.substr(0, path_.find_last_of("/"));

    if (!matches_.empty())
        matches_.clear();
    dir = opendir(path.c_str());
    if (dir)
    {
        /// @note: Debugging purposes
        while ((ent = readdir(dir)))
        {
            std::string name(ent->d_name);
            /// @note Debugging purposes
            // std::cout << "Name: " << name << std::endl;
            if (name == file_name_full_)
                matches_.push_back(name);
            else if (name.find(file_name_) != std::string::npos &&
                     name.find(mime_ext_) != std::string::npos)
                matches_.push_back(name);
        }
        closedir(dir);
    }
    else
    {
        std::cerr << "opendir : " << strerror(errno) << " of " << path_ << std::endl;
    }
}

std::vector<std::string> &File::getMatches()
{
    return matches_;
}

std::string File::getContent()
{
    std::ifstream fileStream(path_.c_str(), std::ios::binary);
    if (!fileStream.is_open())
    {
        std::cerr << "Failed to open file: " << path_ << std::endl;
        return "";
    }

    // Get the size of the file for proper buffer allocation
    fileStream.seekg(0, std::ios::end);
    std::streamsize fileSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);

    // Create a buffer to hold the entire file contents
    std::vector<char> buffer(fileSize);

    // Read the entire file into the buffer
    if (!fileStream.read(buffer.data(), fileSize))
    {
        std::cerr << "Error reading file: " << path_ << std::endl;
        return "";
    }

    // Construct a string from the buffer
    std::string content(buffer.begin(), buffer.end());

    return content;
}

int &File::getFd()
{
    return fd_;
}

std::string &File::getMimeExt()
{
    return mime_ext_;
}

std::string &File::getFilePath()
{
    return path_;
}

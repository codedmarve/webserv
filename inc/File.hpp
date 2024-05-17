#ifndef FILE_HPP
#  define FILE_HPP

#include "./AllHeaders.hpp"
#include "./MimeTypes.hpp"

class MimeTypes;

struct directory_listing
{
    bool is_dir_;
    size_t size_;
    std::string name_;
    std::string date_;

    directory_listing() : is_dir_(false){};
};

bool sort_auto_listing(directory_listing i, directory_listing j);


class File
{
public:
    File();
    File(std::string path);
    ~File();

    bool is_directory();
    bool is_file();
    bool openFile(bool create = false);
    bool exists();
    bool exists(const std::string &path);

    int &getFd();

    void parseExt();
    void parseExtNegotiation();
    void closeFile();
    void createFile(const std::string &body);
    void appendFile(const std::string &body);
    void appendFile(const std::string &body, std::string MimeTypes);
    bool deleteFile();
    void set_path(std::string path, bool negotiation = false);
    void findMatchingFiles();
    void print_file_info(const std::string& filename) const;
    void print_dir_entry(struct dirent* ent) const;
    

    std::string last_modified();
    std::string find_index(std::vector<std::string> &indexes);
    std::string listDir(std::string &target);
    std::string &getMimeExt();
    std::string getContent();
    std::string &getFilePath();
    std::string getMimeType(const std::string ext);
    std::string getStatusCode(int code);
    std::vector<std::string> &getMatches();

    std::string genHtmlFooter();
    std::string genHtmlHeader(const std::string& title);
    std::vector<directory_listing> getDirListings(const std::string& dirPath);
    std::string formatListing(const directory_listing& listing, const std::string& basePath);
    std::string setWidth(size_t width, const std::string& str);
    directory_listing createListing(const std::string& fileName, const std::string& dirPath);
    bool checkFileExists(const std::string& filePath);

private:
    int fd_;
    std::string mime_ext_;
    std::string file_name_;
    std::string file_name_full_;
    std::vector<std::string> matches_;
    std::string path_;
};

#endif

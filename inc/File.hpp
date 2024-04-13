#ifndef FILE_HPP
#  define FILE_HPP

#include "./AllHeaders.hpp"
#include "./MimeTypes.hpp"

class MimeTypes;

struct auto_listing
{
    bool is_dir_;
    size_t size_;
    std::string name_;
    std::string date_;

    auto_listing() : is_dir_(false){};
};

bool sort_auto_listing(auto_listing i, auto_listing j);


class File
{
public:
    File();
    File(std::string path);
    ~File();

    bool is_directory();
    bool openFile(bool create = false);
    bool exists();
    bool exists(std::string &path);

    int &getFd();

    void parseExe();
    void parseExeNegotiation();
    void closeFile();
    void createFile(std::string &body);
    void appendFile(const std::string &body);
    bool deleteFile();
    void set_path(std::string path, bool negotiation = false);
    void parse_match();
    

    std::string last_modified();
    std::string find_index(std::vector<std::string> &indexes);
    std::string autoIndex(std::string &target);
    std::string &getMimeExtension();
    std::string getContent();
    std::string &getPath();
    std::vector<std::string> &getMatches();

    std::string generateHtmlFooter();
    std::string generateHtmlHeader(const std::string& title);
    std::vector<auto_listing> getDirectoryListings(const std::string& dirPath);
    std::string formatListing(const auto_listing& listing, const std::string& basePath);
    std::string setWidth(size_t width, const std::string& str);

private:
    int fd_;
    std::string mime_ext_;
    std::string file_name_;
    std::string file_name_full_;
    std::vector<std::string> matches_;
    std::string path_;
    MimeTypes *mimes_;
};

#endif

#include "../../inc/ErrorCodes.hpp"

ErrorCodes::ErrorCodes() {
    errorMap[EACCES] = "Permission denied";
    errorMap[EADDRINUSE] = "Address already in use";
    errorMap[EADDRNOTAVAIL] = "Address not available";
    errorMap[EAFNOSUPPORT] = "Address family not supported";
    errorMap[EAGAIN] = "Resource temporarily unavailable";
    errorMap[EALREADY] = "Operation already in progress";
    errorMap[EBADF] = "Bad file descriptor";
    errorMap[EBUSY] = "Device or resource busy";
    errorMap[ECANCELED] = "Operation canceled";
    errorMap[ECHILD] = "No child processes";
    errorMap[ECONNABORTED] = "Connection aborted";
    errorMap[ECONNREFUSED] = "Connection refused";
    errorMap[ECONNRESET] = "Connection reset by peer";
    errorMap[EDEADLK] = "Resource deadlock avoided";
    errorMap[EDESTADDRREQ] = "Destination address required";
    errorMap[EDOM] = "Mathematics argument out of domain";
    errorMap[EEXIST] = "File exists";
    errorMap[EFAULT] = "Bad address";
    errorMap[EFBIG] = "File too large";
    errorMap[EHOSTUNREACH] = "Host unreachable";
    errorMap[EIDRM] = "Identifier removed";
    errorMap[EILSEQ] = "Illegal byte sequence";
    errorMap[EINPROGRESS] = "Operation in progress";
    errorMap[EINTR] = "Interrupted function call";
    errorMap[EINVAL] = "Invalid argument";
    errorMap[EIO] = "Input/output error";
    errorMap[EISCONN] = "Socket is already connected";
    errorMap[EISDIR] = "Is a directory";
    errorMap[ELOOP] = "Too many levels of symbolic links";
    errorMap[EMFILE] = "Too many open files";
    errorMap[EMLINK] = "Too many links";
    errorMap[EMSGSIZE] = "Message too long";
    errorMap[EMULTIHOP] = "Multihop attempted";
    errorMap[ENAMETOOLONG] = "Filename too long";
    errorMap[ENETDOWN] = "Network is down";
    errorMap[ENETRESET] = "Network dropped connection on reset";
    errorMap[ENETUNREACH] = "Network is unreachable";
    errorMap[ENFILE] = "Too many files open in system";
    errorMap[ENOBUFS] = "No buffer space available";
    errorMap[ENODATA] = "No message is available on the STREAM head read queue";
    errorMap[ENODEV] = "No such device";
    errorMap[ENOENT] = "No such file or directory";
    errorMap[ENOEXEC] = "Executable file format error";
    errorMap[ENOLCK] = "No locks available";
    errorMap[ENOLINK] = "Link has been severed";
    errorMap[ENOMEM] = "Not enough space";
    errorMap[ENOMSG] = "No message of the desired type";
    errorMap[ENOPROTOOPT] = "Protocol not available";
    errorMap[ENOSPC] = "No space left on device";
    errorMap[ENOSR] = "No STREAM resources";
    errorMap[ENOSTR] = "Not a STREAM";
    errorMap[ENOSYS] = "Function not supported";
    errorMap[ENOTCONN] = "The socket is not connected";
    errorMap[ENOTDIR] = "Not a directory";
    errorMap[ENOTEMPTY] = "Directory not empty";
    errorMap[ENOTSOCK] = "Not a socket";
    errorMap[ENOTSUP] = "Operation not supported";
    errorMap[ENOTTY] = "Inappropriate I/O control operation";
    errorMap[ENXIO] = "No such device or address";
    errorMap[EOPNOTSUPP] = "Operation not supported on socket";
    errorMap[EOVERFLOW] = "Value too large to be stored in data type";
    errorMap[EPERM] = "Operation not permitted";
    errorMap[EPIPE] = "Broken pipe";
    errorMap[EPROTO] = "Protocol error";
    errorMap[EPROTONOSUPPORT] = "Protocol not supported";
    errorMap[EPROTOTYPE] = "Protocol wrong type for socket";
    errorMap[ERANGE] = "Result too large";
    errorMap[EROFS] = "Read-only file system";
    errorMap[ESPIPE] = "Invalid seek";
    errorMap[ESRCH] = "No such process";
    errorMap[ESTALE] = "Stale file handle";
    errorMap[ETIME] = "Stream ioctl() timeout";
    errorMap[ETIMEDOUT] = "Connection timed out";
    errorMap[ETXTBSY] = "Text file busy";
    errorMap[EWOULDBLOCK] = "Operation would block";
    errorMap[EXDEV] = "Improper link";
}

std::string ErrorCodes::getErrorMessage(int errorNumber) {
    std::map<int, std::string>::iterator it = errorMap.find(errorNumber);
    if (it != errorMap.end()) {
        return it->second;
    } else {
        return "Unknown error";
    }
}

ErrorCodes::~ErrorCodes() {
    errorMap.clear();
}

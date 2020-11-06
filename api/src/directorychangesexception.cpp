#include "../include/directorychangesexception.h"

#include <cstring>
#include <cstdio>

namespace api {
DirectoryChangesException::DirectoryChangesException(int error)
    : m_error(error) {
    sprintf(m_errorMessage, "The error 0x%08x occured while reading the directory. Description: %s",
            m_error,
            strerror(m_error));
}

const char* DirectoryChangesException::what() const noexcept {
    return m_errorMessage;
}
}

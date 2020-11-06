#ifndef DIRECTORY_CHANGES_EXCEPTION_H
#define DIRECTORY_CHANGES_EXCEPTION_H

#include <exception>

namespace api {
class DirectoryChangesException final : public std::exception {
public:
    explicit DirectoryChangesException(int error);
    ~DirectoryChangesException() override = default;

    const char* what() const noexcept override;
private:
    char m_errorMessage[512] {};
    int m_error{};
};
}
#endif // DIRECTORY_CHANGES_EXCEPTION_H

#ifndef UTILS_H
#define UTILS_H

#include <functional>
#include <string>
#include <codecvt>
#include <iomanip>

namespace utils {
/**
 * @brief Event class - This class is callback.
 */
template <typename... Args>
class Event final {
public:
    Event() = default;

    ~Event() {
        detach();
    }

    /**
     * @brief attachMember - Wrap class member into callback
     * @param inst - instance of class or struct type of T
     */
    template <typename T>
    void attachMember(T *inst, void (T::*func)(Args...)) {
        if(m_event) {
            return;
        }

        //wrap class member function into function pointer
        attachFunction([=](Args... args) {
            (inst->*func)(std::forward<Args>(args)...);
        });
    }

    /**
     * @brief attachMember - Wrap class const member into callback
     * @param inst - instance of class or struct type of T
     */
    template <typename T>
    void attachMember(T *inst, void (T::*func)(Args...) const) {
        if(m_event) {
            return;
        }

        //wrap class member function into function pointer
        attachFunction([=](Args... args) {
            (inst->*func)(std::forward<Args>(args)...);
        });
    }

    /**
     * @brief attachFunction -  Wrap function into callback
     * @param fn - callable gunction
     */
    void attachFunction(std::function<void(Args...)> const& fn) const {
        if(!m_event) {
            m_event = fn;
        }
    }

    void detach() const {
        if(m_event) {
            m_event = nullptr;
        }
    }

     void raise(Args... p) {
         if(!m_event) {
             return;
         }

        m_event(std::forward<Args>(p)...);
    }

private:
    mutable std::function<void(Args...)> m_event {};
};

/**
  * @brief ReplaceAll - replace all substrings in a given string
  * @param str - A source string
  * @param replace - A substring to replace
  * @param with - A string to replace all occurrences of the "replace"
  * @return A modified string
  */
template <typename CharT, typename StringT = std::basic_string<CharT>>
auto ReplaceAll(StringT&& str,
                StringT&& replace,
                StringT&& with) {
    if (!replace.empty()) {
        std::size_t pos = 0;
        while ((pos = str.find(replace, pos)) != std::string::npos) {
            str.replace(pos, replace.length(), with);
            pos += with.length();
        }
    }

    return str;
}

/**
  * @brief ReplaceAll - replace all substrings in a given string
  * @param str - A source string
  * @param replace - A substring to replace
  * @param replaceLength - The length of the "replace" string
  * @param with - A string to replace all occurrences of the "replace"
  * @param withLength - The length of the "width" string
  * @return A modified string
  */
template <typename CharT, typename StringT = std::basic_string<CharT>>
auto ReplaceAll(StringT&& str,
              const CharT* replace, size_t replaceLength,
              const CharT* with, size_t withLength) {
    if (replace != nullptr) {
        std::size_t pos = 0;
        while ((pos = str.find(replace, pos)) != std::string::npos) {
            str.replace(pos, replaceLength, with);
            pos += withLength;
        }
    }

    return str;
}

/**
 * @brief s2ws - Convert a given string into a wide string
 * @param str - A given string
 * @return
 */
inline std::wstring s2ws(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

/**
 * @brief s2ws - Convert a given wide string into a string
 * @param str - A given string
 * @return
 */
inline std::string ws2s(const std::wstring& wstr)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wstr);
}
}

#define UNUSED(x) (void)x;

#endif //UTILS_H

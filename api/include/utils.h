#ifndef UTILS_H
#define UTILS_H

#include <functional>
#include <string>
#include <codecvt>
#include <iomanip>

namespace utils {
/**
 * @brief Event class - реализация события - по сути callback
 * Отдаленно напоминает Qt signal-slots
 */
template <typename... Args>
class Event {
public:
    Event() = default;

    ~Event() {
        detach();
    }

    /**
     * @brief attachMember - обертка метода класса в callback
     * @param inst - экземпляр класса или структуры
     */
    template <typename T>
    void attachMember(T *inst, void (T::*func)(Args...)) {
        if(m_event) {
            return;
        }

        //оборачивание метода в указатель на функцию
        attachFunction([=](Args... args) {
            (inst->*func)(std::forward<Args>(args)...);
        });
    }

    /**
     * @brief attachMember - обертка метода класса в callback (const вариант)
     * @param inst - экземпляр класса или структуры
     */
    template <typename T>
    void attachMember(T *inst, void (T::*func)(Args...) const) {
        if(m_event) {
            return;
        }

        //оборачивание метода в указатель на функцию
        attachFunction([=](Args... args) {
            (inst->*func)(std::forward<Args>(args)...);
        });
    }

    /**
     * @brief attachFunction - обертка функции в callback
     * @param fn - ссылка на функцию
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
  * @brief ReplaceAll - замена всех вхождений в строке
  * @param str - строка для изменений
  * @param replace - подстрока - что менять
  * @param with - на что менять
  * @return измененная строка
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
  * @brief ReplaceAll - замена всех вхождений в строке
  * @param str - строка для изменений
  * @param replace - что менять (С строка)
  * @param replaceLength - длина строки
  * @param with - на что менять
  * @param withLength - длина строки
  * @return измененная строка
  */
template <typename CharT, typename StringT = std::basic_string<CharT>>
auto ReplaceAll(StringT&& str,
              const CharT* replace, const size_t& replaceLength,
              const CharT* with, const size_t& withLength) {
    if (replace != nullptr) {
        std::size_t pos = 0;
        while ((pos = str.find(replace, pos)) != std::string::npos) {
            str.replace(pos, replaceLength, with);
            pos += withLength;
        }
    }

    return str;
}

inline std::wstring s2ws(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

inline std::string ws2s(const std::wstring& wstr)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wstr);
}
}

#define UNUSED(x) (void)x;

#endif //UTILS_H

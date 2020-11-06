#ifndef UTILS_WIN_H
#define UTILS_WIN_H

#include <string>

#include <windows.h>

namespace utils {
/**
 * @brief The FilesFinder class
 * Класс для поиска файлов в текущей директории
 * Поиск осуществляется рекурсивно по всем поддиректориям
 */
class FilesFinder final {
public:
	/**
	 * @brief FilesFinder - ctor
	 * Запускает первый поиск по маске (FindFirstFile)
	 * @param filePath - полный путь к файлу
	 * @param fData - данные искомого файла
	 */
	FilesFinder(const std::wstring& filePath, WIN32_FIND_DATAW& fData) noexcept;
	~FilesFinder();

	/**
	 * @brief findNext - поиск следующего файла по маске
	 * @param fData - данные о файле
	 * @return true - если найден файл, иначе - false
	 */
	bool findNext(WIN32_FIND_DATAW& fData);

	/**
	 * @brief isValid
	 * @return true - если конструктор отработал (FindFirstFile), иначе - false
	 */
	bool isValid();

private:
	HANDLE m_handle {nullptr};
};

/**
 * @brief FileTimeToString - пренобразование структуры FILETIME в строку
 * @param ftime - структура FILETIME
 * @return преобразованную из структуры FILETIME строку со временем
 */
std::string FileTimeToString(const FILETIME& ftime );

/**
 * @brief FileSize - получение размера файла из длинного числа
 * @param fileSizeHigh - старшее слово
 * @param fileSIzeLow - младшее слово
 * @return размер файла
 */
uint64_t FileSize(DWORD fileSizeHigh, DWORD fileSIzeLow);

bool IsDirectory(const std::wstring& path);
bool IsDirectory(const std::string& path);

bool PathExists(const std::wstring& path);
bool PathExists(const std::string& path);
}

#endif // UTILS_WIN_H

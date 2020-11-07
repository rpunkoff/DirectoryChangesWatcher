#ifndef DIRECTORY_CHANGES_TYPE_H
#define DIRECTORY_CHANGES_TYPE_H

namespace api {
/**
 * @brief The DirectoryChangesType enum
 * Platform independent status of change in some file
 */
enum class DirectoryChangesType {
	Unknown = 0,
	Added,
	Removed,
	Modified,
	OldRenamed,
	NewRenamed
};
}
#endif // DIRECTORY_CHANGES_TYPE_H

#include "basetest.h"
#include "testmanager.h"

#include <QDir>
#include <QTextStream>

#include <cstdio>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace testing {
BaseTest::BaseTest(const QString &path, QObject* parent)
    : QObject (parent),
      m_testDir(path){
    TestManager::instance().addTest(static_cast<BaseTest *>(this));
}

void BaseTest::removeTestDir() {
    removeDir(m_testDir);
}

const QString& BaseTest::testDir() const {
    return m_testDir;
}

void BaseTest::addDir(const QString& path) {
    QDir dir(path);
    dir.mkpath(path);
}

void BaseTest::addFile(const QString& path) {
    QFile file(path);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << path;
        file.close();
    }
}

void BaseTest::removeFile(const QString &path)
{
    QFile file(path);
    if(file.exists()) {
        file.remove();
    }
}

void BaseTest::removeDir(const QString &path)
{
    QDir dir(path);
    if(dir.exists()) {
        dir.removeRecursively();
    }
}

void BaseTest::appendDataToFile(const QString &path, const QString &data)
{
    QFile file(path);
    if(file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << data;
        file.close();
    }
}

void BaseTest::renamePath(const QString &oldPath, const QString &newPath)
{
    QFileInfo fInfo(oldPath);
    if(fInfo.exists()) {
//        if(fInfo.isDir()) {
#ifdef Q_OS_WIN
            _wrename(oldPath.toStdWString().c_str(), newPath.toStdWString().c_str());
#else
            rename(oldPath.toStdString().c_str(), newPath.toStdString().c_str());
#endif
//        } else if(fInfo.isFile() || fInfo.isSymLink()) {
//            rename(oldPath.toStdString().c_str(), newPath.toStdString().c_str());
//        }
    }
}
}

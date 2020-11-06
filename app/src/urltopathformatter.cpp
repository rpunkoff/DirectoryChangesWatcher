#include "../include/urltopathformatter.h"

#include <QUrl>

UrlToPathFormatter::UrlToPathFormatter(QObject* parent) :
    QObject(parent) { }

QString UrlToPathFormatter::format(const QString &urlString) {
    const QUrl url(urlString);
        if (url.isLocalFile()) {
            return url.toLocalFile();
        }

        return url.toString();
}

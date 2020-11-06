#ifndef URL_TO_PATH_FORMEATTER_H
#define URL_TO_PATH_FORMEATTER_H

#include <QObject>
#include <QString>

class UrlToPathFormatter : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(UrlToPathFormatter)
public:
    UrlToPathFormatter(QObject* parent = nullptr);
    ~UrlToPathFormatter() override = default;

    Q_INVOKABLE QString format(const QString& urlString);
};

#endif //URL_TO_PATH_FORMEATTER_H

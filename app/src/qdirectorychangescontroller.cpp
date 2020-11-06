#include "../include/qdirectorychangescontroller.h"
#include "../include/qdirectorycontentreader.h"
#include "../include/qdirectorychangeswatcher.h"
#include "../include/fileinfowrapper.h"

#include <QThread>

#include <memory>

class QDirectoryChangesControllerPrivate {
    Q_DECLARE_PUBLIC(QDirectoryChangesController)
public:
    explicit QDirectoryChangesControllerPrivate(QDirectoryChangesController* q);
    ~QDirectoryChangesControllerPrivate();

private:
    QDirectoryChangesController *q_ptr{nullptr};
    std::unique_ptr<QDirectoryContentReader> m_reader;
    std::unique_ptr<QDirectoryChangesWatcher> m_watcher;
    bool m_running{false};
    QThread* m_threadWatcher{nullptr};
    QThread* m_threadReader{nullptr};
};

QDirectoryChangesControllerPrivate::QDirectoryChangesControllerPrivate(QDirectoryChangesController* q) :
    q_ptr(q) { }

QDirectoryChangesControllerPrivate::~QDirectoryChangesControllerPrivate() {
    Q_Q(QDirectoryChangesController);

    q->stop();
}

QDirectoryChangesController::QDirectoryChangesController(QObject* parent) :
    QObject(parent),
    d_ptr(new QDirectoryChangesControllerPrivate(this)) { }

QDirectoryChangesController::~QDirectoryChangesController() {
    delete d_ptr;
}

void QDirectoryChangesController::run(const QString& directory) {
    Q_D(QDirectoryChangesController);

    if(d->m_threadWatcher) {
        return;
    }

    d->m_threadReader = new QThread;
    d->m_threadWatcher = new QThread;

    d->m_reader = std::make_unique<QDirectoryContentReader>(directory);
    d->m_watcher = std::make_unique<QDirectoryChangesWatcher>(directory);

    connect(d->m_reader.get(), &QDirectoryContentReader::notify, this, &QDirectoryChangesController::notify, Qt::BlockingQueuedConnection);
    connect(d->m_watcher.get(), &QDirectoryChangesWatcher::notify, this, &QDirectoryChangesController::notify, Qt::BlockingQueuedConnection);

    connect(d->m_reader.get(), &QDirectoryContentReader::failed, this, &QDirectoryChangesController::failed);
    connect(d->m_watcher.get(), &QDirectoryChangesWatcher::failed, this, &QDirectoryChangesController::failed);

    connect(d->m_reader.get(), &QDirectoryContentReader::finished, this, &QDirectoryChangesController::stopReader);

    connect(d->m_threadReader, &QThread::finished, [tw = d->m_threadWatcher](){
        tw->start();
    });

    connect(d->m_watcher.get(), &QDirectoryChangesWatcher::finished, this, &QDirectoryChangesController::finished);

    connect(d->m_threadReader, &QThread::started, d->m_reader.get(), &QDirectoryContentReader::run);
    connect(d->m_threadWatcher, &QThread::started, d->m_watcher.get(), &QDirectoryChangesWatcher::run);
    connect(d->m_threadReader, &QThread::started, [self = this](){
        emit self->setRunning(true);
    });

    d->m_reader->moveToThread(d->m_threadReader);
    d->m_watcher->moveToThread(d->m_threadWatcher);

    d->m_threadReader->start();
}

void  QDirectoryChangesController::stopReader() {
    Q_D(QDirectoryChangesController);

    if(d->m_threadReader && d->m_threadReader->isRunning()) {
        d->m_threadReader->quit();
        d->m_threadReader->wait();
    }

    delete d->m_threadReader;
    d->m_threadReader = nullptr;
}

void QDirectoryChangesController::stop() {
    Q_D(QDirectoryChangesController);

    if(d->m_watcher) {
        d->m_watcher->stop();
        if(d->m_threadWatcher && d->m_threadWatcher->isRunning()) {
            d->m_threadWatcher->quit();
            d->m_threadWatcher->wait();
            delete d->m_threadWatcher;
            d->m_threadWatcher = nullptr;
        }
        setRunning(false);
    }
}

bool QDirectoryChangesController::running() const {
    Q_D(const QDirectoryChangesController);

    return d->m_running;
}

void QDirectoryChangesController::setRunning(bool running) {
    Q_D(QDirectoryChangesController);

    d->m_running = running;
    emit runningChanged(d->m_running);
}


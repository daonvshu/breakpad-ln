#pragma once

#include <qobject.h>

namespace google_breakpad {
    class ExceptionHandler;
}

typedef void(*LoggingSave)(const QString&);

class CrashListener {
    Q_DISABLE_COPY(CrashListener)

public:
    explicit CrashListener(const QString& accessKey);
    ~CrashListener();

    static QString getCacheDir();

    const QString& getAccessKey() const {
        return accessKey;
    }

    void setLogSaveCallback(LoggingSave callback) {
        loggingSave = callback;
    }

    LoggingSave getLogSaveCallback() const {
        return loggingSave;
    }

private:
    google_breakpad::ExceptionHandler* eh;
    QString accessKey;

    LoggingSave loggingSave;
};
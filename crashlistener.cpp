#include "crashlistener.h"

#include <qdir.h>
#include <qstandardpaths.h>
#include <qprocess.h>
#include <qcoreapplication.h>

#include "handler/exception_handler.h"

#ifdef __GNUC__
bool dumpResultsHandler(const google_breakpad::MinidumpDescriptor& descriptor, void* context, bool succeeded) {
#else
bool dumpResultsHandler(const wchar_t*, const wchar_t* minidump_id, void* context, EXCEPTION_POINTERS*, MDRawAssertionInfo*, bool succeeded) {
#endif
    auto listener = static_cast<CrashListener*>(context);

#ifdef __GNUC__
    QFileInfo fileInfo(descriptor.path());
    auto minidumpFileName = fileInfo.baseName();
#else
    auto minidumpFileName = QString::fromStdWString(minidump_id);
#endif

    if (listener->getLogSaveCallback()) {
        listener->getLogSaveCallback()(CrashListener::getCacheDir() + "/" + minidumpFileName + ".log");
    }

    QStringList args;
    args << CrashListener::getCacheDir();
    args << minidumpFileName;
    args << listener->getAccessKey();
    args << QCoreApplication::applicationName();
    QProcess::startDetached(BUGREPORT_EXECUTABLE, args);

    return succeeded;
}

CrashListener::CrashListener(const QString& accessKey)
    : accessKey(accessKey)
    , loggingSave(nullptr)
{
#ifdef __GNUC__
    eh = new google_breakpad::ExceptionHandler(google_breakpad::MinidumpDescriptor(getCacheDir().toStdString()), nullptr, dumpResultsHandler, this, true, -1);
#else
    eh = new google_breakpad::ExceptionHandler(getCacheDir().toStdWString(), nullptr, dumpResultsHandler, this, true, nullptr);
#endif
}

CrashListener::~CrashListener() {
    delete eh;
}

QString CrashListener::getCacheDir() {
    auto dir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir path(dir);
    if (!path.exists()) {
        path.mkpath(dir);
    }
    return dir;
}
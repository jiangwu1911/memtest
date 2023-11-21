#include "glog/logging.h"
#include "memory/Container.h"
#include "memory/ContainerFactory.h"
#include "utilities/utility.h"
#include <QCoreApplication>
#include <QDir>

using namespace esi;

void test01(ssize_t size) {
    std::shared_ptr<Container<short>> src;
    ContainerFactory::ContainerStreamType stream;

    stream = ContainerFactory::getNextStream();
    src = std::make_shared<Container<short>>(LocationHost, stream, size);

    auto dest = std::make_shared<Container<short>>(LocationGpu, *src);
}

void initGlog(const char *appName) {
    char logFileName[512];
    snprintf(logFileName, sizeof(logFileName), "%s.log", appName);

    google::InitGoogleLogging(logFileName);

    QDir dir;
    dir.mkpath("log");
    google::SetLogDestination(google::ERROR, "log/error_");
    google::SetLogDestination(google::WARNING, "log/warn_");
    google::SetLogDestination(google::INFO, "log/info_");

    FLAGS_alsologtostderr = true;

    DLOG(INFO) << "log init:Debug(INFO) log print";
    LOG(INFO) << "log init:Normal(INFO) log print";
    LOG(WARNING) << "log init:Normal(WARRING) log print";
    LOG(ERROR) << "log init:Normal(ERROR) log print";
}

int main(int argc, char *argv[]) {
    initGlog(argv[0]);

    for (int i = 0; i < 10; i++) {
        double t1 = getCurrentTime();
        test01(102400000);
        double t2 = getCurrentTime();
        DLOG(INFO) << "test01 used " << t2 - t1 << " second.";
    }
}

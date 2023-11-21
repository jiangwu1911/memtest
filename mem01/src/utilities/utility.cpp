// ================================================================================================
//
// If not explicitly stated: Copyright (C) 2016, all rights reserved,
//      Rüdiger Göbl
//		Email r.goebl@tum.de
//      Chair for Computer Aided Medical Procedures
//      Technische Universität München
//      Boltzmannstr. 3, 85748 Garching b. München, Germany
//
// ================================================================================================

#include "utility.h"
#include "glog/logging.h"
#include <QString>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sys/sysinfo.h>

using namespace std;

BEGIN_NAMESPACE_ESI

double getCurrentTime() {
    typedef chrono::high_resolution_clock clock;
    typedef chrono::duration<double> duration;

    clock::time_point curTimePoint = clock::now();
    double currentTime = chrono::duration_cast<duration>(curTimePoint.time_since_epoch()).count();

    return currentTime;
}

std::string timeToString(double dt) {
    time_t t = (time_t)dt;
    struct tm *p;
    p = localtime(&t);

    std::stringstream ss;
    ss << p->tm_year + 1900 << "-" << p->tm_mon + 1 << "-" << p->tm_mday << " " << p->tm_hour << ":" << p->tm_min << ":"
       << p->tm_sec;

    auto ms = dt - t;
    ss << "." << std::setfill('0') << std::setw(3) << (int)(ms * 1000);
    return ss.str();
}

std::string timeToStringForFilename(double dt) {
    time_t t = (time_t)dt;
    struct tm *p;
    p = localtime(&t);

    std::stringstream ss;
    ss << p->tm_year + 1900 << "_" << p->tm_mon + 1 << "_" << p->tm_mday << "_" << p->tm_hour << "_" << p->tm_min << "_"
       << p->tm_sec;
    return ss.str();
}

void busyWait(size_t microseconds) {
    typedef chrono::high_resolution_clock clock;
    typedef chrono::duration<size_t, micro> duration;
    clock::time_point start = clock::now();

    size_t diff = 0;
    do {
        diff = chrono::duration_cast<duration>(clock::now() - start).count();
    } while (diff < microseconds);
}

template <> std::string stringify(std::vector<bool> v) {
    std::string b;
    if (v.size() > 0) {
        b += "[";
        for (bool value : v) {
            b += (value ? "1" : "0");
        }
        b.erase(b.end() - 1, b.end());
        b += "]";
        return b;
    } else {
        return "[]";
    }
}

bool fileExists(const std::string &path) { return std::ifstream(path).good(); }

bool writeImageFile(const std::string &filename, const float *buffer, int size) {
    std::ofstream os;
    os.open(filename, std::ios::out | std::ios::trunc | std::ios::binary);
    if (os.is_open()) {
        os.write((char *)buffer, size);
        os.close();
        return true;
    } else {
        DLOG(ERROR) << "Cannot open file for write." << std::endl;
        return false;
    }
}

std::string printHex(const char *buff, int buff_len) {
    std::string result = "";
    char *str = (char *)malloc(buff_len * sizeof(char) * 4);

    if (str != NULL) {
        int ret = 0;
        memset(str, 0, buff_len * sizeof(char) * 4);

        for (int i = 0; i < buff_len; i++) {
            ret += sprintf(str + ret, "%02x ", (uint8_t)buff[i]);
            if (i % 16 == 15) {
                ret += sprintf(str + ret, "\n");
            }
        }

        result = str;
        free(str);
    }
    return result;
}

std::string printHexWithAddress(const char *buff, int startAddr, int length) {
    std::string result = "";
    char *str = (char *)malloc(length * sizeof(char) * 4);

    if (str != NULL) {
        int ret = 0;
        memset(str, 0, length * sizeof(char) * 4);

        for (int i = 0; i < length; i++) {
            if (i % 16 == 0) {
                ret += sprintf(str + ret, "%06x    ", startAddr + i);
            }
            ret += sprintf(str + ret, "%02x ", (uint8_t)buff[i]);
            if (i % 16 == 15) {
                ret += sprintf(str + ret, "\n");
            }
        }

        result = str;
        free(str);
    }
    return result;
}

bool writeFile(const std::string &filename, char *buffer, int size) {
    std::ofstream os;
    os.open(filename, std::ios::out | std::ios::app | std::ios::binary);
    if (os.good()) {
        os.write((char *)buffer, size);
        os.close();
        return true;
    } else {
        DLOG(ERROR) << "Cannot open file for write." << std::endl;
        return false;
    }
}

std::string runCmd(const std::string &cmd) {
    FILE *fp = popen(cmd.c_str(), "r");
    if (fp == NULL) {
        LOG(ERROR) << "popen failed. cmd:" << cmd.c_str();
        return ("");
    }

    std::stringstream ss;
    char line[1024];
    while (fgets(line, sizeof(line), fp) != NULL) {
        ss << line;
    }
    fclose(fp);

    std::string s = ss.str();
    boost::trim_right(s);
    return s;
}

// 已使用内存百分比
double usedMemory() {
    struct sysinfo memInfo;
    sysinfo(&memInfo);

    auto totalMemory = memInfo.totalram;
    totalMemory += memInfo.totalswap;
    totalMemory *= memInfo.mem_unit;

    auto totalMemoryUsed = memInfo.totalram - memInfo.freeram;
    totalMemoryUsed += memInfo.totalswap - memInfo.freeswap;
    totalMemoryUsed *= memInfo.mem_unit;

    double percent = (double)totalMemoryUsed / (double)totalMemory * 100.0;
    return percent;
}

// 获取可用内存数量，单位是MB
double availableMemory() {
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    uint64_t mem = memInfo.freeram + memInfo.bufferram + memInfo.freeswap;
    mem = mem / 1048976;
    return mem;
}

// pw缓冲区中每条线存一条记录, 按线数显示会造成混淆，按每500线30帧做个转换
int pwLineNumberToFrameNumber(int lineNum) { return lineNum * 500.0 / 500; }

int pwFrameNumberToLineNumber(int frameNum) { return frameNum * 500.0 / 500; }

END_NAMESPACE_ESI

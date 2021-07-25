#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <butil/logging.h>



class Xlog : public ::logging::LogSink {
private:
    /* data */
public:
    Xlog(const std::string& filename) {
        auto logger = spdlog::basic_logger_mt("xlog", filename);
        spdlog::set_default_logger(logger);
        spdlog::flush_every(std::chrono::seconds(1));
    }

    bool OnLogMessage(int severity, const char* file, int line,
                 const butil::StringPiece& log_content) override {
                     std::stringstream oss;
                     oss << "[" <<file << ":" << line << "]";
                     oss << log_content.data();
                         spdlog::info(oss.str());
                         return true;
    }
};

bool initXlog(const std::string& filename) {
    ::logging::LogSink* new_sink = new Xlog(filename);
    ::logging::LogSink* old_sink = ::logging::SetLogSink(new_sink);
    if (old_sink) {
        delete old_sink;
    }
    return true;
}

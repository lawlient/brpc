#pragma once


#include "thirdparty/asynclog/async_logger.h"

#include <butil/logging.h>

namespace basis {
namespace asynclog {




class AsyncLog : public ::logging::LogSink {
public:
    AsyncLog() = default;
    ~AsyncLog();

    bool Init();


    static AsyncLog* GetInstance();

    bool OnLogMessage(int severity, const char* file, int line,
                 const butil::StringPiece& log_content) override;

private:
    Log* m_logger;
};


} // namespace asynclog
} // namespace basis
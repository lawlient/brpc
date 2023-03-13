#include "asynclog.h"

#include "config/config.h"


#include <butil/memory/singleton.h>


namespace basis {
namespace asynclog {

static inline Severity asyncloglevel(int s) {
    switch (s) {
    case logging::BLOG_INFO: return Info;
    case logging::BLOG_WARNING: return Warning;
    case logging::BLOG_ERROR: return Error;
    default: return Info;
    }
    return Info;
}


AsyncLog::~AsyncLog() { if (m_logger) free(m_logger); }

AsyncLog* AsyncLog::GetInstance() { return Singleton<AsyncLog>::get(); }

bool AsyncLog::Init() {
    if (m_logger) return true;

    const auto& cfg = text_config::TextConfig::GetInstance()->cfg();
    const auto& log = cfg.log();
    Severity s = static_cast<Severity>(log.level());
    m_logger = Log_new(s, log.id(), log.max() * 1024 * 1024, log.prefix().c_str());
    if (m_logger == nullptr) {
        LOG(ERROR) << "init async log failed";
        return false;
    }

    log_err_t lerr = Log_register(m_logger);
    if (lerr) {
        LOG(ERROR) << "register async log failed";
        return false;
    }

    ::logging::LogSink* oldsink = ::logging::SetLogSink(this);
    if (oldsink) delete oldsink;
    LOG(INFO) << "register asynclog successfully";
    return true;
}


bool AsyncLog::OnLogMessage(int severity, const char* file, int line,
                const butil::StringPiece& log_content) {
    log_err_t lerr = Log_log(m_logger, asyncloglevel(severity), file, line, nullptr, "%s", log_content.data());
    return lerr == 0;
}


} // namespace asynclog
} // namespace basis
#include "config.h"

  
#include <brpc/server.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <string>

namespace basis {
namespace text_config {


#define TEXT_FLIENAME "../conf/default.conf"


TextConfig* TextConfig::GetInstance() { 
    static TextConfig ginstance;
    return &ginstance;
}

bool TextConfig::Init() {
    int fd = ::open(TEXT_FLIENAME, O_RDONLY);
    google::protobuf::io::FileInputStream* fin = new google::protobuf::io::FileInputStream(fd);
    if (!fin) {
        LOG(ERROR) << "open config file:" << TEXT_FLIENAME << " fail";
        return false;
    }

    m_cfg.reset(new Config);

    bool status = google::protobuf::TextFormat::Parse(fin, m_cfg.get());
    delete fin;
    fin = nullptr;
    if (!status) {
        LOG(ERROR) << "parse config file:" << TEXT_FLIENAME << " fail";
        return false;
    }


    return true;
}

const Config& TextConfig::cfg() const {
    static basis::text_config::Config kDefault;
    return m_cfg ? *m_cfg : kDefault;
}


} // namespace text_config
} // namespace basis

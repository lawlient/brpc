#pragma once

#include "text_config.pb.h"


#include <google/protobuf/message.h>


namespace basis {
namespace text_config {




class TextConfig {
public:
    static TextConfig* GetInstance();
    bool Init();

    const Config& cfg() const;

private:
    TextConfig() = default;

private:
    std::shared_ptr<Config> m_cfg;
};





} // namespace text_config
} // namespace basis

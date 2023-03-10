#include "config.h"

#include <google/protobuf/text_format.h>

namespace basis {


#define TEXT_FLIENAME "../conf/default.conf"

TextConfig* TextConfig::GetInstance() {
    static TextConfig ginstance;
    return &ginstance;
}



} // namespace basis

#pragma once

namespace basis {

#define CFG TextConfig::GetInstance();


class TextConfig {
public:
    static TextConfig* GetInstance();
    bool Init() const;

private:
    TextConfig() = default;

private:



};





} // namespace basis

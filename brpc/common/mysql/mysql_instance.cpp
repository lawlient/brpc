#include "mysql_instance.h"



namespace mysql {


MysqlInstance::MysqlInstance() {
    _option.reset(new MysqlOption());
}

MysqlInstance::MysqlInstance(const MysqlOption& opt) {
    _option.reset(new MysqlOption(opt));
}


bool MysqlInstance::Init() {
    if (_pool) return true;

    _factory.reset(new CppConnConnectFactory(*_option));

    _pool.reset(new ConnectPool(_factory.get()));
    return true;
}


Result *MysqlInstance::Execute(const std::string& sql, const google::protobuf::Message *msg) const {
    auto c = _pool->get();
    if (!c) {
        Result *result = new Result();
        result->code = 1;
        result->msg = "connect to db fail";
        return result;
    }

    return c->Execute(sql, msg);
}


} // namespace mysql
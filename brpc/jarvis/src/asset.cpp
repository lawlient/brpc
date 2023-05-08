#include "jarvis.h"
#include "util.h"
#include "redis_client.h"


namespace jarvis {


static std::string table() {
    return financial_asset::GetDescriptor()->name();
}

static void latest_record(::jarvis::financial_records* record) {
    std::unique_ptr<sql::ResultSet> res;
    res.reset(mysql_instance->SelectAll(*record, "", "`id` desc", "", "1"));
    if (!res) return ;

    std::vector<google::protobuf::Message*> msgs;
    mysql_instance->Parse(res.get(), record, msgs);
    for (const auto* msg : msgs) {
        const auto* row = dynamic_cast<const jarvis::financial_records*>(msg);
        record->CopyFrom(*row);
    }
}


bool get_user_balance(int32_t uid, jarvis::financial_asset* asset) {
    std::unique_ptr<sql::ResultSet> res;
    std::string where = "uid = " + std::to_string(uid);
    res.reset(mysql_instance->SelectAll(*asset, where, "`timestamp` desc", "", "1"));
    if (!res) return false;

    std::vector<google::protobuf::Message*> msgs;
    mysql_instance->Parse(res.get(), asset, msgs);

    bool success = false;
    for (const auto* msg : msgs) {
        const auto* row = dynamic_cast<const jarvis::financial_asset*>(msg);
        if (row == nullptr) {
            LOG(ERROR) << "cast financial asset fail";
            continue;
        }
        asset->CopyFrom(*row);
        success = true;
        LOG(DEBUG) << asset->ShortDebugString();
    }
    return success;
}

/* 根据最新一条转账记录刷新资产表
   目前仅个人用来记账，省略了uid检查 */
bool JarvisServiceImpl::update_user_balance() {
    jarvis::financial_records record;
    latest_record(&record);
    if (record.payer() == record.payee()) {
        return true; /* asset is unchanged */
    }

    jarvis::financial_asset asset;
    get_user_balance(kOwnerUid, &asset);


    double amount = asset.amount();
    if (record.payer() == asset.uid()) {
        amount -= record.amount();
    } else if (record.payee() == asset.uid()) {
        amount += record.amount();
    } else {
        return false;
    }

    const std::string nows  = jutil::nowstring();
    std::ostringstream cmd;
    cmd << "insert into " << table() << " ("
           "`timestamp`, "
           "`uid`, "
           "`amount`, "
           "`recordid`, "
           "`create_time`, "
           "`update_time`"
           ") values (";
    cmd << "\"" << record.create_time() << "\", ";
    cmd << asset.uid() << ", ";
    cmd << amount << ", ";
    cmd << record.id() << ", ";
    cmd << "\"" << nows << "\", ";
    cmd << "\"" << nows << "\"";
    cmd << ")";

    bool suc = mysql_instance->Execute(cmd.str());
    return suc;
}


void JarvisServiceImpl::GetFinancialAsset(::google::protobuf::RpcController* controller,
                                          const ::jarvis::HttpRequest* request,
                                          ::jarvis::HttpResponse* response,
                                          ::google::protobuf::Closure* done) {
    brpc::ClosureGuard done_guard(done);

    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);

    if (!check_login()) {
        nlohmann::json res;
        res["status"] = 1;
        res["msg"]    = "Please login";
        cntl->response_attachment().append(res.dump());
        return;
    }
    update_token_ttl();

    ::jarvis::financial_asset asset;
    get_user_balance(kOwnerUid, &asset);

    std::string data;
    google::protobuf::util::JsonPrintOptions joption;
    joption.always_print_primitive_fields = true;
    joption.preserve_proto_field_names = true;
    google::protobuf::util::MessageToJsonString(asset, &data, joption);
    std::ostringstream oss;
    oss << "{\"status\":0, \"msg\":\"Success\", \"data\":[" << data << "]}";
    cntl->response_attachment().append(oss.str());
    LOG(INFO) << cntl->response_attachment();
}





    
} // namespace jarvis

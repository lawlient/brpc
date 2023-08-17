#include "jarvis.h"
#include "util.h"
#include "redis_client.h"


namespace jarvis {


static std::string table() {
    return financial_asset::GetDescriptor()->name();
}

static void latest_record(mysql::MysqlInstance* db, ::jarvis::financial_records* record) {
    mysql::SelectGenerator g(*record);
    std::unique_ptr<mysql::Result> res(db->Execute(g.sql(), record));
    for (const auto* msg : res->records) {
        const auto* row = dynamic_cast<const jarvis::financial_records*>(msg);
        record->CopyFrom(*row);
    }
}


bool get_user_balance(mysql::MysqlInstance* db, int32_t uid, jarvis::financial_asset* asset) {
    mysql::SelectGenerator g(*asset);
    g.where("uid = " + std::to_string(uid));
    g.orderby("timestamp desc");
    g.limit("1");
    std::unique_ptr<mysql::Result> res(db->Execute(g.sql(), asset));

    bool success = false;
    for (const auto* msg : res->records) {
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
    latest_record(_db.get(), &record);
    if (record.payer() == record.payee()) {
        return true; /* asset is unchanged */
    }

    jarvis::financial_asset asset;
    get_user_balance(_db.get(), kOwnerUid, &asset);


    double amount = asset.amount();
    if (record.payer() == asset.uid()) {
        amount -= record.amount();
    } else if (record.payee() == asset.uid()) {
        amount += record.amount();
    } else {
        return false;
    }

    const std::string nows  = basis::util::datetimenow();
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

    std::unique_ptr<mysql::Result> res(_db->Execute(cmd.str()));
    return 1 == res->rows;
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
    get_user_balance(_db.get(), kOwnerUid, &asset);

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

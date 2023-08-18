#include "jarvis.h"
#include "util.h"
#include "redis_client.h"


namespace jarvis {


static inline std::string table() {
    return jarvis::financial_records::GetDescriptor()->name();
}


static void get_financial_record_size(int32_t* total) {
    *total = 0;
    // TODO
}

/* 记录数不断追加，分页展示, 按id倒序，即最新数据在前 */
struct record_condition {
    int page;
    int pagesize;
    int32_t payer;
    int32_t payee;

    std::string limit() const { 
        return std::to_string(page * pagesize) + ", " + std::to_string(pagesize);
    }

    std::string where() const { 
        std::ostringstream where;
        where << "status = 0";
        if (payer >= 0) where << " and `payer` = " << payer;
        if (payee >= 0) where << " and `payee` = " << payee;
        return where.str();
    }
    std::string orderby() const { return "`when` desc"; }
};
static const record_condition kRecordCondition = {
    .page = 0,
    .pagesize = 10,
    .payer = -1,
    .payee = -1,
};
static void financial_all_records(mysql::MysqlInstance *db, ::jarvis::financial_response* response, const record_condition& cond = kRecordCondition) {
    jarvis::financial_records financial;
    mysql::SelectGenerator g(financial);
    g.where(cond.where());
    g.orderby(cond.orderby());
    g.limit(cond.limit());

    std::unique_ptr<mysql::Result> res(db->Execute(g.sql(), &financial));
    for (const auto* msg : res->records) {
        const auto* row = dynamic_cast<const jarvis::financial_records*>(msg);
        if (row == nullptr) {
            LOG(ERROR) << "cast financial records fail";
            continue;
        }
        auto* r = response->add_records();
        r->CopyFrom(*row);
        LOG(DEBUG) << r->ShortDebugString();
    }

    int32_t total = 0;
    get_financial_record_size(&total);
    response->set_total(total);
}


void JarvisServiceImpl::GetFinancialRecord(::google::protobuf::RpcController* controller,
                       const ::jarvis::HttpRequest* request,
                       ::jarvis::HttpResponse* response,
                       ::google::protobuf::Closure* done)  {
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


    auto cond = kRecordCondition;
    const auto& uri = cntl->http_request().uri();
    auto pagestr     = uri.GetQuery("page"); /* start from 1 */
    auto pagesizestr = uri.GetQuery("pagesize");
    if (pagestr && pagesizestr) {
        cond.page = atoi(pagestr->c_str()) - 1;
        cond.pagesize = atoi(pagesizestr->c_str());
    }
    auto payer = uri.GetQuery("payer");
    if (payer && !payer->empty()) cond.payer = atoi(payer->c_str());
    auto payee = uri.GetQuery("payee");
    if (payee && !payee->empty()) cond.payee = atoi(payee->c_str());

    ::jarvis::financial_response tmp;
    financial_all_records(_db.get(), &tmp, cond);

    std::string data;
    google::protobuf::util::JsonPrintOptions option;
    option.always_print_primitive_fields = true;
    option.preserve_proto_field_names = true;
    google::protobuf::util::MessageToJsonString(tmp, &data, option);

    std::ostringstream oss;
    oss << "{\"status\":0, \"msg\":\"Success\", \"data\":" << data << "}";

    cntl->response_attachment().append(oss.str());
    LOG(INFO) << cntl->response_attachment();
}

void JarvisServiceImpl::AppendFinancialRecord(::google::protobuf::RpcController* controller,
                       const ::jarvis::financial_request* request,
                       ::jarvis::financial_response* response,
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


    const auto& record = request->record();
    const auto& nows       = basis::util::datetimenow();
    std::ostringstream cmd;
    cmd << "insert into " << table() << " ("
           "`when`, "
           "`payer`, "
           "`payee`, "
           "`amount`, "
           "`clasify`, "
           "`comments`, "
           "`create_time`, "
           "`update_time`"
           ") values (";
    cmd << "\"" << record.when() << "\", ";
    cmd << "\"" << record.payer() << "\", ";
    cmd << "\"" << record.payee() << "\", ";
    cmd << record.amount() << ", ";
    cmd << record.clasify() << ", ";
    cmd << "\"" << record.comments() << "\", ";
    cmd << "\"" << nows << "\", ";
    cmd << "\"" << nows << "\"";
    cmd << ")";

    std::unique_ptr<mysql::Result> suc(_db->Execute(cmd.str()));
    if (1 == suc->rows) {
        update_user_balance();
    }

    response->set_code(1 == suc->rows ? financial_response_rcode_ok : financial_response_rcode_dberr);
    return;
}

void JarvisServiceImpl::DeleteFinancialRecord(::google::protobuf::RpcController* controller,
                       const ::jarvis::financial_request* request,
                       ::jarvis::financial_response* response,
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

    const auto& record = request->record();
    std::ostringstream cmd;
    cmd << "update " << table() << " set status = 1 where id = " << record.id();
    _db->Execute(cmd.str());
    financial_all_records(_db.get(), response);
    response->set_code(financial_response_rcode_ok);
}

void JarvisServiceImpl::UpdateFinancialRecord(::google::protobuf::RpcController* controller,
                    const ::jarvis::financial_request* request,
                    ::jarvis::financial_response* response,
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

    const auto& record = request->record();
    std::ostringstream cmd;
    cmd << "update " << table() << " set ";
    cmd << "`when` = '" << record.when() << "', ";
    /* you should delete the error record first, then add a new one 
       if you want to update the amount, payee or payer
       cause the modify maybe change asset, it's terriable */
    // cmd << "`payer` = " << record.payer() << ", ";
    // cmd << "`payee` = " << record.payee() << ", ";
    // cmd << "`amount` = " << record.amount() << ", ";
    cmd << "`clasify` = " << record.clasify() << ", ";
    cmd << "`comments` = '" << record.comments() << "', ";
    cmd << "`update_time` = '" << basis::util::datetimenow() << "'";
    cmd << " where `id` = " << record.id();

    std::unique_ptr<mysql::Result> res(_db->Execute(cmd.str()));
    financial_all_records(_db.get(), response);

    response->set_code(1 == res->rows ? financial_response_rcode_ok : financial_response_rcode_dberr);
}


} // namespace jarvis
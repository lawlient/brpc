#include "jarvis.h"
#include "mysql.h"

using namespace google::protobuf;

namespace jarvis {


void mysql_test(::jarvis::financial_response* response) {
    sql::ResultSet *res;

    try {
        jarvis::financial financial;
        res = mysql_instance->SelectAll(financial, "", "id asc", "", "9");
        mysql_instance->Fprintf(1, financial, res);
        std::vector<google::protobuf::Message*> msgs;
        mysql_instance->Parse(res, &financial, msgs);

        for (const auto* msg : msgs) {
            const jarvis::financial* row = dynamic_cast<const jarvis::financial*>(msg);
            auto* r = response->add_records();
            r->set_id(row->id());
            r->set_timestamp(row->timestamp());
            r->set_account(row->account());
            r->set_amount(row->amount());
            r->set_type(row->type());
            r->set_comments(row->comments());
            r->set_ctime(row->ctime());
            r->set_mtime(row->mtime());
        }

        delete res;

    } catch (sql::SQLException &e) {
        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
        std::cout << "# ERR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }

    std::cout << std::endl;
    return;
}

void JarvisServiceImpl::TestQuery(::google::protobuf::RpcController* controller,
                       const ::jarvis::HttpRequest* request,
                       ::jarvis::HttpResponse* response,
                       ::google::protobuf::Closure* done)  {
    brpc::ClosureGuard done_guard(done);

    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
    ::jarvis::financial_response tmp;
    mysql_test(&tmp);
    LOG(INFO) << cntl->request_attachment();

    cntl->response_attachment().append("Success");
}



void JarvisServiceImpl::AppendFinancialRecord(::google::protobuf::RpcController* controller,
                       const ::jarvis::financial_request* request,
                       ::jarvis::financial_response* response,
                       ::google::protobuf::Closure* done) {
    brpc::ClosureGuard done_guard(done);

    const auto& record = request->record();
    std::ostringstream cmd;
    cmd << "insert into financial ("
           "`timestamp`, "
           "`account`, "
           "`amount`, "
           "`type`, "
           "`comments`, "
           "`ctime`, "
           "`mtime`"
           ") values (";
    cmd << "\"" << record.timestamp() << "\", ";
    cmd << "\"" << record.account() << "\", ";
    cmd << record.amount() << ", ";
    cmd << record.type() << ", ";
    cmd << "\"" << record.comments() << "\", ";

    char buf[50];
    struct tm tm;
    time_t now = time(nullptr);
    localtime_r(&now, &tm);
    strftime(buf, 50, "%F %T", &tm);
    cmd << "\"" << std::string(buf) << "\", ";
    cmd << "\"" << std::string(buf) << "\"";
    cmd << ")";
    std::cout << cmd.str() << std::endl;

    mysql_instance->Execute(cmd.str());

    response->set_code(financial_response_rcode_ok);
    return;
}


} // namespace jarvis

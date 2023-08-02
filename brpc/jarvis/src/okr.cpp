#include "jarvis.h"
#include "mysql.h"
#include "util.h"

#include <config/config.h>


namespace jarvis {


static inline std::string table() { return jarvis::okr::GetDescriptor()->name(); }

static inline std::string res2json(const google::protobuf::Message& m) {
    google::protobuf::util::JsonPrintOptions joption;
    joption.always_print_primitive_fields = true;
    joption.preserve_proto_field_names = true;
    std::string data;
    google::protobuf::util::MessageToJsonString(m, &data, joption);
    return data;
}

static std::shared_ptr<MysqlWrapper> make_sql_ins() { auto* cfg = basis::text_config::TextConfig::GetInstance(); const auto& mysql_config = cfg->cfg().mysql();
    MysqlOption mysql_option;
    mysql_option.url    = mysql_config.url();
    mysql_option.user   = mysql_config.user();
    mysql_option.passwd = mysql_config.passwd();
    mysql_option.schema = mysql_config.schema();
    return std::make_shared<MysqlWrapper>(mysql_option);
}

void JarvisServiceImpl::GetOKR(::google::protobuf::RpcController *controller,
                        const ::jarvis::HttpRequest *request,
                        ::jarvis::HttpResponse *response,
                        ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);
    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
    ::jarvis::okr_response okrres;
    okrres.set_code(::jarvis::okr_response_rcode_ok);
    okrres.set_msg("ok");


    std::ostringstream where;
    const auto& uri = cntl->http_request().uri();

    auto it         = uri.GetQuery("classify");
    if (it) where << "classify = " << *it;

    /// it              = uri.GetQuery("year");
    /// if (!it) {
    ///     okrres.set_code(::jarvis::okr_response_rcode_noyear);
    ///     okrres.set_msg("you have to specify year");
    ///     cntl->response_attachment().append(res2json(okrres));
    ///     return;
    /// }
        /// if (where.str().size()) where << " and ";
        /// where << " `date` = " << *it;

    
    std::unique_ptr<sql::ResultSet> res;
    ::jarvis::okr okr;
    auto sql = make_sql_ins();
    res.reset(sql->SelectAll(okr, where.str(), "id", "", "1000"));
    if (res) {
        std::vector<google::protobuf::Message*> tl;
        ::jarvis::obj obj;
        sql->Parse(res.get(), &obj, tl);
        for (const auto& t : tl) {
            const auto& row = *dynamic_cast<const ::jarvis::obj*>(t);
            if (row.level() == 1) {
                okrres.add_okr_list()->CopyFrom(row);
            } else if (row.level() == 2) {
                for (auto& o : (*okrres.mutable_okr_list())) {
                    if (o.id() != row.parent()) continue;

                    o.add_children()->CopyFrom(row);
                }
            }
        }
    }

    cntl->response_attachment().append(res2json(okrres));
}

void JarvisServiceImpl::AddOKR(::google::protobuf::RpcController *controller,
                        const ::jarvis::HttpRequest *request,
                        ::jarvis::HttpResponse *response,
                        ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);

    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
    ::jarvis::okr_response okrres;
    okrres.set_code(::jarvis::okr_response_rcode_ok);
    okrres.set_msg("ok");

    jarvis::okr t;
    const auto& body = cntl->request_attachment().to_string();
    google::protobuf::util::JsonStringToMessage(body, &t);
    const auto& now = basis::util::datetimenow();
    t.set_id(0);  // auto inc
    t.set_ctime(now);
    t.set_mtime(now);
    auto change = mysql_instance->InsertRaw(t);
    if (!change) {
        okrres.set_code(okr_response_rcode_dberr);
    }

    cntl->response_attachment().append(res2json(okrres));
}

void JarvisServiceImpl::UpdOKR(::google::protobuf::RpcController *controller,
                        const ::jarvis::HttpRequest *request,
                        ::jarvis::HttpResponse *response,
                        ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);

    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
    ::jarvis::okr_response okrres;
    okrres.set_code(::jarvis::okr_response_rcode_ok);
    okrres.set_msg("ok");

    jarvis::okr t;
    const auto& body = cntl->request_attachment().to_string();
    google::protobuf::util::JsonStringToMessage(body, &t);
    const auto& now = basis::util::datetimenow();
    t.set_mtime(now);       // update time
    auto change = mysql_instance->UpdateRaw(t, "id = " + std::to_string(t.id()));
    if (!change) {
        okrres.set_code(okr_response_rcode_dberr);
    }

    cntl->response_attachment().append(res2json(okrres));
}

void JarvisServiceImpl::DelOKR(::google::protobuf::RpcController *controller,
                        const ::jarvis::HttpRequest *request,
                        ::jarvis::HttpResponse *response,
                        ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);

    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
    ::jarvis::okr_response okrres;
    okrres.set_code(::jarvis::okr_response_rcode_ok);
    okrres.set_msg("ok");

    const auto& uri = cntl->http_request().uri();
    auto id         = uri.GetQuery("id");
    if (id) {
        const auto sql = "delete from " + table() + " where `id` = " + *id;
        bool suc = mysql_instance->Execute(sql);
        if (!suc)  {
            okrres.set_code(okr_response_rcode_dberr);
        }
    }
    cntl->response_attachment().append(res2json(okrres));
}




} // namespace jarvis


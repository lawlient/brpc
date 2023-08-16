#include "jarvis.h"
#include "util.h"


namespace jarvis {


static inline std::string table() {
    return jarvis::tasks::GetDescriptor()->name();
}


void JarvisServiceImpl::GetTask(::google::protobuf::RpcController *controller,
                        const ::jarvis::HttpRequest *request,
                        ::jarvis::TaskResponse *response,
                        ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);
    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
    
    response->set_status(0);
    response->set_msg("success");


    const auto& uri = cntl->http_request().uri();
    std::vector<std::string> conditions;
    auto it         = uri.GetQuery("status");
    if (it && it->size()) {
        if (it->at(0) == '(') conditions.push_back("status in " + *it);
        else if (isdigit( it->at(0))) conditions.push_back("status = " + *it);
    }
    it = uri.GetQuery("date");
    if (it && it->size()) {
        if (it->at(0) == '(') conditions.push_back("date in " + *it);
        else conditions.push_back("date = \'" + *it + "\'");
    }
    std::ostringstream where;
    if (conditions.size()) {
        where  << conditions[0];
        for (size_t i = 1; i < conditions.size(); i++) {
            where << " and " << conditions[i];
        }
    }
    
    std::unique_ptr<sql::ResultSet> res;
    ::jarvis::tasks tasks;
    res.reset(make_sql_ins()->SelectAll(tasks, where.str(), "id", "", "1000"));
    if (!res) {
        response->set_status(1);
        response->set_msg("select from table fail");
        return;
    }

    std::vector<google::protobuf::Message*> tl;
    make_sql_ins()->Parse(res.get(), &tasks, tl);
    for (const auto* t : tl) {
        response->mutable_data()->add_items()->CopyFrom(*dynamic_cast<const jarvis::tasks*>(t));
    }
}

void JarvisServiceImpl::AddTask(::google::protobuf::RpcController *controller,
                        const ::jarvis::HttpRequest *request,
                        ::jarvis::TaskResponse *response,
                        ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);

    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);

    response->set_status(0);
    response->set_msg("ok");

    jarvis::tasks t;
    const auto& body = cntl->request_attachment().to_string();
    google::protobuf::util::JsonStringToMessage(body, &t);
    const auto& now = basis::util::datetimenow();
    t.set_id(0);  // auto inc
    t.set_ctime(now);
    t.set_mtime(now);
    auto status = make_sql_ins()->InsertRaw(t);
    if (status.code) {
        response->set_status(status.code);
        response->set_msg(status.msg);
    }
}

void JarvisServiceImpl::UpdTask(::google::protobuf::RpcController *controller,
                        const ::jarvis::HttpRequest *request,
                        ::jarvis::TaskResponse *response,
                        ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);

    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
    response->set_status(0);
    response->set_msg("ok");

    jarvis::tasks t;
    const auto& now = basis::util::datetimenow();
    if (!parse_param_from_http_req(cntl, response, &t)) {
        return;
    }
    t.set_mtime(now);       // update time
    auto status = make_sql_ins()->UpdateRaw(t, "id = " + std::to_string(t.id()));
    if (status.code) {
        response->set_status(status.code);
        response->set_msg(status.msg);
    }
}

void JarvisServiceImpl::DelTask(::google::protobuf::RpcController *controller,
                        const ::jarvis::HttpRequest *request,
                        ::jarvis::TaskResponse *response,
                        ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);

    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
    response->set_status(0);
    response->set_msg("ok");

    jarvis::tasks t;
    if (!parse_param_from_http_req(cntl, response, &t)) {
        return;
    }
    if (t.id()) {
        const auto sql = "delete from " + table() + " where `id` = " + std::to_string(t.id());
        bool suc = make_sql_ins()->Execute(sql);
        if (!suc)  {
            response->set_status(1);
            response->set_msg("execute delete from table error");
        }
    }
}




} // namespace jarvis


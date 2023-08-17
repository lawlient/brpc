#include "jarvis.h"
#include "util.h"

#include "sql_generator.h"

namespace jarvis {


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
    
    ::jarvis::tasks tasks;
    mysql::SelectGenerator g(tasks);
    g.where(where.str());
    g.orderby("id");
    g.limit("1000");
    std::unique_ptr<mysql::Result> res(_db->Execute(g.sql(), &tasks));
    for (const auto& t : res->records) {
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
    mysql::InsertGenerator g(t);
    std::unique_ptr<mysql::Result> status(_db->Execute(g.sql()));
    if (status->code) {
        response->set_status(status->code);
        response->set_msg(status->msg);
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
    mysql::UpdateGenerator g(t);
    g.where("id = " + std::to_string(t.id()));
    std::unique_ptr<mysql::Result> res(_db->Execute(g.sql()));
    if (res->code) {
        response->set_status(res->code);
        response->set_msg(res->msg);
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
    response->set_msg("success");

    jarvis::tasks t;
    if (!parse_param_from_http_req(cntl, response, &t)) {
        return;
    }
    if (t.id()) {
        mysql::DeleteGenerator g(t);
        g.where("id = " + std::to_string(t.id()));
        std::unique_ptr<mysql::Result> res(_db->Execute(g.sql()));
        if (res->code)  {
            response->set_status(res->code);
            response->set_msg(res->msg);
        }
    }
}




} // namespace jarvis


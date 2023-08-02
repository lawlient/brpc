#include "jarvis.h"
#include "util.h"


namespace jarvis {


static inline std::string table() {
    return jarvis::tasks::GetDescriptor()->name();
}

static inline std::string res2json(const google::protobuf::Message& m) {
    google::protobuf::util::JsonPrintOptions joption;
    joption.always_print_primitive_fields = true;
    joption.preserve_proto_field_names = true;
    std::string data;
    google::protobuf::util::MessageToJsonString(m, &data, joption);
    return data;
}


void JarvisServiceImpl::GetTask(::google::protobuf::RpcController *controller,
                        const ::jarvis::HttpRequest *request,
                        ::jarvis::HttpResponse *response,
                        ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);
    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
    ::jarvis::task_response taskres;
    taskres.set_code(::jarvis::task_response_rcode_ok);
    taskres.set_msg("ok");


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
    res.reset(mysql_instance->SelectAll(tasks, where.str(), "id", "", "1000"));
    if (res) {
        std::vector<google::protobuf::Message*> tl;
        mysql_instance->Parse(res.get(), &tasks, tl);
        for (const auto* t : tl) {
            taskres.add_task_list()->CopyFrom(*dynamic_cast<const jarvis::tasks*>(t));
        }
    }

    cntl->response_attachment().append(res2json(taskres));
}

void JarvisServiceImpl::AddTask(::google::protobuf::RpcController *controller,
                        const ::jarvis::HttpRequest *request,
                        ::jarvis::HttpResponse *response,
                        ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);

    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
    ::jarvis::task_response taskres;
    taskres.set_code(::jarvis::task_response_rcode_ok);
    taskres.set_msg("ok");

    jarvis::tasks t;
    const auto& body = cntl->request_attachment().to_string();
    google::protobuf::util::JsonStringToMessage(body, &t);
    const auto& now = basis::util::datetimenow();
    t.set_id(0);  // auto inc
    t.set_ctime(now);
    t.set_mtime(now);
    auto change = mysql_instance->InsertRaw(t);
    if (!change) {
        taskres.set_code(task_response_rcode_dberr);
    }

    cntl->response_attachment().append(res2json(taskres));
}

void JarvisServiceImpl::UpdTask(::google::protobuf::RpcController *controller,
                        const ::jarvis::HttpRequest *request,
                        ::jarvis::HttpResponse *response,
                        ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);

    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
    ::jarvis::task_response taskres;
    taskres.set_code(::jarvis::task_response_rcode_ok);
    taskres.set_msg("ok");

    jarvis::tasks t;
    const auto& body = cntl->request_attachment().to_string();
    google::protobuf::util::JsonStringToMessage(body, &t);
    const auto& now = basis::util::datetimenow();
    t.set_mtime(now);       // update time
    auto change = mysql_instance->UpdateRaw(t, "id = " + std::to_string(t.id()));
    if (!change) {
        taskres.set_code(task_response_rcode_dberr);
    }

    cntl->response_attachment().append(res2json(taskres));
}

void JarvisServiceImpl::DelTask(::google::protobuf::RpcController *controller,
                        const ::jarvis::HttpRequest *request,
                        ::jarvis::HttpResponse *response,
                        ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);

    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
    ::jarvis::task_response taskres;
    taskres.set_code(::jarvis::task_response_rcode_ok);
    taskres.set_msg("ok");

    jarvis::tasks t;
    const auto& body = cntl->request_attachment().to_string();
    google::protobuf::util::JsonStringToMessage(body, &t);
    if (t.id()) {
        const auto sql = "delete from " + table() + " where `id` = " + std::to_string(t.id());
        bool suc = mysql_instance->Execute(sql);
        if (!suc)  {
            taskres.set_code(task_response_rcode_dberr);
        }
    }
    cntl->response_attachment().append(res2json(taskres));
}




} // namespace jarvis


#include "jarvis.h"
#include "mysql.h"
#include "util.h"

#include <config/config.h>

#include <butil/strings/string_number_conversions.h>


namespace jarvis {


static inline std::string table() { return jarvis::okr::GetDescriptor()->name(); }

/* 将select返回的数据改造成树结构, 通过递归实现广度优先遍历 */
static void make_tree(std::vector<google::protobuf::Message*>& tl, ::jarvis::obj* n) {
    auto it = tl.begin();
    while (it != tl.end()) {
        const auto& row = *dynamic_cast<const ::jarvis::obj*>(it.operator*());
        if (row.parent() != n->id()) {
            it++;
            continue;
        }

        n->add_children()->CopyFrom(row);
        it = tl.erase(it);
    }
    for (auto& c : *(n->mutable_children())) {
        make_tree(tl, &c);
    }
}

void JarvisServiceImpl::GetOKR(::google::protobuf::RpcController *controller,
                        const ::jarvis::HttpRequest *request,
                        ::jarvis::OKResponse *response,
                        ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);
    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
    response->set_status(0);
    response->set_msg("success");

    std::ostringstream where;
    const auto& uri = cntl->http_request().uri();

    std::vector<std::string> conditions;
    auto it         = uri.GetQuery("classify");
    if (it) conditions.push_back("classify = " + *it);

    it              = uri.GetQuery("year");
    if (it) {
        int y = 0;
        if (!butil::StringToInt(*it, &y)) {
            response->set_status(1);
            response->set_msg("you have to specify year");
            return;
        }
        it = uri.GetQuery("month");
        if (it) {
            int m = 0;
            if (butil::StringToInt(*it, &m)) {
                struct tm tm;
                memset(&tm, 0, sizeof(tm));
                tm.tm_year = y - 1900;
                tm.tm_mon = m - 1;
                tm.tm_mday = 1;
                char firstday[11], lastday[11];
                strftime(firstday, 11, "%F", &tm);
                tm.tm_mday = basis::util::dayofmonth(y, m);
                strftime(lastday, 11, "%F", &tm);
                conditions.push_back("start <= '" + std::string(lastday) + "'");
                conditions.push_back("end >= '" + std::string(firstday) + "'");
            }
        }
    }

    if (conditions.size()) {
        where << "(" << conditions[0] << ")";
        for (size_t i = 1; i < conditions.size(); i++) {
            where << " and (" << conditions[i] << ")";
        }
    }
    
    std::unique_ptr<sql::ResultSet> res;
    ::jarvis::okr okr;
    auto sql = make_sql_ins();
    res.reset(sql->SelectAll(okr, where.str(), "id", "", "1000"));
    if (res) {
        std::vector<google::protobuf::Message*> tl;
        ::jarvis::obj obj;
        sql->Parse(res.get(), &obj, tl);

        /* 该过程同make_tree的过程，由于第一层并没有一个共同祖先存在，此处单独处理 */
        auto it = tl.begin();
        while (it != tl.end()) {
            const auto& row = *dynamic_cast<const ::jarvis::obj*>(it.operator*());
            if (row.parent() > 0) {
                it++;
                continue;
            }
            response->mutable_data()->add_items()->CopyFrom(row);
            it = tl.erase(it);
        }
        for (auto& okr : *(response->mutable_data()->mutable_items())) {
            make_tree(tl, &okr);
        }
    }
}

void JarvisServiceImpl::AddOKR(::google::protobuf::RpcController *controller,
                        const ::jarvis::HttpRequest *request,
                        ::jarvis::OKResponse *response,
                        ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);

    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
    response->set_status(0);
    response->set_msg("success");

    jarvis::okr t;
    const auto& body = cntl->request_attachment().to_string();
    google::protobuf::util::JsonStringToMessage(body, &t);
    const auto& now = basis::util::datetimenow();
    t.set_id(0);  // auto inc
    t.set_ctime(now);
    t.set_mtime(now);
    auto status = make_sql_ins()->InsertRaw(t);
    if (status.rows == 0) {
        response->set_status(1);
        response->set_msg(status.msg);
    }
}

void JarvisServiceImpl::UpdOKR(::google::protobuf::RpcController *controller,
                        const ::jarvis::HttpRequest *request,
                        ::jarvis::OKResponse *response,
                        ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);

    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
    response->set_status(0);
    response->set_msg("success");

    jarvis::okr o;
    if (!parse_param_from_http_req(cntl, response, &o)) {
        return;
    }
    const auto& now = basis::util::datetimenow();
    o.set_mtime(now);       // update time
    auto status = make_sql_ins()->UpdateRaw(o, "id = " + std::to_string(o.id()));
    if (!status.rows) {
        response->set_status(1);
        response->set_msg(status.msg);
    }
}

void JarvisServiceImpl::DelOKR(::google::protobuf::RpcController *controller,
                        const ::jarvis::HttpRequest *request,
                        ::jarvis::OKResponse *response,
                        ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);

    common_cntl_set(controller);
    brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
    response->set_status(0);
    response->set_msg("ok");

    jarvis::okr o;
    if (!parse_param_from_http_req(cntl, response, &o)) {
        return;
    }
    if (o.id()) {
        const auto sql = "delete from " + table() + " where `id` = " + std::to_string(o.id());
        bool suc = make_sql_ins()->Execute(sql);
        if (!suc)  {
            response->set_status(1);
            response->set_msg("delete from table fail");
        }
    }
}




} // namespace jarvis


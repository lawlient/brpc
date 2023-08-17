#pragma once

#include <google/protobuf/message.h>

#include <string>


namespace mysql {


class SQLGenerator {
public: 
    static std::string table(const google::protobuf::Message &table) {
        return table.GetDescriptor()->name();
    }

    virtual std::string sql() const = 0;

    virtual void where(const std::string& w) { _where = w; }
    virtual std::string where() const { return _where; }

    static std::ostream& field(std::ostream& os, const google::protobuf::Message &raw, const google::protobuf::FieldDescriptor* field) {
        using namespace google::protobuf;
        const auto* refl = raw.GetReflection();
        switch (field->type()) {
            case FieldDescriptor::Type::TYPE_DOUBLE: { os << field->name() << " = " << refl->GetDouble(raw, field); break; }
            case FieldDescriptor::Type::TYPE_FLOAT: { os << field->name() << " = " << refl->GetFloat(raw, field); break; }
            case FieldDescriptor::Type::TYPE_INT64: { os << field->name() << " = " << refl->GetInt64(raw, field); break; }
            case FieldDescriptor::Type::TYPE_UINT64: { os << field->name() << " = " << refl->GetUInt64(raw, field); break; }
            case FieldDescriptor::Type::TYPE_INT32: { os << field->name() << " = " << refl->GetInt32(raw, field); break; }
            case FieldDescriptor::Type::TYPE_FIXED64: { os << field->name() << " = " << refl->GetUInt64(raw, field); break; }
            case FieldDescriptor::Type::TYPE_FIXED32: { os << field->name() << " = " << refl->GetUInt32(raw, field); break; }
            case FieldDescriptor::Type::TYPE_STRING: { os << field->name() << " = " << "\"" << refl->GetString(raw, field) << "\""; break; }
            default: break;
        }
        return os;
    }

    static std::string fieldv(const google::protobuf::Message &raw, const google::protobuf::FieldDescriptor* field) {
        std::ostringstream os;
        using namespace google::protobuf;
        const auto* refl = raw.GetReflection();
        switch (field->type()) {
            case  FieldDescriptor::Type::TYPE_DOUBLE: { os << refl->GetDouble(raw, field); break; }
            case  FieldDescriptor::Type::TYPE_FLOAT: { os << refl->GetFloat(raw, field); break; }
            case  FieldDescriptor::Type::TYPE_INT64: { os << refl->GetInt64(raw, field); break; }
            case  FieldDescriptor::Type::TYPE_UINT64: { os << refl->GetUInt64(raw, field); break; }
            case  FieldDescriptor::Type::TYPE_INT32: { os << refl->GetInt32(raw, field); break; }
            case  FieldDescriptor::Type::TYPE_FIXED64: { os << refl->GetUInt64(raw, field); break; }
            case  FieldDescriptor::Type::TYPE_FIXED32: { os << refl->GetUInt32(raw, field); break; }
            case  FieldDescriptor::Type::TYPE_STRING: { os << "\"" << refl->GetString(raw, field) << "\""; break; }
            default: break;
        }
        return os.str();
    }

protected:
    SQLGenerator() = default;

private:
    std::string _where;
};


class SelectGenerator : public SQLGenerator {
public:
    SelectGenerator() = delete;
    explicit SelectGenerator(const google::protobuf::Message &meta) : _meta(meta) {}

    std::string sql() const override {
        std::ostringstream ss;
        ss << "SELECT" << " * from " << table(_meta);
        if (!where().empty())   ss << " where " << where();
        if (!_orderby.empty()) ss << " order by " << _orderby;
        if (!_groupby.empty()) ss << " group by " << _groupby;
        if (!_limit.empty())   ss << " limit " << _limit;
        return ss.str();
    }

    void orderby(const std::string& order) { _orderby = order; }
    std::string orderby() const { return _orderby; }

    void groupby(const std::string& group) { _groupby = group; }
    std::string groupby() const { return _groupby; }

    void limit(const std::string& limit) { _limit = limit; }
    std::string limit() const { return _limit; }

private:

private:
    const google::protobuf::Message &_meta;
    std::string _groupby;
    std::string _orderby;
    std::string _limit;
};


class InsertGenerator : public SQLGenerator {
public:
    InsertGenerator() = delete;
    explicit InsertGenerator(const google::protobuf::Message &meta) : _meta(meta) {}

    std::string sql() const override {
        const auto* desc = _meta.GetDescriptor();

        std::ostringstream ss;
        ss << "INSERT into " << table(_meta) <<  " ";
        ss << "(`" << desc->field(0)->name() << "`";
        for (int i = 1; i < desc->field_count(); i++) {
            ss << ", `" << desc->field(i)->name() << "`";
        }
        ss << ") VALUES (";

        ss << fieldv(_meta, desc->field(0));
        for (int i = 1; i < desc->field_count(); i++) {
            ss << ", " << fieldv(_meta, desc->field(i));
        }
        ss << ")";

        return ss.str();
    }
private:
    const google::protobuf::Message &_meta;
};

class UpdateGenerator : public SQLGenerator {
public:
    UpdateGenerator() = delete;
    explicit UpdateGenerator(const google::protobuf::Message &meta) : _meta(meta) {}

    std::string sql() const override {
        const auto* desc = _meta.GetDescriptor();

        std::ostringstream ss;
        ss << "UPDATE " << table(_meta) <<  " SET ";
        field(ss, _meta, desc->field(0));
        for (int i = 1; i < desc->field_count(); i++) {
            ss << ", ";
            field(ss, _meta, desc->field(i));
        }
        ss << " WHERE " << where();

        return ss.str();
    }
private:
    const google::protobuf::Message &_meta;
};


class DeleteGenerator : public SQLGenerator {
public:
    DeleteGenerator() = delete;
    explicit DeleteGenerator(const google::protobuf::Message &meta) : _meta(meta) {}

    std::string sql() const override {
        std::ostringstream ss;
        ss << "DELETE from " << table(_meta) << " WHERE " << where();
        return ss.str();
    }
private:
    const google::protobuf::Message &_meta;
};



} //namespace mysql 

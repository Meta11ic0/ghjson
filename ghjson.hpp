#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace ghjson
{

class JsonValue
{
    virtual const Json::JsonType GetTyoe() const;
    virtual const Json GetValue() const;
};

class Json
{
    public:

        enum class JsonType
        {
            NUL, NUMBER, BOOL, STRING, ARRAY, OBJECT
        };
        Json Parse(const std::string & str);
        std::string Dump();
    private:
        std::shared_ptr<JsonValue> m_ptr;
};

}
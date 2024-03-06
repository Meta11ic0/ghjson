#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace ghjson
{
enum class JsonType
{
    NUL, NUMBER, BOOL, STRING, ARRAY, OBJECT
};

class JsonValue
{
    public:
        virtual const JsonType GetType() const = 0;
        template<typename>
        
        virtual ~JsonValue(){};
};

using array = std::vector<JsonValue>;
using object = std::map<std::string,JsonValue>;

class Json
{
    public:
        Json Parse(const std::string & str);
        std::string Dump();
        const JsonType Type();
        const JsonValue Value();
    private:
        std::shared_ptr<JsonValue> m_ptr;
};

}
#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

namespace ghjson
{
    class Json;
    class JsonValue;

    using array = std::vector<Json>;
    using object = std::map<std::string, Json>;

    enum class JsonType
    {
        NUL, NUMBER, BOOL, STRING, ARRAY, OBJECT
    };

    class Json
    {
        public:
            //Constructor
            Json() noexcept;
            Json(std::nullptr_t) noexcept;
            Json(bool value);
            //Constructor

            //GetType
            JsonType Type() const;
            //GetType

            //GetValue
            double              GetNumber() const;
            bool                GetBool()   const;
            const std::string & GetString() const;
            const array &       GetArray()  const;
            const object &      GetObject() const;

            const Json &        operator[](size_t i) const;
            const Json &        operator[](const std::string &key) const;
            //GetValue

            //Comparisons
            bool operator== (const Json &rhs) const;
            bool operator<  (const Json &rhs) const;
            //Comparisons
            
        private:
            std::shared_ptr<JsonValue> m_ptr;
    };
}
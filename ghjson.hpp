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
            double GetDouble() const;
            int GetInt() const;
            // Return the enclosed value if this is a boolean, false otherwise.
            bool GetBool() const;
            // Return the enclosed string if this is a string, "" otherwise.
            const std::string &GetString() const;
            // Return the enclosed vector if this is an array, or an empty vector otherwise.
            const array &GetArray() const;
            // Return the enclosed map if this is an object, or an empty map otherwise.
            const object &GetObject() const;
            // Return a reference to arr[i] if this is an array, Json() otherwise.
            const Json & operator[](size_t i) const;
            // Return a reference to obj[key] if this is an object, Json() otherwise.
            const Json & operator[](const std::string &key) const;

            JsonType Type() const;
            // Serialize.
            std::string Dump() const;
            // Parse. 
            static Json Parse(const std::string & in, std::string & err);
        private:
            std::shared_ptr<JsonValue> m_ptr;
    };

    class JsonValue
    {
        public:
            virtual const std::string Dump() const = 0;
            virtual const JsonType Type() const = 0;

            virtual double GetDouble() const;
            virtual int GetInt() const;
            virtual bool GetBool() const;
            virtual const std::string& GetString() const;
            virtual const array& GetArray() const;
            virtual const Json& operator[](size_t i) const;
            virtual const object& GetObject() const;
            virtual const Json& operator[](const std::string& key) const;

            virtual ~JsonValue();
    };
}
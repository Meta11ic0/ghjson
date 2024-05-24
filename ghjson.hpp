#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <cassert>
#include <stdexcept>

#define DEBUG 
#ifdef DEBUG
#include <iostream>
using namespace std;
#endif


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
            Json() noexcept;                // NUL
            Json(std::nullptr_t) noexcept;  // NUL
            Json(int value);             // NUMBER
            Json(double value);             // NUMBER
            Json(bool value);               // BOOL
            Json(const std::string &value); // STRING
            Json(std::string &&value);      // STRING
            Json(const char * value);       // STRING
            Json(const array &values);      // ARRAY
            Json(array &&values);           // ARRAY
            Json(const object &values);     // OBJECT
            Json(object &&values);          // OBJECT
            //Constructor

            //Type
            JsonType Type() const;
            bool is_null()   const { return Type() == JsonType::NUL; }
            bool is_number() const { return Type() == JsonType::NUMBER; }
            bool is_bool()   const { return Type() == JsonType::BOOL; }
            bool is_string() const { return Type() == JsonType::STRING; }
            bool is_array()  const { return Type() == JsonType::ARRAY; }
            bool is_object() const { return Type() == JsonType::OBJECT; }
            //Type

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
            bool operator!= (const Json &rhs) const { return !(*this == rhs); }
            bool operator<= (const Json &rhs) const { return !(rhs < *this); }
            bool operator>  (const Json &rhs) const { return  (rhs < *this); }
            bool operator>= (const Json &rhs) const { return !(*this < rhs); }
            //Comparisons

            //dump
            void dump(std::string &out) const;
            std::string dump() const
            {
                std::string str;
                dump(str);
                return str;
            }
            //dump
        private:
            std::shared_ptr<JsonValue> m_ptr;
    };

    class JsonParseException : public std::runtime_error 
    {
        public:
            JsonParseException(const std::string& message, size_t position): std::runtime_error(message), pos(position) {}
            size_t GetPosition() const { return pos; }
        private:
            size_t pos;
    };
    
    Json Parse(const std::string & in);
    
}
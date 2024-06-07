#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <cassert>
#include <stdexcept>
#include <utility>

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
            Json(const Json & other);
            Json(Json && other) noexcept;
            Json& operator=(const Json& other) ;
            Json& operator=(Json&& other) noexcept;
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

            //SetValue
            void SetNumber (double               value);
            inline void SetNumber (int value)
            {
                SetNumber(static_cast<double>(value));
            }
            void SetBool   (bool                 value);
            void SetString (const  std::string & value);
            inline void SetString (const char * value)
            {
                SetString(std::string(value));
            }
            void SetArray  (const  array       & value);
            void SetObject (const  object      & value);

            void AddToArray (const Json & value);
            void AddToObject(const std::string & key, const Json & value);

            void RemoveFromArray(size_t index) ;
            void RemoveFromObject(const std::string& key);
            //SetValue

            //Comparisons
            bool operator== (const Json &rhs) const;
            bool operator<  (const Json &rhs) const;
            bool operator!= (const Json &rhs) const { return !(*this == rhs); }
            bool operator<= (const Json &rhs) const { return !(rhs < *this); }
            bool operator>  (const Json &rhs) const { return  (rhs < *this); }
            bool operator>= (const Json &rhs) const { return !(*this < rhs); }
            //Comparisons

            //Dump
            void Dump(std::string &out) const;

            inline std::string Dump() const
            {
                std::string str;
                Dump(str);
                return str;
            }
            //Dump

            //swap
            void swap(Json &other) noexcept;
            //swap
        
            
        private:
            std::shared_ptr<JsonValue> m_ptr;
    };

    class ghJsonException : public std::runtime_error 
    {
        public:
            ghJsonException(const std::string& message, size_t position): std::runtime_error(message), pos(position) {}
            size_t GetPosition() const { return pos; }
        private:
            size_t pos;
    };
    
    Json Parse(const std::string & in);
    
    inline Json Parse(const char * in)
    {
        if(in)
        {
            return Parse(std::string(in));
        }
        else
        {
            throw ghJsonException("null str", 0);
        }
    }

    // 定义一个将JsonType转换为字符串的函数
    inline const char * ToString(ghjson::JsonType type)
    {
        switch (type) 
        {
            case ghjson::JsonType::NUL:    return "null";
            case ghjson::JsonType::NUMBER: return "number";
            case ghjson::JsonType::BOOL:   return "bool";
            case ghjson::JsonType::STRING: return "string";
            case ghjson::JsonType::ARRAY:  return "array";
            case ghjson::JsonType::OBJECT: return "object";
            default:                       return "unknown";
        }
    }

    inline std::ostream & operator<<(std::ostream & os, ghjson::JsonType tag)
    {
        return os << std::string(ToString(tag));
    }
}


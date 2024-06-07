#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <stdexcept>

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
        private:
            std::unique_ptr<JsonValue> m_ptr;
        public:
            //constructor
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
            Json& operator=(Json && other) noexcept;
            
            ~Json() noexcept {};
            //constructor

            //type
            JsonType type() const;
            bool is_null()   const;
            bool is_number() const;
            bool is_bool()   const;
            bool is_string() const;
            bool is_array()  const;
            bool is_object() const;
            //type

            //getValue
            double              getNumber() const;
            bool                getBool()   const;
            const std::string & getString() const;
            const array &       getArray()  const;
            const object &      getObject() const;
            //getValue

            //operator[]
            Json & operator[](size_t index);
            Json & operator[](const std::string &key);
            const Json & operator[](size_t index) const ; 
            const Json & operator[](const std::string &key) const;
            //operator[]

            //setValue
            void setNumber (double               value);
            void setBool   (bool                 value);
            void setString (const  std::string & value);
            void setArray  (const  array       & value);
            void setObject (const  object      & value);

            void addToArray (const Json & value);
            void addToObject(const std::string & key, const Json & value);

            void removeFromArray(size_t index) ;
            void removeFromObject(const std::string& key);
            //setValue

            //comparisons
            bool operator== (const Json &rhs) const;
            bool operator<  (const Json &rhs) const;
            bool operator!= (const Json &rhs) const;
            bool operator<= (const Json &rhs) const;
            bool operator>  (const Json &rhs) const;
            bool operator>= (const Json &rhs) const;
            //comparisons

            //dump
            void dump(std::string & str);
            //dump

            //check
            inline void check() const
            {
                if(!m_ptr)
                {
                    throw ghJsonException("Bad Json access: m_ptr is null", 0);
                } 
            }
            //check
    };

    class ghJsonException : public std::runtime_error 
    {
        public:
            ghJsonException(const std::string& message, size_t position): std::runtime_error(message), pos(position) {}
            size_t GetPosition() const { return pos; }
        private:
            size_t pos;
    };

    Json parse(const std::string & in);

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

}
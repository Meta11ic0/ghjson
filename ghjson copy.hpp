#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace ghjson
{
    class JsonValue;
    class Json;
    using array = std::vector<Json>;
    using object = std::map<std::string,Json>;
    enum class JsonType
    {
        NUL, NUMBER, BOOL, STRING, ARRAY, OBJECT
    };

    class JsonValue
    {
        public:
            virtual const JsonType type() const = 0;
            //要提供比较函数
            virtual bool equals(const JsonValue * other) const = 0;
            virtual bool less(const JsonValue * other) const = 0;
            virtual const std::string dump() const = 0;
            virtual double number_value() const;
            virtual int int_value() const; 
            virtual bool bool_value() const;
            virtual const std::string &string_value() const;
            virtual const array &array_items() const;
            virtual const Json &operator[](size_t i) const;
            virtual const object &object_items() const;
            virtual const Json &operator[](const std::string &key) const;
            virtual ~JsonValue(){};   
    };

    class Json
    {
        public:
            // Constructors for the various types of JSON value.
            Json() noexcept;                // NUL
            Json(std::nullptr_t) noexcept;  // NUL
            Json(double value);             // NUMBER
            Json(int value);                // NUMBER
            Json(bool value);               // BOOL
            Json(const std::string &value); // STRING
            Json(std::string &&value);      // STRING
            Json(const char * value);       // STRING
            Json(const array &values);      // ARRAY
            Json(array &&values);           // ARRAY
            Json(const object &values);     // OBJECT
            Json(object &&values);          // OBJECT
            
            // Return the enclosed value if this is a number, 0 otherwise. Note that json11 does not
            // distinguish between integer and non-integer numbers - number_value() and int_value()
            // can both be applied to a NUMBER-typed object.
            double number_value() const;
            int int_value() const;
            //int int_value() const;
            // Return the enclosed value if this is a boolean, false otherwise.
            bool bool_value() const;
            // Return the enclosed string if this is a string, "" otherwise.
            const std::string &string_value() const;
            // Return the enclosed vector if this is an array, or an empty vector otherwise.
            const array &array_items() const;
            // Return the enclosed map if this is an object, or an empty map otherwise.
            const object &object_items() const;

            // Return a reference to arr[i] if this is an array, Json() otherwise.
            const Json & operator[](size_t i) const;
            // Return a reference to obj[key] if this is an object, Json() otherwise.
            const Json & operator[](const std::string &key) const;
            // Accessors
            JsonType type() const;

            bool is_null()   const { return type() == JsonType::NUL; }
            bool is_number() const { return type() == JsonType::NUMBER; }
            bool is_bool()   const { return type() == JsonType::BOOL; }
            bool is_string() const { return type() == JsonType::STRING; }
            bool is_array()  const { return type() == JsonType::ARRAY; }
            bool is_object() const { return type() == JsonType::OBJECT; }

            // Serialize.
            void dump(std::string &out) const;
            std::string dump() const 
            {
                std::string out;
                dump(out);
                return out;
            }
            // Parse. If parse fails, return Json() and assign an error message to err.
            static Json parse(const std::string & in, std::string & err);
            static Json parse(const char * in, std::string & err)
            {
                if (in) 
                {
                    return parse(std::string(in), err);
                } 
                else 
                {
                    err = "null input";
                    return nullptr;
                }
            }

            
            bool operator== (const Json &rhs) const;
            bool operator<  (const Json &rhs) const;
            bool operator!= (const Json &rhs) const { return !(*this == rhs); }
            bool operator<= (const Json &rhs) const { return !(rhs < *this); }
            bool operator>  (const Json &rhs) const { return  (rhs < *this); }
            bool operator>= (const Json &rhs) const { return !(*this < rhs); }
        private:
            std::shared_ptr<JsonValue> m_ptr;
    };

}
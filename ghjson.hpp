#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace ghjson
{
    using std::string;
    using std::vector;
    using std::map;

    enum class JsonType
    {
        NUL, NUMBER, BOOL, STRING, ARRAY, OBJECT
    };

    using array = vector<JsonValue>;
    using object = map<string,JsonValue>;

    class JsonValue
    {
        public:
            virtual const JsonType type() const = 0;
            virtual const string dump() const = 0;
            virtual double number_value() const;
            virtual int int_value() const; 
            virtual bool bool_value() const;
            virtual const string &string_value() const;
            virtual const array &array_items() const;
            virtual const Json &operator[](size_t i) const;
            virtual const object &object_items() const;
            virtual const Json &operator[](const string &key) const;
            virtual ~JsonValue(){};   
    };



    class Json
    {
        public:
            Json Parse(const string & str);
            string dump() const;
            const JsonType type() const;
            // Return the enclosed value if this is a number, 0 otherwise. Note that json11 does not
            // distinguish between integer and non-integer numbers - number_value() and int_value()
            // can both be applied to a NUMBER-typed object.
            const double number_value() const;
            //int int_value() const;
            // Return the enclosed value if this is a boolean, false otherwise.
            const bool bool_value() const;
            // Return the enclosed string if this is a string, "" otherwise.
            const string &string_value() const;
            // Return the enclosed vector if this is an array, or an empty vector otherwise.
            const array &array_items() const;
            // Return the enclosed map if this is an object, or an empty map otherwise.
            const object &object_items() const;

            // Return a reference to arr[i] if this is an array, Json() otherwise.
            const Json & operator[](size_t i) const;
            // Return a reference to obj[key] if this is an object, Json() otherwise.
            const Json & operator[](const string &key) const;

        private:
            std::shared_ptr<JsonValue> m_ptr;
    };

}
#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <stdexcept>

#define MAXDEPTH 10

namespace ghjson
{

    class Json;
    class JsonValue;
    
    using array = std::vector<Json>;
    using object = std::map<std::string, Json>;
    using arrayiter = array::iterator;
    using const_arrayiter = array::const_iterator;
    using objectiter = object::iterator;
    using const_objectiter = object::const_iterator;

    class ghJsonException : public std::runtime_error 
    {
        public:
            ghJsonException(const std::string& message, size_t position): std::runtime_error(message), pos(position) {}
            size_t getPosition() const { return pos; }
        private:
            size_t pos;
    };

    enum class JsonType
    {
        NUL, NUMBER, BOOL, STRING, ARRAY, OBJECT
    };

    //JsonValue
    class JsonValue
    {
        public:
            //type
            virtual JsonType type() const = 0;
            //type
            //comparisons
            virtual bool equals(const JsonValue * other) const = 0;
            virtual bool less(const JsonValue * other) const  = 0;
            //comparisons
            //getvalue
            virtual double              getNumber() const = 0;
            virtual bool                getBool  () const = 0;
            virtual const std::string & getString() const = 0;
            virtual const array &       getArray () const = 0;
            virtual const object &      getObject() const = 0;
            //getvalue
            //operator[]
            virtual Json& operator[](size_t index)  = 0;
            virtual Json& operator[](const std::string& key)  = 0;
            virtual const Json& operator[](size_t index) const   = 0;
            virtual const Json& operator[](const std::string& key) const  = 0;
            //operator[]
            //setvalue
            virtual void setNumber (double               value)  = 0;
            virtual void setBool   (bool                 value)  = 0;
            virtual void setString (const  std::string & value)  = 0;
            virtual void setArray  (const  array       & value)  = 0;
            virtual void setObject (const  object      & value)  = 0;

            virtual void addToArray (const Json & value)  = 0;
            virtual void addToObject(const std::string & key, const Json & value)  = 0;

            virtual void removeFromArray(size_t index)  = 0;
            virtual void removeFromObject(const std::string& key)  = 0;
            //setvalue
            //clone
            virtual std::unique_ptr<JsonValue> clone() const = 0;
            //clone
            //dump
            virtual void dump(std::string & str, size_t depth) const = 0;
            //dump
            //iterator
            virtual arrayiter arrayBegin() = 0;
            virtual const_arrayiter arrayBegin_const() const = 0;
            virtual arrayiter arrayEnd() = 0;
            virtual const_arrayiter arrayEnd_const() const = 0;
            virtual objectiter objectBegin() = 0;
            virtual const_objectiter objectBegin_const() const = 0;
            virtual objectiter objectEnd() = 0;
            virtual const_objectiter objectEnd_const() const = 0;
            //iterator
            virtual ~JsonValue() noexcept {} ;
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
            bool operator!= (const Json &rhs) const { return !(*this == rhs); }
            bool operator<= (const Json &rhs) const { return !(rhs < *this); }
            bool operator>  (const Json &rhs) const { return  (rhs < *this); }
            bool operator>= (const Json &rhs) const { return !(*this < rhs); }  
            //comparisons
            //dump
            void dump(std::string & str, size_t depth) const;
            const std::string dump() const;
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
            //iterator
            arrayiter arrayBegin();
            const_arrayiter arrayBegin_const() const ;
            arrayiter arrayEnd();
            const_arrayiter arrayEnd_const() const ;
            objectiter objectBegin();
            const_objectiter objectBegin_const() const ;
            objectiter objectEnd();
            const_objectiter objectEnd_const() const ;
            //iterator
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

    inline std::ostream & operator<<(std::ostream & os, ghjson::JsonType tag)
    {
        return os << std::string(ToString(tag));
    }

}
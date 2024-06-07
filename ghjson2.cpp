#include "ghjson2.hpp"

namespace ghjson
{
    class JsonValue
    {
        public:
            //type
            virtual JsonType type() const = 0;
            //type

            //comparisons
            virtual bool equals(const JsonValue * other) const = 0 ;
            virtual bool less(const JsonValue * other) const = 0 ;
            //comparisons

            //dump
            virtual void dump(std::string &out) const = 0;
            //dump

            //getvalue
            virtual double              getNumber() const { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            virtual bool                getBool  () const { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            virtual const std::string & getString() const { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            virtual const array &       getArray () const { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            virtual const object &      getObject() const { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            //getvalue

            //operator[]
            virtual Json& operator[](size_t index) { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            virtual Json& operator[](const std::string& key) { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            virtual const Json& operator[](size_t index) const  { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            virtual const Json& operator[](const std::string& key) const { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            //operator[]

            //setvalue
            virtual void setNumber (double               value) { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            virtual void setBool   (bool                 value) { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            virtual void setString (const  std::string & value) { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            virtual void setArray  (const  array       & value) { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            virtual void setObject (const  object      & value) { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }

            virtual void addToArray (const Json & value) { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            virtual void addToObject(const std::string & key, const Json & value) { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }

            virtual void removeFromArray(size_t index) { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            virtual void removeFromObject(const std::string& key) { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            //setvalue

            //clone
            virtual std::unique_ptr<JsonValue> clone() const = 0;
            //clone

            virtual ~JsonValue() noexcept {} ;
    };

    template<JsonType tag, typename T>
    class Value : public JsonValue
    {
        protected:
            T m_value;
            //constructor
            explicit Value(const T &value) : m_value(value) {}
            explicit Value(T &&value)      : m_value(std::move(value)) {}
            //constructor
            //type
            JsonType type() const override { return tag; }
            //type
            //comparisons
            bool equals(const JsonValue * other) const { return m_value == static_cast<const Value<tag, T> *>(other)->m_value; }
            bool less(const JsonValue * other) const { return m_value < static_cast<const Value<tag, T> *>(other)->m_value; }
            //comparisons
    };

    class NullClass
    {
        public:
            bool operator == (NullClass) const { return true; }
            bool operator < (NullClass)  const { return false; }
    };

    class JsonNull : public Value<JsonType::NUL, NullClass>
    {
        public:
            explicit JsonNull() : Value(NullClass()) {};
            //clone
            virtual std::unique_ptr<JsonValue> clone() const override{ return std::make_unique<JsonNull>(); }
            //clone
    };

    class JsonBool : public Value<JsonType::BOOL, bool>
    {
        public:
            explicit JsonBool(bool value) : Value(value) {};
        private:
            //getValue
            bool getBool() const override { return m_value; }
            //getValue
            //setvalue
            void setBool (bool value) override { m_value = value; }
            //setvalue
            //clone
            virtual std::unique_ptr<JsonValue> clone() const override{ return std::make_unique<JsonBool>(); }
            //clone
    };

    class JsonNumber : public Value<JsonType::NUMBER, double>
    {
        public:
            explicit JsonNumber(double value) : Value(value) {};
        private:
            //getValue
            double getNumber() const override { return m_value; }
            //getValue
            //setvalue
            void setNumber(double value) override { m_value = value; }
            //setvalue
            //clone
            virtual std::unique_ptr<JsonValue> clone() const override{ return std::make_unique<JsonNumber>(); }
            //clone
    };

    class JsonString : public Value<JsonType::STRING, std::string>
    {
        public:
            explicit JsonString(const std::string& value) : Value(value) {}
            explicit JsonString(std::string&& value) : Value(move(value)) {}
        private:
            //getValue
            const std::string & getString() const override { return m_value; }
            //getValue
            //setvalue
            void setString (const std::string & value) override { m_value = value; }
            //setvalue
            //clone
            virtual std::unique_ptr<JsonValue> clone() const override{ return std::make_unique<JsonString>(); }
            //clone
    };

    class JsonArray : public Value<JsonType::ARRAY, array>
    {
        public:
            explicit JsonArray(const array& value) : Value(value) {};
            explicit JsonArray(array&& value) : Value(move(value)) {};        
        private:
            //getValue
            const array & getArray() const override { return m_value; }
            //getValue
            //setvalue
            void setArray (const array & value) override { m_value = value; }
            void addToArray (const Json & value) override{ m_value.emplace_back(value);}
            void removeFromArray (size_t index) override
            {
                if (index < m_value.size()) 
                {
                    m_value.erase(m_value.begin() + index);
                }
                else
                {
                    throw ghJsonException(std::string(__func__) + "index out of range!", 0);
                }
            }
            //setvalue
            //operator[]
            Json & operator[] (size_t index) override
            {
                if (index < m_value.size()) 
                {
                    return m_value[index];
                }
                else
                {
                    throw ghJsonException(std::string(__func__) + "index out of range!", 0);
                }
            }
            const Json & operator[] (size_t index) const override
            {
                if (index < m_value.size()) 
                {
                    return m_value[index];
                }
                else
                {
                    throw ghJsonException(std::string(__func__) + "index out of range!", 0);
                }
            }
            //operator[]
            //clone
            virtual std::unique_ptr<JsonValue> clone() const override{ return std::make_unique<JsonArray>(); }
            //clone
    };

    class JsonObject : public Value<JsonType::OBJECT, object>
    {
        public:
            explicit JsonObject(const object& value) : Value(value) {};
            explicit JsonObject(object&& value) : Value(move(value)) {};
        private:
            //getValue
            const object & getObject() const override { return m_value; }
            //getValue
            //setvalue
            void setObject (const object & value) override { m_value = value; }
            void addToObject(const std::string & key, const Json & value){ m_value[key] = value; }
            void removeFromObject (const std::string& key) override
            {
                auto iter = m_value.find(key);
                if(iter == m_value.end())
                {
                    throw ghJsonException(std::string(__func__) + " key :[" + key + "] not exits! ", 0);
                }
                else
                    m_value.erase(iter);
            }
            //setvalue
            //operator[]
            Json & operator[](const std::string& key) override { return m_value[key]; }
            const Json & operator[](const std::string& key) const override 
            { 
                auto iter = m_value.find(key);
                if(iter == m_value.end())
                {
                    throw ghJsonException(std::string(__func__) + " key :[" + key + "] not exits! ", 0);
                }
                else
                    return iter->second;
            }
            //operator[]
            //clone
            virtual std::unique_ptr<JsonValue> clone() const override{ return std::make_unique<JsonObject>(); }
            //clone
    };

    //Json
    //constructor
    Json::Json() noexcept                : m_ptr(std::make_unique<JsonNull>()) {}
    Json::Json(std::nullptr_t) noexcept  : m_ptr(std::make_unique<JsonNull>()) {}
    Json::Json(int value)                : m_ptr(std::make_unique<JsonNumber>(double(value))) {}
    Json::Json(double value)             : m_ptr(std::make_unique<JsonNumber>(value)) {}
    Json::Json(bool value)               : m_ptr(std::make_unique<JsonBool>(value)) {}
    Json::Json(const std::string &value) : m_ptr(std::make_unique<JsonString>(value)) {}
    Json::Json(std::string &&value)      : m_ptr(std::make_unique<JsonString>(move(value))) {}
    Json::Json(const char * value)       : m_ptr(std::make_unique<JsonString>(value)) {}
    Json::Json(const array &values)      : m_ptr(std::make_unique<JsonArray>(values)) {}
    Json::Json(array &&values)           : m_ptr(std::make_unique<JsonArray>(move(values))) {}
    Json::Json(const object &values)     : m_ptr(std::make_unique<JsonObject>(values)) {}
    Json::Json(object &&values)          : m_ptr(std::make_unique<JsonObject>(move(values))) {}

    Json::Json(const Json & other) : m_ptr(other.m_ptr ? other.m_ptr->clone() : std::make_unique<JsonNull>()) {}
    Json::Json(Json && other) noexcept : m_ptr(std::move(other.m_ptr)){}
    Json& Json::operator=(const Json& other) 
    {
        if (this != &other) // 防止自赋值
        { 
            m_ptr = other.m_ptr ? other.m_ptr->clone() : std::make_unique<JsonNull>(); //避免有nullptr
        }
        return *this;
    }
    Json& Json::operator=(Json&& other) noexcept 
    {
        if (this != &other) // 防止自赋值
        { 
            m_ptr = std::move(other.m_ptr) ; //避免有nullptr
        }
        return *this;
    }
    //constructor
    //type
    JsonType Json::type()      const { return m_ptr->type();              }
    bool     Json::is_null()   const { return type() == JsonType::NUL;    }
    bool     Json::is_number() const { return type() == JsonType::NUMBER; }
    bool     Json::is_bool()   const { return type() == JsonType::BOOL;   }
    bool     Json::is_string() const { return type() == JsonType::STRING; }
    bool     Json::is_array()  const { return type() == JsonType::ARRAY;  }
    bool     Json::is_object() const { return type() == JsonType::OBJECT; }
    //type
    //getValue
    double Json::getNumber() const { check(); return m_ptr->getNumber(); }
    bool Json::getBool() const { check(); return m_ptr->getBool(); }
    const std::string & Json::getString() const { check(); return m_ptr->getString(); }
    const array & Json::getArray()  const { check(); return m_ptr->getArray(); }
    const object & Json::getObject() const { check(); return m_ptr->getObject(); }
    //getValue
    //operator[]
    Json & Json::operator[](size_t i) { check(); return (*m_ptr)[i]; }
    Json & Json::operator[](const std::string &key) { check(); return (*m_ptr)[key]; }
    const Json & Json::operator[](size_t i) const { check(); return (*m_ptr)[i]; }
    const Json & Json::operator[](const std::string &key) const { check(); return (*m_ptr)[key]; }
    //operator[]
    //operator==
    bool Json::operator== (const Json &rhs) const
    {
        if(m_ptr == rhs.m_ptr)
            return true;
        else if(type() != rhs.type())
            return false;
        else
            return m_ptr->equals(rhs.m_ptr.get());
    }
    bool Json::operator< (const Json &rhs) const
    {
        if (m_ptr == rhs.m_ptr)
            return false;
        else if (m_ptr->type() != rhs.m_ptr->type())
            return m_ptr->type() < rhs.m_ptr->type();
        else
            return m_ptr->less(rhs.m_ptr.get());
    }
    //operator==
    //Json

}
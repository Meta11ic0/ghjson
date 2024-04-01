#include "ghjson.hpp"

namespace ghjson
{
    //JsonValue
    class JsonValue
    {
        public:            
            virtual JsonType Type() const = 0;
            
            virtual double              GetNumber() const;
            virtual bool                GetBool() const;
            virtual const std::string & GetString() const;
            virtual const array &       GetArray() const;
            virtual const object &      GetObject() const;

            virtual const Json& operator[](size_t i) const;
            virtual const Json& operator[](const std::string& key) const;

            virtual ~JsonValue(){};
    };

    template <JsonType tag, typename T>
    class Value : public JsonValue
    {
        protected:
            explicit Value(const T &value) : m_value(value) {}
            explicit Value(T &&value)      : m_value(move(value)) {}

            JsonType Type() const override { return tag; }
            const T m_value;
    };

    class NullClass
    {
    public:
        bool operator == (NullClass) { return true; }
        bool operator < (NullClass) { return false; }
    };

    class JsonNull final : public Value<JsonType::NUL, NullClass>
    {
        public:
            JsonNull() : Value({}) {};
    };

    class JsonNumber final : public Value<JsonType::NUMBER, double>
    {
        public:
            explicit JsonNumber(double value) : Value(value) {}
        private:
            double GetNumber() const override { return m_value; }
    };

    class JsonBool final : public Value<JsonType::BOOL, bool>
    {
        public:
            explicit JsonBool(bool value) : Value(value) {}
        private:
            bool GetBool() const override { return m_value; }
    };

    class JsonString final : public Value<JsonType::STRING, std::string>
    {
        public:
            explicit JsonString(const std::string& value) : Value(value) {}
            explicit JsonString(std::string&& value) : Value(move(value)) {}
        private:
            const std::string& GetString() const override { return m_value; }

    };

    class JsonArray final : public Value<JsonType::ARRAY, array>
    {
        public:
            explicit JsonArray(const array& value) : Value(value) {};
            explicit JsonArray(array&& value) : Value(move(value)) {};
        private:
            const array& GetArray() const override { return m_value; }
            const Json& operator[](size_t i) const;
    };

    class JsonObject final : public Value<JsonType::OBJECT, object>
    {
        public:
            explicit JsonObject(const object& value) : Value(value) {};
            explicit JsonObject(object&& value) : Value(move(value)) {};
        private:
            const object& GetObject() const override { return m_value; }
            const Json& operator[](const std::string& key) const;
    };
    //JsonValue

    //Static Value
    class Default_JsonValue
    {
        public:
            const std::shared_ptr<JsonValue> null = std::make_shared<JsonNull>();
            const std::shared_ptr<JsonValue> t    = std::make_shared<JsonBool>(true);
            const std::shared_ptr<JsonValue> f    = std::make_shared<JsonBool>(false);
            const std::string empty_string;
            const array       empty_vector;
            const object      empty_map;
            Default_JsonValue(){};
    };

    static const Default_JsonValue & GetDefaultJsonValue()
    {
        static const Default_JsonValue d{};
        return d;
    };

    static const Json & GetJsonNull()
    {
        static const Json jsonnull; //会调用Json()，从而要使用GetDefaultJsonValue().null，所以将Json默认值跟JsonValue默认值分隔开，不然会有循环依赖
        return jsonnull;
    };
    //Static Value

    //GetType
    JsonType Json::Type() const { return m_ptr->Type(); }
    //GetType

    //GetValue
    //Json
    double              Json::GetNumber() const { return m_ptr->GetNumber(); }
    bool                Json::GetBool()   const { return m_ptr->GetBool();   }
    const std::string & Json::GetString() const { return m_ptr->GetString(); }
    const array &       Json::GetArray()  const { return m_ptr->GetArray();  }
    const object &      Json::GetObject() const { return m_ptr->GetObject();  }
    //JsonValue
    double              JsonValue::GetNumber() const { return 0;                                  }
    bool                JsonValue::GetBool()   const { return false;                              }
    const std::string & JsonValue::GetString() const { return GetDefaultJsonValue().empty_string; }
    const array &       JsonValue::GetArray()  const { return GetDefaultJsonValue().empty_vector; }
    const object &      JsonValue::GetObject() const { return GetDefaultJsonValue().empty_map;    }
    //operator[]
    const Json & Json::operator[] (size_t i)                 const { return (*m_ptr)[i];   }
    const Json & Json::operator[] (const std::string &key)   const { return (*m_ptr)[key]; }
    const Json & JsonValue::operator[] (size_t)              const { return GetJsonNull(); }
    const Json & JsonValue::operator[] (const std::string &) const { return GetJsonNull(); }
    //特化的operator[]
    const Json & JsonArray::operator[] (size_t i) const 
    { 
        return (i > m_value.size() ? GetJsonNull() : m_value[i]);
    }
    const Json & JsonObject::operator[] (const std::string & key) const 
    { 
        auto iter = m_value.find(key);
        return (iter == m_value.end() ? GetJsonNull() : iter->second); 
    }
    //GetValue

    //Comparisons
    bool Json::operator== (const Json &rhs) const
    {
        if(this->Type() != rhs.Type())
            return false;
        
    }

    bool Json::operator<  (const Json &rhs) const
    {

    }
    //Comparisons

    //Construtor
    Json::Json() noexcept                  : m_ptr(GetDefaultJsonValue().null) {}
    Json::Json(std::nullptr_t) noexcept    : m_ptr(GetDefaultJsonValue().null) {}
    Json::Json(bool value)                 : m_ptr(value ? GetDefaultJsonValue().t : GetDefaultJsonValue().f) {}
    //Construtor
}
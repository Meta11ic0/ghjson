#include "ghjson.hpp"

namespace ghjson
{

    using std::string;
    using std::shared_ptr;
    using std::make_shared;
    /* * * * * * * * * * * * * * * * * * * */    
    /* Helper for representing null - just a do-nothing struct, plus comparison
    /* operators so the helpers in JsonValue work. We can't use nullptr_t because
    /* it may not be orderable.* * * * * * */
    class NullClass
    {
        public:
            bool operator == (NullClass) {return true;}
            bool operator < (NullClass) {return false;}
    };
    /* * * * * * * * * * * * * * * * * * * */
   
    template<JsonType tag, typename T>
    class Value : public JsonValue
    {
        protected:
            explicit Value(const T &value) : m_value(value) {};
            explicit Value(T &&value)      : m_value(move(value)) {};
            // Get type tag
            const JsonType type() const override 
            {
                return tag;
            }
            // Comparisons 因为是JsonValue的指针
            //所以需要用static_cast转换成Value类型指针才能访问m_value
            bool equals(const JsonValue * other) const override 
            {
                return m_value == static_cast<const Value<tag, T> *>(other)->m_value;
            }
            bool less(const JsonValue * other) const override 
            {
                return m_value <  static_cast<const Value<tag, T> *>(other)->m_value;
            }   
            const T m_value;
    };

    class JsonDouble final : public Value<JsonType::NUMBER, double> 
    {
        public:
            explicit JsonDouble(double value) : Value(value) {}
        private:
            double number_value() const override { return m_value; }
            int int_value() const override { return static_cast<int>(m_value); }
            bool equals(const JsonValue * other) const override { return m_value == other->number_value(); }
            bool less(const JsonValue * other)   const override { return m_value <  other->number_value(); }

    };

    class JsonInt final : public Value<JsonType::NUMBER, int> 
    {
        public:
            explicit JsonInt(int value) : Value(value) {}
        private:
            double number_value() const override { return m_value; }
            int int_value() const override { return m_value; }
            bool equals(const JsonValue * other) const override { return m_value == other->number_value(); }
            bool less(const JsonValue * other)   const override { return m_value <  other->number_value(); }
    };

    class JsonBoolean final : public Value<JsonType::BOOL, bool>
    {
        public:
            explicit JsonBoolean(bool value) : Value(value){}
        private:
            bool bool_value() const override {return m_value;}
    };

    class JsonString final : public Value<JsonType::STRING, string>
    {
        public:
            explicit JsonString(const string &value) : Value(value) {}
            explicit JsonString(string &&value) : Value(move(value)) {}
        private:
            const string &string_value() const override { return m_value;}

    };

    class JsonArray final : public Value<JsonType::ARRAY, array>
    {
        public:
            explicit JsonArray(const array &value) : Value(value) {};
            explicit JsonArray(array &&value) : Value(move(value)) {};
        private:
            const array & array_items() const override { return m_value; }
            const Json & operator[](size_t i) const override;
    };

    class JsonObject final : public Value<JsonType::OBJECT, object>
    {
        public:
            explicit JsonObject(const object &value) : Value(value) {};
            explicit JsonObject(object &&value) : Value(move(value)) {};
        private:
            const object & object_items() const override { return m_value; }
            const Json & operator[](const string & key) const override;
    };

    class JsonNull final : public Value<JsonType::NUL, NullClass> 
    {
        public:
            JsonNull() : Value({}) {};
    };

    /* Static globals - static-init-safe * */
    class Static_JsonValues
    {
        public:
            const shared_ptr<JsonValue> null = make_shared<JsonNull>();
            const shared_ptr<JsonValue> t    = make_shared<JsonBoolean>(true);
            const shared_ptr<JsonValue> f    = make_shared<JsonBoolean>(false);
            const string                       empty_string;
            const array                empty_vector;
            const object           empty_map;
            Static_JsonValues(){};
    };
    static const Static_JsonValues & Get_Static_JsonValues() 
    {
        static const Static_JsonValues s {};
        return s;
    }
    static const Json & Get_Static_Json_Null()
    {
        static const Json json_null;
        return json_null;
    }
    //我们将Get_Static_Json_Null函数单独设置出来，
    //以确保在第一次需要一个类型为Json且表示null的对象时安全地创建它。
    //我们避免的问题是Json构造函数依赖犹未构建完成的Static_JsonValues().null，
    //于是设计了一种方法，可以在不引起依赖问题的情况下，安全并按需初始化全局Json null对象。
    /* * * * * * * * * * * * * * * * * * * */      

    /* class Json* * * * * * * * * * * * * */
    /* Constructors* * * * * * * * * * * * */
    Json::Json() noexcept               : m_ptr(Get_Static_JsonValues().null) {}
    Json::Json(std::nullptr_t) noexcept : m_ptr(Get_Static_JsonValues().null) {}
    Json::Json(double value)            : m_ptr(make_shared<JsonDouble>(value)) {}
    Json::Json(int value)               : m_ptr(make_shared<JsonInt>(value)) {}
    Json::Json(bool value)              : m_ptr(value ? Get_Static_JsonValues().t : Get_Static_JsonValues().f) {}
    Json::Json(const string &value)     : m_ptr(make_shared<JsonString>(value)) {}
    Json::Json(string &&value)          : m_ptr(make_shared<JsonString>(move(value))) {}
    Json::Json(const char * value)      : m_ptr(make_shared<JsonString>(value)) {}
    Json::Json(const array &values)     : m_ptr(make_shared<JsonArray>(values)) {}
    Json::Json(array &&values)          : m_ptr(make_shared<JsonArray>(move(values))) {}
    Json::Json(const object &values)    : m_ptr(make_shared<JsonObject>(values)) {}
    Json::Json(object &&values)         : m_ptr(make_shared<JsonObject>(move(values))) {}

    JsonType       Json::type()                           const { return m_ptr->type();         }
    double         Json::number_value()                   const { return m_ptr->number_value(); }
    int            Json::int_value()                      const { return m_ptr->number_value(); }
    bool           Json::bool_value()                     const { return m_ptr->bool_value();   }
    const          string & Json::string_value()          const { return m_ptr->string_value(); }
    const array &  Json::array_items()                    const { return m_ptr->array_items();  }
    const object & Json::object_items()                   const { return m_ptr->object_items(); }
    const Json &   Json::operator[] (size_t i)            const { return (*m_ptr)[i];           }
    const Json &   Json::operator[] (const string &key)   const { return (*m_ptr)[key];         }

    /* Comparison */
    bool Json::operator== (const Json &other) const 
    {
        if (m_ptr == other.m_ptr)
            return true;
        if (m_ptr->type() != other.m_ptr->type())
            return false;

        return m_ptr->equals(other.m_ptr.get());
    }

    bool Json::operator< (const Json &other) const 
    {
        if (m_ptr == other.m_ptr)
            return false;
        if (m_ptr->type() != other.m_ptr->type())
            return m_ptr->type() < other.m_ptr->type();

        return m_ptr->less(other.m_ptr.get());
    }

    /* class JsonValue * * * * * * * * * * */
    double         JsonValue::number_value()              const { return 0; }
    int            JsonValue::int_value()                 const { return 0; }
    bool           JsonValue::bool_value()                const { return false; }
    const string & JsonValue::string_value()              const { return Get_Static_JsonValues().empty_string; }
    const array &  JsonValue::array_items()               const { return Get_Static_JsonValues().empty_vector; }
    const object & JsonValue::object_items()              const { return Get_Static_JsonValues().empty_map; }
    const Json &   JsonValue::operator[] (size_t)         const { return Get_Static_Json_Null(); }
    const Json &   JsonValue::operator[] (const string &) const { return Get_Static_Json_Null(); } 
    /* class JsonArray * * * * * * * * * * */
    const Json & JsonArray::operator[] (size_t i) const
    {
        if(i >= m_value.size())
        {
            return Get_Static_Json_Null();
        }
        else
            return m_value[i];
    }
    /* class JsonObject * * * * * * * * * */
    const Json & JsonObject::operator[] (const string & key) const
    {
        auto iter = m_value.find(key);
        return (iter == m_value.end()) ? Get_Static_Json_Null() : iter->second;
    }   
} 

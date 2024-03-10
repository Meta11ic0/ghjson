#include "ghjson.hpp"

namespace ghjson
{
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

    class JsonNumber final : public Value<JsonType::NUMBER, double>
    {
        public:
            explicit JsonNumber(double value) : Value(value) {}
            explicit JsonNumber(int value) : Value(value) {}
        private:
            const double number_value() const override { return m_value; }
            const int int_value() const override { return m_value; }
            bool equals(const JsonValue * other) const override { return m_value == other->number_value(); }
            bool less(const JsonValue * other)   const override { return m_value <  other->number_value(); }
    };

    class JsonBoolean final : public Value<JsonType::BOOL, bool>
    {
        public:
            explicit JsonBoolean(bool value) : Value(value){}
        private:
            const bool bool_value() const override {return m_value;}
    };

    class JsonString final : public Value<JsonType::STRING, string>
    {
        public:
            explicit JsonString(string &&value) : Value(move(value)) {}
        private:
            const string &string_value() const override { return m_value;}

    };

    class JsonArray final : public Value<JsonType::ARRAY, ghjson::array>
    {

    };

    class JsonObject final : public Value<JsonType::OBJECT, ghjson::object>
    {

    };

    class JsonNull final : public Value<JsonType::NUL, NullClass> 
    {
        public:
            JsonNull() : Value({}) {};
    };

    /* * * * * * * * * * * * * * * * * * * */
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
    /* Constructors*/
    Json::Json() noexcept                  : m_ptr(Get_Static_JsonValues().null) {}
    Json::Json(std::nullptr_t) noexcept    : m_ptr(Get_Static_JsonValues().null) {}
    Json::Json(double value)               : m_ptr(make_shared<JsonNumber>(value)) {}
    Json::Json(int value)                  : m_ptr(make_shared<JsonNumber>(value)) {}
    Json::Json(bool value)                 : m_ptr(value ? Get_Static_JsonValues().t : Get_Static_JsonValues().f) {}
    Json::Json(const string &value)        : m_ptr(make_shared<JsonString>(value)) {}
    Json::Json(string &&value)             : m_ptr(make_shared<JsonString>(move(value))) {}
    Json::Json(const char * value)         : m_ptr(make_shared<JsonString>(value)) {}
    Json::Json(const array &values)        : m_ptr(make_shared<JsonArray>(values)) {}
    Json::Json(array &&values)             : m_ptr(make_shared<JsonArray>(move(values))) {}
    Json::Json(const object &values)       : m_ptr(make_shared<JsonObject>(values)) {}
    Json::Json(object &&values)            : m_ptr(make_shared<JsonObject>(move(values))) {}


    const JsonType Json::type()                           const{return m_ptr->type();        }
    const double   Json::number_value()                   const{return m_ptr->number_value();}
    const int      Json::int_value()                      const{return m_ptr->number_value();}
    const bool     Json::bool_value()                     const{return m_ptr->bool_value();  }
    const const    string & Json::string_value()          const{return m_ptr->string_value();}
    const array &  Json::array_items()                    const{return m_ptr->array_items(); }
    const object & Json::object_items()                   const{return m_ptr->object_items();}
    /* * * * * * * * * * * * * * * * * * * *
    * class JsonValue * * * * * * * * * * *
    * * * * * * * * * * * * * * * * * * * */ 
    const double   JsonValue::number_value()              const { return 0; }
    const int      JsonValue::int_value()                 const { return 0; }
    const bool     JsonValue::bool_value()                const { return false; }
    const string & JsonValue::string_value()              const { return Get_Static_JsonValues().empty_string; }
    const array &  JsonValue::array_items()               const { return Get_Static_JsonValues().empty_vector; }
    const object & JsonValue::object_items()              const { return Get_Static_JsonValues().empty_map; }
    const Json &   JsonValue::operator[] (size_t)         const { return Get_Static_Json_Null(); }
    const Json &   JsonValue::operator[] (const string &) const { return Get_Static_Json_Null(); }    
} 

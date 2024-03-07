#include "ghjson.hpp"

namespace ghjson
{
    /* * * * * * * * * * * * * * * * * * * */    
    /* Helper for representing null - just a do-nothing struct, plus comparison
     * operators so the helpers in JsonValue work. We can't use nullptr_t because
     * it may not be orderable.
     */
    struct NullStruct{};
    /* * * * * * * * * * * * * * * * * * * */
   
    template<JsonType tag, typename T>
    class Value : public JsonValue
    {
        protected:
            explicit Value(const T &value) : m_value(value) {};
            explicit Value(T &&value)      : m_value(move(value)) {};
            // Get type tag
            JsonType type() const override 
            {
                return tag;
            }

            const T m_value;
    };

    class JsonNull final : public Value<JsonType::NUL, NullStruct> 
    {
        public:
            JsonNull() : Value({}) {};
    };

    class JsonNumber : public Value<JsonType::NUMBER, double>
    {

    };

    class JsonBool : public Value<JsonType::BOOL, bool>
    {

    };

    class JsonString : public Value<JsonType::STRING, string>
    {

    };

    class JsonArray : public Value<JsonType::ARRAY, ghjson::array>
    {

    };

    class JsonObject: public Value<JsonType::OBJECT, ghjson::object>
    {

    };

    /* * * * * * * * * * * * * * * * * * * */
    /* Static globals - static-init-safe * */

    class Statics
    {
        public:
            std::shared_ptr<JsonValue> null = make_shared<JsonNull>();
            std::shared_ptr<JsonValue> t = make_shared<JsonBoolean>(true);
            std::shared_ptr<JsonValue> f = make_shared<JsonBoolean>(false);
            string empty_string;
            vector<Json> empty_vector;
            map<string, Json> empty_map;
            Statics(){};
    };
    /* * * * * * * * * * * * * * * * * * * */      

    /* * * * * * * * * * * * * * * * * * * *
    * class Json* * * * * * * * * * * * * *
    * * * * * * * * * * * * * * * * * * * */ 
    const JsonType Json::type()                  const{return m_ptr->type();        }
    const double   Json::number_value()          const{return m_ptr->number_value();}
    const bool     Json::bool_value()            const{return m_ptr->bool_value();  }
    const const    string & Json::string_value() const{return m_ptr->string_value();}
    const array &  Json::array_items()           const{return m_ptr->array_items(); }
    const object & Json::object_items()          const{return m_ptr->object_items();}
    /* * * * * * * * * * * * * * * * * * * *
    * class JsonValue * * * * * * * * * * *
    * * * * * * * * * * * * * * * * * * * */ 
    double         JsonValue::number_value()              const { return 0; }
    int            JsonValue::int_value()                 const { return 0; }
    bool           JsonValue::bool_value()                const { return false; }
    const string & JsonValue::string_value()              const { return statics().empty_string; }
    const array &  JsonValue::array_items()               const { return statics().empty_vector; }
    const object & JsonValue::object_items()              const { return statics().empty_map; }
    const Json &   JsonValue::operator[] (size_t)         const { return static_null(); }
    const Json &   JsonValue::operator[] (const string &) const { return static_null(); }    
} 

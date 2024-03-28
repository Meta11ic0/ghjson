#include "ghjson.hpp"

namespace ghjson
{
    class JsonValue
    {
        //..
        public:            
            virtual const JsonType type() const = 0;
            
            virtual double              GetNumber() const;
            virtual bool                GetBool() const;
            virtual const std::string & GetString() const;
            virtual const array &       GetArray() const;
            virtual const object &      GetObject() const;

            virtual const Json& operator[](size_t i) const;
            virtual const Json& operator[](const std::string& key) const;

            virtual ~JsonValue();
        //..
    };

    
    template <JsonType tag, typename T>
    class Value : public JsonValue
    {
        protected:
            explicit Value(const T &value) : m_value(value) {}
            explicit Value(T &&value)      : m_value(move(value)) {}

            JsonType Type() const override { return tag; }
            const T m_value;
    }
}
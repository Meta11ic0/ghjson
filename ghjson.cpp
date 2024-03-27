#include "ghjson.hpp"

namespace ghjson
{
    template<JsonType tag, typename T>
    class JsonValue
    {
        //..
        public:         
            const JsonType type() const { return tag; }

            virtual double GetNumber() const;
            virtual bool GetBool() const;
            virtual const std::string& GetString() const;
            virtual const array& GetArray() const;
            virtual const Json& operator[](size_t i) const;
            virtual const object& GetObject() const;
            virtual const Json& operator[](const std::string& key) const;

            virtual const std::string Dump() const = 0; 

            virtual ~JsonValue();
        private:
            const T m_value;
        //..
    };
}
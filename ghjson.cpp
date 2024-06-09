#include "ghjson.hpp"

namespace ghjson
{

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
            void dump(std::string &out) const { out += "null"; }
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
            virtual std::unique_ptr<JsonValue> clone() const override{ return std::make_unique<JsonBool>(m_value); }
            //clone
            //dump
            void dump(std::string &out) const { out += (m_value ? "true" : "false"); }
            //dump
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
            virtual std::unique_ptr<JsonValue> clone() const override{ return std::make_unique<JsonNumber>(m_value); }
            //clone
            //dump
            void dump(std::string &out) const { out += std::to_string(m_value); }
            //dump
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
            virtual std::unique_ptr<JsonValue> clone() const override{ return std::make_unique<JsonString>(m_value); }
            //clone
            //dump
            void dump(std::string &out) const { out += "\"" + m_value + "\""; }
            //dump
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
            virtual std::unique_ptr<JsonValue> clone() const override{ return std::make_unique<JsonArray>(m_value); }
            //clone
            //dump
            void dump(std::string &out) const 
            {
                out += '[';
                bool first = true;
                for (const auto& item : m_value)
                {
                    if (!first)
                    {
                        out += ",\n";
                    }
                    first = false;
                    item.dump(out);
                }
                out += ']';
            }
            //dump
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
            virtual std::unique_ptr<JsonValue> clone() const override{ return std::make_unique<JsonObject>(m_value); }
            //clone
            //dump
            void dump(std::string &out) const 
            {
                out += "{\n";
                bool first = true;
                for (const auto& item : m_value)
                {
                    if (!first)
                    {
                        out += ",\n";
                    }
                    first = false;
                    out += '\t' + item.first + " : ";
                    item.second.dump(out);
                }
                out += "\n}";
            }
            //dump
    };
    //JsonValue

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
    //setValue
    void Json::setNumber(double value) { check(); return m_ptr->setNumber(value); }
    void Json::setBool(bool value) { check(); return m_ptr->setBool(value); }
    void Json::setString(const std::string & value) { check(); return m_ptr->setString(value); }
    void Json::setArray(const array & value) { check(); return m_ptr->setArray(value); }
    void Json::setObject(const object & value) { check(); return m_ptr->setObject(value); }

    void Json::addToArray (const Json & value) { check(); return m_ptr->addToArray(value); }
    void Json::addToObject(const std::string & key, const Json & value) { check(); return m_ptr->addToObject(key, value); }
    void Json::removeFromArray(size_t index) { check(); return m_ptr->removeFromArray(index); }
    void Json::removeFromObject(const std::string& key) { check(); return m_ptr->removeFromObject(key); }
    //setValue
    //operator[]
    Json & Json::operator[](size_t i) { check(); return (*m_ptr)[i]; }
    Json & Json::operator[](const std::string &key) { check(); return (*m_ptr)[key]; }
    const Json & Json::operator[](size_t i) const { check(); return (*m_ptr)[i]; }
    const Json & Json::operator[](const std::string &key) const { check(); return (*m_ptr)[key]; }
    //operator[]
    //operator==
    bool Json::operator== (const Json &rhs) const
    {
        check();
        if(m_ptr == rhs.m_ptr)
            return true;
        else if(type() != rhs.type())
            return false;
        else
            return m_ptr->equals(rhs.m_ptr.get());
    }
    bool Json::operator< (const Json &rhs) const
    {
        check();
        if (m_ptr == rhs.m_ptr)
            return false;
        else if (m_ptr->type() != rhs.m_ptr->type())
            return m_ptr->type() < rhs.m_ptr->type();
        else
            return m_ptr->less(rhs.m_ptr.get());
    }
    //operator==
    //dump
    void Json::dump(std::string &out) const { check();m_ptr->dump(out); }
    //Json
    //parse
    void checkIndex(const std::string& str, size_t idx) 
    {
        if (idx >= str.size()) 
        {
            throw ghJsonException("Unexpected end", idx);
        }
    }

    //提前声明为了调用
    Json parseJson(const std::string & str, size_t & idx, size_t depth);
    Json parseString(const std::string & str, size_t & idx);
    
    void parseWhitespace(const std::string& str, size_t & idx)
    {
        while (idx < str.size() && (str[idx] == ' ' || str[idx] == '\r' || str[idx] == '\n' || str[idx] == '\t'))
            idx++;
    }

    Json parseObject(const std::string & str, size_t & idx, size_t depth) 
    {
        object out;

        parseWhitespace(str, idx);
        checkIndex(str, idx);

        if(str[idx] == '}')
            return Json(out);

        while(1)
        {
            try
            {
                parseWhitespace(str, idx);
                checkIndex(str, idx);

                std::string key = parseString(str, idx).getString();

                parseWhitespace(str, idx);
                checkIndex(str, idx);
                
                if(str[idx]!=':')
                    throw ghJsonException("[ERROR]: object parsing, expect':', got " + str[idx], idx);
                idx++;
                
                parseWhitespace(str, idx);
                checkIndex(str, idx);
                Json value  = parseJson(str, idx, depth);
                out.emplace(key, value);
            }
            catch(const ghJsonException& ex)
            {
                throw ghJsonException("[ERROR]: object parse worng, " + std::string(ex.what()) , idx);
            }
            parseWhitespace(str, idx);
            checkIndex(str, idx);
            if(str[idx] == '}')
                break;
            if(str[idx] != ',')
                throw ghJsonException("[ERROR]: object format worng, ", idx);
            idx++;
        }
        idx++;
        return Json(out);
    }

    Json parseArray(const std::string & str, size_t & idx, size_t depth) 
    {
        array out;
        parseWhitespace(str, idx);
        checkIndex(str, idx);
        if(str[idx] == ']')
            return Json(out);

        while(1)
        {
            try
            {
                Json test = parseJson(str, idx, depth);
                out.emplace_back(std::move(test));
            }
            catch(const ghJsonException& ex)
            {
                throw ghJsonException("[ERROR]: array parse worng, " + std::string(ex.what()) , idx);
            }
            parseWhitespace(str, idx);
            checkIndex(str, idx);
            if(str[idx] == ']')
                break;
            if(str[idx] != ',')
                throw ghJsonException("[ERROR]: array format worng, ", idx);
            idx++;
        }
        idx++;
        return Json(out);
    }

    Json parseString(const std::string & str, size_t & idx) 
    {
        std::string out;
        idx++;
        while(1)
        {
            if(idx == str.size())
            {
                throw ghJsonException("Unexpected end", idx);
            }
            if(str[idx] == '\"')
                break;
            else if(str[idx] == '\\')
            {
                idx++;
                switch(str[idx])
                {
                    case '\"':  out+='\"'; break;
                    case '\\':  out+='\\'; break;
                    case '/' :  out+='/' ; break;
                    case 'b' :  out+='\b'; break;
                    case 'f' :  out+='\f'; break;
                    case 'n' :  out+='\n'; break;
                    case 'r' :  out+='\r'; break;
                    case 't' :  out+='\t'; break;
                    default : throw ghJsonException("unknow sequence:" + std::string(1, str[idx-1]) + std::string(1, str[idx]), idx);
                }
            }
            else
            {
                out+=str[idx];
            }
            idx++;
        }
        idx++;
        return Json(out);
    }

    /*oldversion
    bool InRange(long x, long lower, long upper)
    {
        return (x >= lower && x <= upper);
    }

    Json parseNumber(const std::string & str, size_t & idx)
    {

        size_t start = idx;
        size_t len = str.length();
        
        if(str[idx] == '-')
        {
            idx++;
        }

        if(idx < len && str[idx] == '0')
        {
            idx++;
        }
        else if(idx < len && InRange(str[idx], '1', '9'))
        {
            idx++;
            while(idx < len && InRange(str[idx], '0', '9'))
            {
                idx++;
            }
        }
        else
        {
            throw ghJsonException("[ERROR]:wrong number format", idx);
        }

        if(idx < len && str[idx] == '.')
        {
            idx++;
            if(idx < len && InRange(str[idx], '0', '9'))
            {
                idx++;
            }
            else
            {
                throw ghJsonException("[ERROR]:wrong number format", idx);
            }
            
            while(idx < len && InRange(str[idx], '0', '9'))
            {
                idx++;
            }
        }
        
        if(idx < len && (str[idx] == 'e' || str[idx] == 'E'))
        {
            idx++;
            if(idx < len && (str[idx] == '+' || str[idx] == '-'))
            {
                idx++;
            }

            if(idx < len && InRange(str[idx], '0', '9'))
            {
                idx++;
            }
            else
            {
                throw ghJsonException("[ERROR]:wrong number format", idx);
            }

            while(idx < len && InRange(str[idx], '0', '9'))
            {
                idx++;
            }
        }

        return Json(std::strtod(str.c_str() + start, nullptr));
    }
    */
    Json parseNumber(const std::string &str, size_t &idx) 
    {
        size_t processed = 0;
        double value = std::stod(str.substr(idx), &processed);
        if (processed == 0) {
            throw ghJsonException("Invalid number format", idx);
        }
        idx += processed;
        return Json(value);
    }

    Json parseLiteral(const std::string &literal, Json target, const std::string & str, size_t & idx) 
    {
        if(str.compare(idx, literal.length(), literal) == 0)
        {
            idx += literal.length();
            return target;
        }
        else
            throw ghJsonException("[ERROR]:expected (" + literal + "), got (" + str.substr(idx, literal.length()) + ")", idx);
    }

    Json parseJson(const std::string & str, size_t & idx, size_t depth)
    {
        if(depth > MAXDEPTH)
        {
            throw ghJsonException("exceeded maximum nesting depth", 0);
        }
        parseWhitespace(str, idx);
        checkIndex(str, idx);

        if(str[idx] == 'n')
        {
            return parseLiteral("null", Json(), str, idx);
        }
        else if(str[idx] == 't')
        {
            return parseLiteral("true", Json(true), str, idx);
        }
        else if(str[idx] == 'f')
        {
            return parseLiteral("false", Json(false), str, idx);
        }
        else if(str[idx] == '\"')
        {
            return parseString(str, idx);
        }
        else if(str[idx] == '[')
        {
            return parseArray(str, ++idx, ++depth);
        }
        else if(str[idx] == '{')
        {
            return parseObject(str, ++idx, ++depth);
        }
        else
        {
            return parseNumber(str, idx);
        }
    }
    
    Json parse(const std::string & in)
    {
        size_t idx = 0;
        size_t depth = 0;
        return parseJson(in, idx, depth);
    }
    //parse
}
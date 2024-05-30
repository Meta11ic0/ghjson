#include "ghjson.hpp"

namespace ghjson
{
    //JsonValue
    class JsonValue
    {
        public:            
            virtual JsonType Type() const = 0;

            virtual bool equals(const JsonValue * other) const = 0;
            virtual bool less  (const JsonValue * other) const = 0;

            virtual void Dump(std::string &out) const = 0;
            
            virtual double              GetNumber() const;
            virtual bool                GetBool  () const;
            virtual const std::string & GetString() const;
            virtual const array &       GetArray () const;
            virtual const object &      GetObject() const;

            virtual const Json& operator[](size_t i) const;
            virtual const Json& operator[](const std::string& key) const;

            virtual void SetNumber (double               value);
            virtual void SetBool   (bool                 value);
            virtual void SetString (const  std::string & value);
            virtual void SetArray  (const  array       & value);
            virtual void SetObject (const  object      & value);

            virtual void AddToArray (const Json & value);
            virtual void AddToObject(const std::string & key, const Json & value);

            virtual ~JsonValue(){};
    };

    template <JsonType tag, typename T>
    class Value : public JsonValue
    {
        protected:
            explicit Value(const T &value) : m_value(value) {}
            explicit Value(T &&value)      : m_value(std::move(value)) {}

            JsonType Type() const override { return tag; }
            T m_value;

            void Dump(std::string &out) const;

            //Comparisons
            bool equals(const JsonValue * other) const {
                return m_value == static_cast<const Value<tag, T> *>(other)->m_value;
            }
            bool less(const JsonValue * other) const {
                return m_value < static_cast<const Value<tag, T> *>(other)->m_value;
            }
            //Comparisons

    };

    class NullClass
    {
    public:
        bool operator == (NullClass) const { return true; }
        bool operator < (NullClass) const { return false; }
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
            double GetNumber()             const override { return m_value; }
            void   SetNumber(double value) override       { m_value = value;}
    };

    class JsonBool final : public Value<JsonType::BOOL, bool>
    {
        public:
            explicit JsonBool(bool value) : Value(value) {}
        private:
            bool GetBool() const override     { return m_value; }
            void SetBool(bool value) override { m_value = value;}
    };

    class JsonString final : public Value<JsonType::STRING, std::string>
    {
        public:
            explicit JsonString(const std::string& value) : Value(value) {}
            explicit JsonString(std::string&& value) : Value(move(value)) {}
        private:
            const std::string& GetString() const override { return m_value; }
            void SetString(const std::string& value) override { m_value = value;}
    };

    class JsonArray final : public Value<JsonType::ARRAY, array>
    {
        public:
            explicit JsonArray(const array& value) : Value(value) {};
            explicit JsonArray(array&& value) : Value(move(value)) {};
        private:
            const array& GetArray() const override { return m_value; }
            const Json& operator[](size_t i) const;

            void SetArray(const array & value) override { m_value = value;}
            void AddToArray(const Json & value) override { m_value.push_back(value); }

            void RemoveFromArray(size_t index) 
            {
                if (index < m_value.size()) 
                {
                    m_value.erase(m_value.begin() + index);
                } 
                else 
                {
                    throw ghJsonException("Index out of bounds", index);
                }
            }
    };

    class JsonObject final : public Value<JsonType::OBJECT, object>
    {
        public:
            explicit JsonObject(const object& value) : Value(value) {};
            explicit JsonObject(object&& value) : Value(move(value)) {};
        private:
            const object& GetObject() const override { return m_value; }
            const Json& operator[](const std::string& key) const;

            void SetObject (const  object      & value) override { m_value = value;}
            void AddToObject(const std::string & key, const Json & value) override { m_value[key] = value; }

            void RemoveFromObject(const std::string& key) 
            {
                if (m_value.find(key) != m_value.end()) 
                {
                    m_value.erase(key);
                } 
                else 
                {
                    throw ghJsonException("Key not found", 0); // 0 as a placeholder
                }
            }
    };
    //JsonValue

    //Static Value
    class DefaultValue
    {
        public:
            const std::shared_ptr<JsonValue> null = std::make_shared<JsonNull>();
            const std::shared_ptr<JsonValue> t    = std::make_shared<JsonBool>(true);
            const std::shared_ptr<JsonValue> f    = std::make_shared<JsonBool>(false);
            const std::string empty_string;
            const array       empty_vector;
            const object      empty_map;
            DefaultValue(){};
    };

    static const DefaultValue & GetDefaultValue()
    {
        static const DefaultValue d{};
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
    const std::string & JsonValue::GetString() const { return GetDefaultValue().empty_string; }
    const array &       JsonValue::GetArray()  const { return GetDefaultValue().empty_vector; }
    const object &      JsonValue::GetObject() const { return GetDefaultValue().empty_map;    }
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

    //SetValue
    void Json::SetNumber(double               value) { m_ptr->SetNumber(value); }
    void Json::SetBool  (bool                 value) { m_ptr->SetBool(value);   }
    void Json::SetString(const  std::string & value) { m_ptr->SetString(value); }
    void Json::SetArray (const  array       & value) { m_ptr->SetArray(value);  }
    void Json::SetObject(const  object      & value) { m_ptr->SetObject(value); }

    void Json::AddToArray (const Json & value)                          { m_ptr->AddToArray(value);       }
    void Json::AddToObject(const std::string & key, const Json & value) { m_ptr->AddToObject(key, value); }

    void JsonValue::SetNumber(double              value) { throw ghJsonException("Invalid type", 0); }
    void JsonValue::SetBool  (bool                value) { throw ghJsonException("Invalid type", 0); }
    void JsonValue::SetString(const std::string & value) { throw ghJsonException("Invalid type", 0); }
    void JsonValue::SetArray (const array       & value) { throw ghJsonException("Invalid type", 0); }
    void JsonValue::SetObject(const object      & value) { throw ghJsonException("Invalid type", 0); }

    void JsonValue::AddToArray (const Json & value)                          { throw ghJsonException("Invalid type", 0); }
    void JsonValue::AddToObject(const std::string & key, const Json & value) { throw ghJsonException("Invalid type", 0); }
    //SetValue

    //Comparisons
    bool Json::operator== (const Json &rhs) const
    {
        if(m_ptr == rhs.m_ptr)
            return true;
        else if(m_ptr->Type() != rhs.Type())
            return false;
        
        //没法直接return m_ptr->m_value == rhs.m_value;
        return m_ptr->equals(rhs.m_ptr.get());     
    }

    bool Json::operator<  (const Json &rhs) const
    {
        if (m_ptr == rhs.m_ptr)
            return false;
        if (m_ptr->Type() != rhs.m_ptr->Type())
            return m_ptr->Type() < rhs.m_ptr->Type();

        return m_ptr->less(rhs.m_ptr.get());
    }
    //Comparisons

    //Json Constructor
    Json::Json() noexcept                : m_ptr(GetDefaultValue().null) {}
    Json::Json(std::nullptr_t) noexcept  : m_ptr(GetDefaultValue().null) {}
    Json::Json(int value)                : m_ptr(std::make_shared<JsonNumber>(value)) {}
    Json::Json(double value)             : m_ptr(std::make_shared<JsonNumber>(value)) {}
    Json::Json(bool value)               : m_ptr(value ? GetDefaultValue().t : GetDefaultValue().f) {}
    Json::Json(const std::string &value) : m_ptr(std::make_shared<JsonString>(value)) {}
    Json::Json(std::string &&value)      : m_ptr(std::make_shared<JsonString>(move(value))) {}
    Json::Json(const char * value)       : m_ptr(std::make_shared<JsonString>(value)) {}
    Json::Json(const array &values)      : m_ptr(std::make_shared<JsonArray>(values)) {}
    Json::Json(array &&values)           : m_ptr(std::make_shared<JsonArray>(move(values))) {}
    Json::Json(const object &values)     : m_ptr(std::make_shared<JsonObject>(values)) {}
    Json::Json(object &&values)          : m_ptr(std::make_shared<JsonObject>(move(values))) {}

    Json::Json(const Json& other) : m_ptr(other.m_ptr) {}

    Json& Json::operator=(const Json& other) 
    {
        if (this != &other) 
        {
            m_ptr = other.m_ptr;
        }
        return *this;
    }

    Json::Json(Json&& other) noexcept 
    {
        swap(other);
    }

    Json& Json::operator=(Json&& other) noexcept 
    {
        swap(other);
        return *this;
    }

    //Json Constructor

    //parse
    void CheckIndex(const std::string& str, size_t idx) 
    {
        if (idx >= str.size()) 
        {
            throw ghJsonException("Unexpected end", idx);
        }
    }

    //提前声明为了调用
    Json ParseJson(const std::string & str, size_t & idx);
    Json ParseString(const std::string & str, size_t & idx);
    
    void ParseWhitespace(const std::string& str, size_t & idx)
    {
        while (idx < str.size() && (str[idx] == ' ' || str[idx] == '\r' || str[idx] == '\n' || str[idx] == '\t'))
            idx++;
    }

    Json ParseObject(const std::string & str, size_t & idx) 
    {
        object out;

        ParseWhitespace(str, idx);
        CheckIndex(str, idx);

        if(str[idx] == '}')
            return Json(out);

        while(1)
        {
            try
            {
                ParseWhitespace(str, idx);
                CheckIndex(str, idx);

                std::string key = ParseString(str, idx).GetString();

                ParseWhitespace(str, idx);
                CheckIndex(str, idx);
                
                if(str[idx]!=':')
                    throw ghJsonException("[ERROR]: object parsing, expect':', got " + str[idx], idx);
                idx++;
                
                ParseWhitespace(str, idx);
                CheckIndex(str, idx);
                Json value  = ParseJson(str, idx);
                out.emplace(key, value);
            }
            catch(const ghJsonException& ex)
            {
                throw ghJsonException("[ERROR]: object parse worng, " + std::string(ex.what()) , idx);
            }
            ParseWhitespace(str, idx);
            CheckIndex(str, idx);
            if(str[idx] == '}')
                break;
            if(str[idx] != ',')
                throw ghJsonException("[ERROR]: object format worng, ", idx);
            idx++;
        }
        idx++;
        return Json(out);
    }

    Json ParseArray(const std::string & str, size_t & idx) 
    {
        array out;
        ParseWhitespace(str, idx);
        CheckIndex(str, idx);
        if(str[idx] == ']')
            return Json(out);

        while(1)
        {
            try
            {
                Json test = ParseJson(str, idx);
                out.emplace_back(std::move(test));
            }
            catch(const ghJsonException& ex)
            {
                throw ghJsonException("[ERROR]: array parse worng, " + std::string(ex.what()) , idx);
            }
            ParseWhitespace(str, idx);
            CheckIndex(str, idx);
            if(str[idx] == ']')
                break;
            if(str[idx] != ',')
                throw ghJsonException("[ERROR]: array format worng, ", idx);
            idx++;
        }
        idx++;
        return Json(out);
    }

    Json ParseString(const std::string & str, size_t & idx) 
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

    bool InRange(long x, long lower, long upper)
    {
        return (x >= lower && x <= upper);
    }

    Json ParseNumber(const std::string & str, size_t & idx)
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

    Json ParseLiteral(const std::string &literal, Json target, const std::string & str, size_t & idx) 
    {
        if(str.compare(idx, literal.length(), literal) == 0)
        {
            idx += literal.length();
            return target;
        }
        throw ghJsonException("[ERROR]:expected (" + literal + "), got (" + str.substr(idx, literal.length()) + ")", idx);
    }

    Json ParseJson(const std::string & str, size_t & idx)
    {
        ParseWhitespace(str, idx);
        CheckIndex(str, idx);

        if(str[idx] == 'n')
        {
            return ParseLiteral("null", GetJsonNull(), str, idx);
        }
        else if(str[idx] == 't')
        {
            return ParseLiteral("true", Json(true), str, idx);
        }
        else if(str[idx] == 'f')
        {
            return ParseLiteral("false", Json(false), str, idx);
        }
        else if(str[idx] == '\"')
        {
            return ParseString(str, idx);
        }
        else if(str[idx] == '[')
        {
            return ParseArray(str, ++idx);
        }
        else if(str[idx] == '{')
        {
            return ParseObject(str, ++idx);
        }
        else
        {
            return ParseNumber(str, idx);
        }
    }
    
    Json Parse(const std::string & in)
    {
        size_t idx = 0;
        return ParseJson(in, idx);
    }
    //parse

    //Dump
    void Json::Dump(std::string &out) const 
    { 
        m_ptr->Dump(out);
    }
    //JsonNull
    template<>
    void Value<JsonType::NUL, NullClass>::Dump(std::string &out) const 
    {
        out += "null";
    }
    //JsonBool
    template<>
    void Value<JsonType::BOOL, bool>::Dump(std::string &out) const 
    {
        out += (m_value ? "true" : "false");
    }
    //JsonNumber
    template<>
    void Value<JsonType::NUMBER, double>::Dump(std::string &out) const 
    {
        out += std::to_string(m_value);
    }
    //JsonString
    template<>
    void Value<JsonType::STRING, std::string>::Dump(std::string &out) const 
    {
        out += "\"" + m_value + "\"";
    }
    // JsonArray
    template<>
    void Value<JsonType::ARRAY, array>::Dump(std::string &out) const 
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
            item.Dump(out);
        }
        out += ']';
    }
    // JsonObject
    template<>
    void Value<JsonType::OBJECT, object>::Dump(std::string &out) const 
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
            item.second.Dump(out);
        }
        out += "\n}";
    }
    //Dump

    //swap
    void Json::swap(Json &other) noexcept 
    { 
        std::swap(m_ptr, other.m_ptr); 
    }
    //swap
}
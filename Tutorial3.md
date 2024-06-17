# 从一开始搭建json库教程（三）

我们已经完成了整个数据结构的基础，剩下我们的工作就是搭建 ``parse()`` 接口和搭建 ``dump()`` 接口。`parse()`接口，接受一个参数，原生的 JSON 字符串，然后解析并将其映射到我们之前构建的数据结构中。``dump()``接口的功能则反过来。需要注意的是，在 C++ 中，特殊字符需要转义，因此在字符串中使用双引号时，需要添加反斜杠 \ 进行转义。

例如， 有一个JSON 值：

~~~
{"name": "Jane Doe","age": 30,"address": {"street": "123 Main St","city": "Anytown","zipcode": "12345"}}
~~~

在 C++ 中，我们可以这样表示这一个字符串：

~~~cpp
std::string str = "{\"name\": \"Jane Doe\",\"age\": 30,\"address\": {\"street\": \"123 Main St\",\"city\": \"Anytown\",\"zipcode\": \"12345\"}}";
~~~

在上述示例中，我们已经正确地进行了转义处理。

我相信解析的基础逻辑都应该很容易想到，根据传进来的字符串的首个字符进行判断即可。但因为NUMBER类型没固定首字符，所以我们将无法匹配的都扔到解析数字的部分中。而且有两个特别要注意的点，第一，array和object都是可能有嵌套得，所以我们传入的参数除了待解析的字符串以外，还需要一个解析开始的位置；第二，传入的字符串需要将无效的字符串去掉。

~~~c++
    Json parse(const std::string & in)
    {
        size_t idx = 0;
        size_t depth = 0;
        return parseJson(in, idx, depth);
    }

    void checkIndex(const std::string& str, size_t idx) 
    {
        if (idx >= str.size()) 
        {
            throw ghJsonException("Unexpected end", idx);
        }
    }

    void parseWhitespace(const std::string& str, size_t & idx)
    {
        while (idx < str.size() && (str[idx] == ' ' || str[idx] == '\r' || str[idx] == '\n' || str[idx] == '\t'))
            idx++;
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
    
~~~

像是true、false和null这种应该都能放到同一个处理逻辑中，只需有个判断是否为目标字符串即可，以下是一种实现方式。

~~~c++
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
~~~

如果是解析字符串，就会复杂一些，因为要判断是否需要转义。

~~~c++
    Json parseString(const std::string & str) 
    {
        std::string out;

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
~~~

解析数字的话就可以利用库函数stod

~~~c++
    Json parseNumber(const std::string &str, size_t &idx) 
    {
        size_t processed = 0;
        double value = 0.0;

        try
        {
            value = std::stod(str.substr(idx), &processed);
        }
        catch (const std::invalid_argument&)
        {
            throw ghJsonException("Invalid number format", idx);
        }
        catch (const std::out_of_range&)
        {
            throw ghJsonException("Number out of range", idx);
        }

        // If we get here, processed must be more than 0, since otherwise
        // an exception would have been thrown.
        idx += processed;
        return Json(value);
    }
~~~

解析数组和对象只需要注意特殊的格式要求以及重复调用解析起点函数parse_json()传入的解析位置

~~~c++
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
~~~

如果能已经理解并且完成上述步骤，我觉得这个函数的编写应该很容易，就是要在每个具体的json类中实现自己的部分，举例

~~~c++
    const std::string Json::dump() const
    {
        std::string str;
        size_t depth = 0;
        dump(str, depth);
        return str;
    }

    void Json::dump(std::string &out, size_t depth) const 
    { 
        check();
        m_ptr->dump(out, depth); 
    }

    class JsonNull : public Value<JsonType::NUL, NullClass>
    {
        //..
        void dump(std::string &out, size_t depth) const { out += "null"; }
    };
    class JsonArray : public Value<JsonType::ARRAY, array>
    {
        //..
        void dump(std::string &out, size_t depth) const 
        {
            out += '[';
            bool first = true;
            for (const auto& item : m_value)
            {
                if (!first)
                {
                    out += ", ";
                }
                first = false;
                item.dump(out, depth+1);
            }
            out += ']';
        }
    };

~~~
# 从一开始搭建json库教程（二）

## 完善细节

紧接上一章末尾，要考虑一个问题了，各个Json类型分别用什么来实现呢？在实例化模板的时候，**T**分别都是什么呢？NUMBER用double1，STRING用string，BOOOL用bool，ARRAY其实我们很容易想到可以使用vector<Json>来实现，OBJECT的话是map<string, Json>，那么NULL呢？起码我暂时想不到对应的，那么我就自己造一个，然后各个派生类的代码基础就出来了。同时我们定义一个异常类，用于抛出我们这个库的异常信息。

~~~c++
    class ghJsonException : public std::runtime_error 
    {
        public:
            ghJsonException(const std::string& message, size_t position): std::runtime_error(message), pos(position) {}
            size_t getPosition() const { return pos; }
        private:
            size_t pos;
    };

    class JsonNull : public Value<JsonType::NUL, NullClass>
    {
        public:
            explicit JsonNull() : Value(NullClass()) {};
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
            //iterator
            arrayiter arrayBegin() override {  return m_value.begin(); }
            const_arrayiter arrayBegin_const() const override { return m_value.cbegin();}
            arrayiter arrayEnd() override {  return m_value.end(); }
            const_arrayiter arrayEnd_const() const override { return m_value.cend();}
            //iterator
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
            //iterator
            objectiter objectBegin() override {  return m_value.begin(); }
            const_objectiter objectBegin_const() const override{ return m_value.cbegin();}
            objectiter objectEnd() override {  return m_value.end(); }
            const_objectiter objectEnd_const() const override{ return m_value.cend();}
            //iterator
    };
    //JsonValue
~~~

此时一开始设置的接口，现在可以开始实现了。

~~~C++
    //Json
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
    double Json::getNumber() const { return m_ptr->getNumber(); }
    bool Json::getBool() const { return m_ptr->getBool(); }
    const std::string & Json::getString() const { return m_ptr->getString(); }
    const array & Json::getArray()  const { return m_ptr->getArray(); }
    const object & Json::getObject() const { return m_ptr->getObject(); }
    //getValue
    //setValue
    void Json::setNumber(double value) { return m_ptr->setNumber(value); }
    void Json::setBool(bool value) { return m_ptr->setBool(value); }
    void Json::setString(const std::string & value) { return m_ptr->setString(value); }
    void Json::setArray(const array & value) { return m_ptr->setArray(value); }
    void Json::setObject(const object & value) { return m_ptr->setObject(value); }

    void Json::addToArray (const Json & value) { return m_ptr->addToArray(value); }
    void Json::addToObject(const std::string & key, const Json & value) { return m_ptr->addToObject(key, value); }
    void Json::removeFromArray(size_t index) { return m_ptr->removeFromArray(index); }
    void Json::removeFromObject(const std::string& key) { return m_ptr->removeFromObject(key); }
    //setValue
    //Json
~~~

现在假设一个场景，一个Json对象 test，类型为JsonString，但对其使用getBool()的方法，应该怎么处理呢？我想得到就有两种方法：第一种是对类型不匹配的都返回默认值，第二种是直接抛出异常，我们来实现第二种。

~~~c++
    const char * ToString(ghjson::JsonType type)
    {
        switch (type) 
        {
            case ghjson::JsonType::NUL:    return "null";
            case ghjson::JsonType::NUMBER: return "number";
            case ghjson::JsonType::BOOL:   return "bool";
            case ghjson::JsonType::STRING: return "string";
            case ghjson::JsonType::ARRAY:  return "array";
            case ghjson::JsonType::OBJECT: return "object";
            default:                       return "unknown";
        }
    }

    class JsonValue
    {
        public:
            //type
            virtual JsonType type() const = 0;
            //type
            //getvalue
            virtual double              getNumber() const = 0;
            virtual bool                getBool  () const = 0;
            virtual const std::string & getString() const = 0;
            //getvalue
            //setvalue
            virtual void setNumber (double               value)  = 0;
            virtual void setBool   (bool                 value)  = 0;
            virtual void setString (const  std::string & value)  = 0;
            virtual void setArray  (const  array       & value)  = 0;
            virtual void setObject (const  object      & value)  = 0;

            virtual void addToArray (const Json & value)  = 0;
            virtual void addToObject(const std::string & key, const Json & value)  = 0;
            virtual void removeFromArray(size_t index)  = 0;
            virtual void removeFromObject(const std::string& key)  = 0;
            //setvalue
            //iterator
            virtual arrayiter arrayBegin() = 0;
            virtual const_arrayiter arrayBegin_const() const = 0;
            virtual arrayiter arrayEnd() = 0;
            virtual const_arrayiter arrayEnd_const() const = 0;
            virtual objectiter objectBegin() = 0;
            virtual const_objectiter objectBegin_const() const = 0;
            virtual objectiter objectEnd() = 0;
            virtual const_objectiter objectEnd_const() const = 0;
            //iterator
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
            //getvalue
            double getNumber() const override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            bool getBool  () const override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            const std::string & getString() const override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            const array & getArray () const override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            const object &      getObject() const override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            //getvalue
            //setvalue
            void setNumber (double               value) override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            void setBool   (bool                 value) override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            void setString (const  std::string & value) override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            void setArray  (const  array       & value) override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            void setObject (const  object      & value) override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }

            void addToArray (const Json & value) override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            void addToObject(const std::string & key, const Json & value) override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            void removeFromArray(size_t index) override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            void removeFromObject(const std::string& key) override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); }
            //setvalue
            //iterator
            arrayiter arrayBegin() override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); };
            const_arrayiter arrayBegin_const() const override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); };
            arrayiter arrayEnd() override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); };
            const_arrayiter arrayEnd_const() const override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); };
            objectiter objectBegin() override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); };
            const_objectiter objectBegin_const() const override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); };
            objectiter objectEnd() override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); };
            const_objectiter objectEnd_const() const override { throw ghJsonException("Invalid type:  Attempted to call " + std::string(__func__) + " on a JsonValue of type " + ToString(type()), 0); };
            //iterator
    };
~~~

我们用于存放数据的部分已经完成得差不多了，再补充一下class Json的构造函数。

~~~c++
    class Json
    {
            //..
            Json() noexcept;                // NUL
            Json(std::nullptr_t) noexcept;  // NUL
            Json(int value);             // NUMBER
            Json(double value);             // NUMBER
            Json(bool value);               // BOOL
            Json(const std::string &value); // STRING
            Json(std::string &&value);      // STRING
            Json(const char * value);       // STRING
            Json(const array &values);      // ARRAY
            Json(array &&values);           // ARRAY
            Json(const object &values);     // OBJECT
            Json(object &&values);          // OBJECT
            //..
    }
~~~

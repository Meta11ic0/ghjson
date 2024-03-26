# 从一开始搭建json库教程（二）

~~~cpp
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
~~~

现在先来完善JsonValue，上一章做了一些基础的通用的设计，我们需要针对JsonType中每个类型设计一个继承JsonValue的派生类，并且在其中设计一些独有的细节。除了代表空的NUL没法找到对应的m_value类型，其他应该都很容易想得到。

解决方法也很简单，我们实现一个NUllClass即可，里面只实现我们需要的内容就好。

~~~cpp
    class NullClass
    {
    public:
        bool operator == (NullClass) { return true; }
        bool operator < (NullClass) { return false; }
    };
~~~

现在每个派生类的实现就很容易了

~~~cpp
    class JsonNull final : public Value<JsonType::NUL, NullClass>
    {
        public:
            JsonNull() : Value({}) {};
    };

    class JsonNumber final : public Value<JsonType::NUMBER, double>
    {
        public:
            explicit JsonNumber(int value) : Value(value) {}
        private:
            double GetDouble() const override { return m_value; }
            int GetInt() const override { return m_value; }
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
~~~

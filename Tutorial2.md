# 从一开始搭建json库教程（二）

上一章定义了中间层class Value，实现了可以公用的构造函数、用于返回JSON类型的Tyoe函数，现在先来实现各个JSON类型对应的派生类。但JSON类型为NUL的对象，我们没有在C++可以直接使用的，如果使用nullptr的话代码也不好统一，而且nullptr也无法进行比较。

~~~cpp
    //..
    if(nullptr1 < nullptr2) //报错
    //..
~~~

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

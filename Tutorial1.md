# 从一开始搭建json库教程（一）

上一章开始我们了解了我们什么是json以及json库需要做什么。最重要的是我们知道了我们做哪几部分工作，现在我们从搭建Json数据结构开始。

## Json

我对Json类的大致规划是如下面代码所示，有一个类成员变量指针指向存储的值本身，使用C++11新添加的特性shared_ptr保证内存的安全；有一个类成员变量指向数据类型。

~~~cpp
    using array = std::vector<Json>;
    using object = std::map<std::string, Json>;

    class Json
    {
        public:
            double GetDouble() const;
            int GetInt() const;
            // Return the enclosed value if this is a boolean, false otherwise.
            bool GetBool() const;
            // Return the enclosed string if this is a string, "" otherwise.
            const std::string &GetString() const;
            // Return the enclosed vector if this is an array, or an empty vector otherwise.
            const array &GetArray() const;
            // Return the enclosed map if this is an object, or an empty map otherwise.
            const object &GetObject() const;
            // Return a reference to arr[i] if this is an array, Json() otherwise.
            const Json & operator[](size_t i) const;
            // Return a reference to obj[key] if this is an object, Json() otherwise.
            const Json & operator[](const std::string &key) const;

            JsonType Type() const;
            // Serialize.
            std::string Dump() const;
            // Parse. 
            static Json Parse(const std::string & in, std::string & err);
        private:
            std::shared_ptr<JsonValue> m_ptr;
    };

~~~

## JsonType
    
如上一篇文章所示，json数据类型应该有数值、字符串、布尔值、数组、对象和空值六种。那么我们可以先定义一个枚举类来代表这六种类型。

### Enum vs Enum Class

C++11中，新增加了enum class这一特性。在C++11之前的版本中，只有传统的enum可用。`enum`在C++中可能会导致的隐患包括作用域污染和类型安全问题。以下是一个展示问题的具体例子：

```cpp
enum Color { RED, GREEN, BLUE };
enum Feeling { EXCITED, RELAXED, BLUE };

void paint(Color color) {
    if (color == BLUE) // 调用的是哪个BLUE？？
        // ... paint something blue
}

int main() {
    paint(BLUE); // 语义不明确，因为两个枚举中都有BLUE
    int colorValue = RED; // 隐式转换为int，这可能是不希望的行为
    return 0;
}
```
在上面的例子中，`Color`和`Feeling`两个枚举类型都定义了`BLUE`。在`paint`函数中，当我们用`BLUE`来进行比较时，并不清楚到底是哪个枚举类型的`BLUE`。这就产生了潜在的名称冲突。

此外，枚举值会隐式地转换成整数值，这可能会导致类型安全问题。例如，将枚举`Color`的值`RED`直接赋值给一个整型变量`colorValue`，虽然在某些情况下这是有用的，但这种行为可能会导致逻辑错误，特别是在复杂的应用程序中，我们可能希望明确地避免这种类型转换。

使用`enum class`的优势在于：

1. **作用域限定**：`enum class`不会像传统的`enum`那样将枚举值带入到包含它们的作用域中，而是将枚举值限定在枚举类的作用域下。这样可以避免潜在的名称冲突。

2. **强类型**：`enum class`提高了类型安全，因为它不允许隐式类型转换。所有的枚举值必须显式地进行转换，这减少了因错误的类型转换所导致的bug。

3. **更好的前向兼容性**：`enum class`有助于维护和更新代码，因为任何试图利用可能存在的隐式类型转换的代码都将被编译器捕获。

下面是使用`enum class`重写上述例子的方式：

```cpp
enum class Color { RED, GREEN, BLUE };
enum class Feeling { EXCITED, RELAXED, BLUE };

void paint(Color color) {
    if (color == Color::BLUE) // 调用哪个BLUE现在就很清晰
        // ... paint something blue
}

int main() {
    paint(Color::BLUE); // 明确表明是Color中的BLUE
    // int colorValue = Color::RED; // 这将会导致编译错误，因为强类型不允许隐式转换
    return 0;
}
```
这里，两个`BLUE`都被它们各自的枚举类名所限定，不再存在歧义。这样代码可读性更高，也更容易维护。

### 代码

现在我们就定义一个枚举类JsonType

~~~cpp
    enum class JsonType
    {
        NUL, NUMBER, BOOL, STRING, ARRAY, OBJECT
    };
~~~

其中空值取NUL是为了避免与关键字NULL冲突

## JsonValue

JsonValue 包含了多种不同类型的 JSON 值，如数值、字符串、布尔值、数组和对象等。使用基类和派生类的方式可以使代码结构更加清晰和灵活。基类可以定义通用的接口和操作，而派生类则可以根据具体类型来实现特定的功能。

基类中为不同类型的值提供特定的方法来访问这些值。这种方式的目的是确保类型安全，以及明确表达一个值的预期类型。然后对于ARRAY和OBJECT类型的json对象，我们还要提供“[]”操作符来取值。同时我们要提供纯虚函数dump()和type()，确保每一个特化的派生类有合适的实现方式。

~~~cpp
    template<JsonType tag, typename T>
    class JsonValue
    {
        public:
            explicit JsonValue(const T& value) : m_value(value) {};
            explicit JsonValue(T&& value) : m_value(move(value)) {};
            
            const JsonType type() const { return tag; }
            virtual double GetDouble() const;
            virtual int GetInt() const;
            virtual bool GetBool() const;
            virtual const std::string& GetString() const;
            virtual const array& GetArray() const;
            virtual const Json& operator[](size_t i) const;
            virtual const object& GetObject() const;
            virtual const Json& operator[](const std::string& key) const;

            virtual const std::string Dump() const = 0; //必须每个类型特化一个版本
            virtual ~JsonValue();
        private:
            const T m_value;
            
    };
~~~

然后是每个派生类的实现，将Double和Int分开实现是为了精度，但这并不是必须的。然后除了array和object要实现"[]"运算符重载以外，其它的也只是依葫芦画瓢。

~~~cpp
    class JsonDouble final : public JsonValue<JsonType::NUMBER, double> 
    {
        public:
            explicit JsonDouble(double value) : JsonValue(value) {}
        private:
            double GetDouble() const override { return m_value; }
            int int_value() const override { return static_cast<int>(m_value); }
    };

    class JsonInt final : public JsonValue<JsonType::NUMBER, int> 
    {
        public:
            explicit JsonInt(int value) : JsonValue(value) {}
        private:
            double GetDouble() const override { return m_value; }
            int int_value() const override { return m_value; } 
    }

    class JsonDouble final : public JsonValue<JsonType::NUMBER, double>
    {
        public:
            explicit JsonDouble(double value) : JsonValue(value) {}
        private:
            double GetDouble() const override { return m_value; }
            int GetInt() const override { return static_cast<int>(m_value); }
    };

    class JsonInt final : public JsonValue<JsonType::NUMBER, int>
    {
    public:
        explicit JsonInt(int value) : JsonValue(value) {}
    private:
        double GetDouble() const override { return m_value; }
        int GetInt() const override { return m_value; }
    };

    class JsonBoolen final : public JsonValue<JsonType::BOOL, bool>
    {
    public:
        explicit JsonBoolen(bool value) : JsonValue(value) {}
    private:
        bool GetBool() const override { return m_value; }
    };

    class JsonString final : public JsonValue<JsonType::STRING, std::string>
    {
    public:
        explicit JsonString(const std::string& value) : JsonValue(value) {}
        explicit JsonString(std::string&& value) : JsonValue(move(value)) {}
    private:
        const std::string& GetString() const override { return m_value; }

    };

    class JsonArray final : public JsonValue<JsonType::ARRAY, array>
    {
    public:
        explicit JsonArray(const array& value) : JsonValue(value) {};
        explicit JsonArray(array&& value) : JsonValue(move(value)) {};
    private:
        const array& GetArray() const override { return m_value; }
        const Json& operator[](size_t i) const override { return (*m_ptr)[i]; };
    };

    class JsonObject final : public JsonValue<JsonType::OBJECT, object>
    {
    public:
        explicit JsonObject(const object& value) : JsonValue(value) {};
        explicit JsonObject(object&& value) : JsonValue(move(value)) {};
    private:
        const object& GetObject() const override { return m_value; }
        const Json& operator[](const std::string& key) const override { return (*m_ptr)[key] };
    };

~~~


特殊的例子是空值，C++ 中的 nullptr_t 并不具有可比性，因此无法直接用于比较。
~~~cpp
    if(nullptr1 < nullptr2) //出错
        //..
~~~

所以我们自己可以自定义一个空类，可以通过重载运算符实现比较操作，更方便地进行空值的判断和处理。

~~~cpp
    class NullClass
    {
    public:
        bool operator == (NullClass) { return true; }
        bool operator < (NullClass) { return false; }
    };

    class JsonNull final : public JsonValue<JsonType::NUL, NullClass>
    {
    public:
        JsonNull() : JsonValue({}) {};
    };
~~~

# 从一开始搭建json库教程（一）

上一章开始我们了解了我们什么是json以及json库需要做什么。最重要的是我们知道了我们做哪几部分工作，现在我们从搭建Json数据结构开始。

## class Json

首先class Json中应该有存放对应JSON类型的数据的对象，所以先我们定义一个类用于存放实际数据以及数据类型，然后还需要定义一些获取数据的接口。

首先因为JSON类型有很多种，存放数据的类的设计我们可以使用基类和派生类的方式，基类JsonValue中包含一些通用的成员，派生类按类型实现为JsonNumber、JsonString等，这样的话只需在class Json中包含一个基类JsonValue的指针即可。使用std::string表示JSON类型的STRING，std::vector表示JSON类型的ARRAY，std::map表示JSON类型的OBJECT。为了节省类型检查的工作，对所有JSON类型提供所有JSON类型的取值接口，还需要为ARRAY和OBJECT提供对应的operator[]，意味着当你向一个JSON类型为STRING的对象使用GetNumber()也是可行的，但是我们对类型没有正确匹配的取值操作返回默认值，这样能保证代码的容错性和灵活性。

代码暂时设计如下：

~~~cpp
    class Json;
    class JsonValue;

    using array = std::vector<Json>;
    using object = std::map<std::string, Json>;

    class Json
    {
        public:
            //..
            JsonType Type() const;
            //GetValue
            double              GetNumber() const;
            bool                GetBool()   const;
            const std::string & GetString() const;
            const array &       GetArray()  const;
            const object &      GetObject() const;

            const Json &        operator[](size_t i) const;
            const Json &        operator[](const std::string &key) const;
            //Comparisons
            bool operator== (const Json &rhs) const;
            bool operator<  (const Json &rhs) const;
            //..
        private:
            std::shared_ptr<JsonValue> m_ptr;
            //..
    };
~~~



## Class JsonType
    
JSON类型应该有数值、字符串、布尔值、数组、对象和空值六种。那么我们可以先定义一个枚举类来代表这六种类型。 

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

## Class JsonValue

基类class JsonValue可以定义通用的接口和操作，而派生类则可以根据具体类型来实现特定的功能。派生类的实现因为要存储一个JsonType和实际值的类型（譬如：double，string），使用template设计是比较合适的。但一旦在class JsonValue这一层使用Template的话，class Json中的JsonValue指针就会比较臃肿。所以我们在实际使用的派生类如class JsonNumber、class JsonString和基类class JsonValue中加一个中间层class Value，用于实现需要template定义后的通用的函数。

~~~cpp 
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
~~~
# 从一开始搭建json库教程（一）

本文开始我们就正式进入代码层面。

## 数据类型
    
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



### JsonType

现在我们就定义一个枚举类JsonType

~~~cpp
    enum class JsonType
    {
        NUL, NUMBER, BOOL, STRING, ARRAY, OBJECT
    };
~~~

其中空值取NUL是为了避免与关键字NULL冲突

## 数据值

然后我们创建一个Json类

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace ghjson
{
    using std::string;
    using std::vector;
    using std::map;
    using std::shared_ptr;
    using std::make_shared;

    enum class JsonType
    {
        NUL, NUMBER, BOOL, STRING, ARRAY, OBJECT
    };

    using array = vector<JsonValue>;
    using object = map<string,JsonValue>;

    class JsonValue
    {
        public:
            virtual const JsonType type() const = 0;
            //要提供比较函数
            virtual bool equals(const JsonValue * other) const = 0;
            virtual bool less(const JsonValue * other) const = 0;
            virtual const string dump() const = 0;
            virtual const double number_value() const;
            virtual const int int_value() const; 
            virtual const bool bool_value() const;
            virtual const string &string_value() const;
            virtual const array &array_items() const;
            virtual const Json &operator[](size_t i) const;
            virtual const object &object_items() const;
            virtual const Json &operator[](const string &key) const;
            virtual ~JsonValue(){};   
    };



    class Json
    {
        public:

            /*noexcept关键字在C++中用来指定一个函数不会抛出异常。
            当用在构造函数上时，它表明构造函数调用过程中不会抛出任何异常，
            这样的构造函数通常具有更高的性能，因为编译器可以做出额外优化，例如更有效地使用移动语义而不是复制。
            在Json类中，NUL类型的构造函数被标记为noexcept，
            因为它们实际上执行的操作非常简单：它们只是初始化内部的指针成员为null，没有进行任何可能抛出异常的操作
            如NUMBER、BOOL、STRING、ARRAY和OBJECT, 
            例如字符串的构造可能涉及到内存的分配，
            而这个操作是有可能失败并抛出异常的，
            所以它们的构造函数不应标记为noexcept。
            相似地，向vector添加元素（在ARRAY类型中）
            和向map添加键值对（在OBJECT类型中）都可能引起内存分配错误，
            并携带异常抛出的风险。
            因此，只有那些保证在任何情况下都不会触发异常抛出的构造函数才应该被声明为noexcept。
            这是为了清晰地表明它们的异常安全保证，
            以及允许编译器进行优化
            */
            // Constructors for the various types of JSON value.
            Json() noexcept;                // NUL
            Json(std::nullptr_t) noexcept;  // NUL
            Json(double value);             // NUMBER
            Json(int value);                // NUMBER
            Json(bool value);               // BOOL
            Json(const std::string &value); // STRING
            Json(std::string &&value);      // STRING
            Json(const char * value);       // STRING
            Json(const array &values);      // ARRAY
            Json(array &&values);           // ARRAY
            Json(const object &values);     // OBJECT
            Json(object &&values);          // OBJECT

            Json Parse(const string & str);
            string dump() const;
            const JsonType type() const;
            // Return the enclosed value if this is a number, 0 otherwise. Note that json11 does not
            // distinguish between integer and non-integer numbers - number_value() and int_value()
            // can both be applied to a NUMBER-typed object.
            const double number_value() const;
            const int int_value() const;
            //int int_value() const;
            // Return the enclosed value if this is a boolean, false otherwise.
            const bool bool_value() const;
            // Return the enclosed string if this is a string, "" otherwise.
            const string &string_value() const;
            // Return the enclosed vector if this is an array, or an empty vector otherwise.
            const array &array_items() const;
            // Return the enclosed map if this is an object, or an empty map otherwise.
            const object &object_items() const;

            // Return a reference to arr[i] if this is an array, Json() otherwise.
            const Json & operator[](size_t i) const;
            // Return a reference to obj[key] if this is an object, Json() otherwise.
            const Json & operator[](const string &key) const;

        private:
            std::shared_ptr<JsonValue> m_ptr;
    };

}
# 从一开始搭建json库教程（一）

## 框架搭建

首先我们了解到，json格式的数据有六种类型，分别是null（空值），bool（布尔值），number（数值），string（字符值），array（数组），object（对象）。那么我们可以用一个枚举类来表示，其中空值取NUL是为了避免与关键字NULL冲突：

~~~cpp
    enum class JsonType
    {
        NUL, NUMBER, BOOL, STRING, ARRAY, OBJECT
    };
~~~

我们需要提供给使用者一个类，但是却可以在实际运行的时候指向上述不同的类型。在C++中，我们可以使用多态实现。我们这样设计:

~~~c++
    class JsonValue
    {
        //..
    };

    class JsonNumber : public JsonValue
    {
        //..
        double m_value;
        //..
    };

    class Json
    {
        //..
        JsonValue * m_ptr;
        //..
    };
~~~

这样的话，我只需要再在Json类中设计一些调用m_ptr的接口，就可以实现预想的使用场景。

## 接口设计
如同我一开始设想，现在Json类还需要一些方法给客户调用，那我们现在考虑需要一些什么接口呢？对于类型JsonType而言，很简单的，可以想到

~~~c++
    class Json
    {
        //..
        JsonType type()  const;
        bool is_null()   const;
        bool is_number() const;
        bool is_bool()   const;
        bool is_string() const;
        bool is_array()  const;
        bool is_object() const;
        //..
    }
~~~

然后关于JsonValue的操作接口，对于null（空值）我们不需要提供另外的操作，但基础类型如bool（布尔值），number（数值），string（字符值）我们需要提供取值以及写入的函数。

~~~c++
    class Json
    {
        //..
        double              getNumber() const;
        bool                getBool()   const;
        const std::string & getString() const;
        //..
        void setNumber (double               value);
        void setBool   (bool                 value);
        void setString (const  std::string & value);
        //..
    }
~~~

对于array（数组），object（对象），我们有两种思路，第一种是将实际值的引用作为返回值，然后调用者直接在返回值上操作。

~~~c++
    class Json
    {
        //..
        array &       getArray() ;
        object &      getObject();
        //..
        void setArray  (const  array       & value);
        void setObject (const  object      & value);
        //..
    }            
~~~

第二种是我们直接提供接口用于操作，提供类似vector迭代器，map迭代器的一些操作，也提供查找，插入，删除组内元素的操作。

~~~c++
    class Json
    {
        //..
        arrayiter arrayBegin();
        const_arrayiter arrayBegin_const() const ;
        arrayiter arrayEnd();
        const_arrayiter arrayEnd_const() const ;
        objectiter objectBegin();
        const_objectiter objectBegin_const() const ;
        objectiter objectEnd();
        const_objectiter objectEnd_const() const ;
        //..
        arrayiter find(size_t index);
        const_arrayiter find(size_t index) const ;
        objectiter find(const std::string & key, const Json & value);
        const_objectiter find(const std::string & key, const Json & value) const ;
        //..
        void addToArray (const Json & value);
        void addToObject(const std::string & key, const Json & value);

        void removeFromArray(size_t index) ;
        void removeFromObject(const std::string& key);
        //..
        void setArray (const array  & value);
        void setObject (const object & value);
        //..
    }
~~~

## 细节设计

既然最外层的接口定了，现在我们可以来设计细节。为了结构统一和封装性，我们在class Json中只提供一个指向JsonValue的指针和各种方法。我们初步可以想到，因为每个具体的Json类型实际存储的值的类型不一样，JsonNumber使用double，JsonString使用标准库的string，返回的Json类型也不一样，所以可能要到类型确定了才能进行一部分方法的实现。

~~~cpp
    class JsonNumber : public JsonValue
    {
        //..
        JsonNumber(double value) : m_value(value) {}
        JsonType type() { return JsonType::NUMBER; }
        double m_value;
        //..
    };

    class JsonString : public JsonValue
    {
        //..
        JsonString(const string & value) : JsonString(value) {}
        JsonType type() { return JsonType::STRING; }
        string m_value;
        //..
    };
~~~
这个时候我们容易发现派生类中其实很多方法实现方式差不多，所以我们可以考虑将其共同的部分提取出来到基类中，以减少重复代码。

~~~c++
    template<JsonType tag, typename T>
    class JsonValue
    {
        //..
        JsonValue(T value): m_value(value) {}
        JsonType type() { return tag; }
        T m_value;
        //..
    };
~~~

但如果这样设计的话，class Json中的JsonValue指针就会比较臃肿，class Json就要进行相应的修改。所以我们可以在JsonValue和具体的Json类中加一个中间层Value，用于实现需要类型确定后的通用的函数。

~~~c++
    class JsonValue
    {
        //..
        //..
    };

    template<JsonType tag, typename T>
    class Value : public JsonValue
    {
        //..
        Value(T value): m_value(value) {}
        JsonType type() { return tag; }
        T m_value;
        //..
    };
~~~

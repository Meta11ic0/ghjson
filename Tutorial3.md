# 从一开始搭建json库教程（三）

上一章我们有了基础的代码框架，接下来就是各种声明过了函数的实现。我们先来实现取值函数。正如我们一开始的时候提到的，为了省去类型检查的工作，我们对没有正确使用的对应类型的取值返回默认值。

## 默认值的设置

我们返回的默认值在整个程序运行过程中都应该是不变的，不能被修改的。所以只需在内存中分配一次，之后可以重复使用，而不需要每次调用函数时都重新创建这些空的容器。

~~~cpp
    class DefalutValue
    {
        public:
            const shared_ptr<JsonValue> null = make_shared<JsonNull>();
            const shared_ptr<JsonValue> t    = make_shared<JsonBool>(true);
            const shared_ptr<JsonValue> f    = make_shared<JsonBool>(false);
            const std::string                  empty_string;
            const array                        empty_vector;
            const object                       empty_map;
            DefalutValue(){};       
    };

    static const DefalutValue & GetDefaultValue()
    {
        static const DefalutValue d{};
        return d;
    };
~~~

然后对于取值函数的非特化实现

~~~cpp
    //..
    //Json
    double              Json::GetNumber() const { return m_ptr->GetNumber(); }
    bool                Json::GetBool()   const { return m_ptr->GetBool();   }
    const std::string & Json::GetString() const { return m_ptr->GetString(); }
    const array &       Json::GetArray()  const { return m_ptr->GetArray();  }
    const object &      Json::GetObject() const { return m_ptr->GetObject(); }
    //JsonValue
    double              JsonValue::GetNumber() const { return 0;                                  }
    bool                JsonValue::GetBool()   const { return false;                              }
    const std::string & JsonValue::GetString() const { return GetDefaultValue().empty_string; }
    const array &       JsonValue::GetArray()  const { return GetDefaultValue().empty_vector; }
    const object &      JsonValue::GetObject() const { return GetDefaultValue().empty_map;    }
    //..
~~~

但注意到，对于ARRAY和OBJECT来说，他们n的operator[]特化还没实现。这两个类别的operator[]都遵循一个逻辑，要判断合法性，不合法是返回默认值，这里的这个默认值类型为Json而不是JsonValue，我们如法炮制定义一个JSON类型为NULL的局部静态变量。

~~~cpp
    //..
    static const Json & GetJsonNull()
    {
        static const Json jsonnull; //会调用Json()，从而要使用GetDefaultValue().null，所以将Json默认值跟JsonValue默认值分隔开，不然会有循环依赖
        return d;
    }
    //..
    //operator[]
    const Json & Json::operator[] (size_t i)          const { return (*m_ptr)[i];           }
    const Json & Json::operator[] (const string &key) const { return (*m_ptr)[key];         }
    const Json & JsonValue::operator[] (size_t)         const { return GetJsonNull(); }
    const Json & JsonValue::operator[] (const string &) const { return GetJsonNull(); }
    //特化的operator[]
    const Json & JsonArray::operator[] (size_t i) const 
    { 
        return (i > m_value.size() ? GetJsonNull() : m_value[i]);
    }
    const Json & JsonObject::operator[] (const string & key) const 
    { 
        auto iter = m_value.find(key);
        return (iter == m_value.end() ? GetJsonNull() : iter->second); //这里不能使用m_value[key],因为m_value是const对象，这个函数也是const函数，而m_value[key]是会在key不存在的时候新建一个值，所以编译器会报错
    }
~~~

## 比较的实现

因为在Json类的实例对象中，没法直接调用成员变量m_ptr的成员m_value，所以在Value类中再实现成员函数equals和less。

~~~cpp

    template <JsonType tag, typename T>
    class Value : public JsonValue
    {
        //..
        protected:
            // Comparisons
            bool equals(const JsonValue * other) const {
                return m_value == static_cast<const Value<tag, T> *>(other)->m_value;
            }
            bool less(const JsonValue * other) const {
                return m_value < static_cast<const Value<tag, T> *>(other)->m_value;
            }
        //..
    };
    bool Json::operator== (const Json &rhs) const
    {
        if(m_ptr == rhs.m_ptr)
            return true;
        else if(m_ptr->Type() != rhs.Type())
            return false;
        
        //没法直接return m_ptr->m_value == rhs.m_value;
        return m_ptr->equals(rhs.m_ptr->get());
    }

    bool Json::operator<  (const Json &rhs) const
    {
        if (m_ptr == rhs.m_ptr)
            return false;
        if (m_ptr->Type() != rhs.m_ptr->Type())
            return m_ptr->Type() < rhs.m_ptr->Type();

        return m_ptr->less(rhs.m_ptr.get());
    }
~~~

只要实现了上面相等和小于，其他的比较操作就能通过调用来这两个函数实现

~~~cpp
    class Json
    {
        public:
        //..
            bool operator!= (const Json &rhs) const { return !(*this == rhs); }
            bool operator<= (const Json &rhs) const { return !(rhs < *this); }
            bool operator>  (const Json &rhs) const { return  (rhs < *this); }
            bool operator>= (const Json &rhs) const { return !(*this < rhs); }
        //..
    }
~~~

目前为止，我们已经将需要在代码中使用的json数据结构搭建的差不多了，可能还差一些各式各样的对应的构造函数

~~~cpp
    //..
    //Constructors
    //因为设置了一个JSON类型为NULL的局部静态变量，所以这里可以实现没有入参的或者入参为空值的Json构造函数
    Json::Json() noexcept                : m_ptr(GetDefaultValue().null) {}
    Json::Json(std::nullptr_t) noexcept  : m_ptr(GetDefaultValue().null) {}
    Json::Json(double value)             : m_ptr(std::make_shared<JsonNumber>(value)) {}
    Json::Json(bool value)               : m_ptr(value ? GetDefaultValue().t : GetDefaultValue().f) {}
    Json::Json(const std::string &value) : m_ptr(std::make_shared<JsonString>(value)) {}
    Json::Json(std::string &&value)      : m_ptr(std::make_shared<JsonString>(move(value))) {}
    Json::Json(const char * value)       : m_ptr(std::make_shared<JsonString>(value)) {}
    Json::Json(const array &values)      : m_ptr(std::make_shared<JsonArray>(values)) {}
    Json::Json(array &&values)           : m_ptr(std::make_shared<JsonArray>(move(values))) {}
    Json::Json(const object &values)     : m_ptr(std::make_shared<JsonObject>(values)) {}
    Json::Json(object &&values)          : m_ptr(std::make_shared<JsonObject>(move(values))) {}
    //Constructors
    //..
~~~
最后还可以加上一些用于判断类型的函数
~~~cpp
    //..
    bool is_null()   const { return Type() == JsonType::NUL; }
    bool is_number() const { return Type() == JsonType::NUMBER; }
    bool is_bool()   const { return Type() == JsonType::BOOL; }
    bool is_string() const { return Type() == JsonType::STRING; }
    bool is_array()  const { return Type() == JsonType::ARRAY; }
    bool is_object() const { return Type() == JsonType::OBJECT; }
    //..
~~~

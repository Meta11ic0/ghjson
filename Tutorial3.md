# 从一开始搭建json库教程（三）

上一章我们有了基础的代码框架，接下来就是各种声明过了函数的实现。我们先来实现取值函数。正如我们一开始的时候提到的，为了省去类型检查的工作，我们对没有正确使用的对应类型的取值返回默认值。

## 默认值的设置

我们返回的默认JsonValue值在整个程序运行过程中都应该是不变的，不能被修改的。所以只需在内存中分配一次，之后可以重复使用，而不需要每次调用函数时都重新创建这些空的容器。

~~~cpp
    class Default_JsonValue
    {
        public:
            const shared_ptr<JsonValue> null = make_shared<JsonNull>();
            const shared_ptr<JsonValue> t    = make_shared<JsonBool>(true);
            const shared_ptr<JsonValue> f    = make_shared<JsonBool>(false);
            const std::string                  empty_string;
            const array                        empty_vector;
            const object                       empty_map;
            Default_JsonValue(){};       
    };

    static const Default_JsonValue & GetDefaultJsonValue()
    {
        static const Default_JsonValue d{};
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
    const std::string & JsonValue::GetString() const { return GetDefaultJsonValue().empty_string; }
    const array &       JsonValue::GetArray()  const { return GetDefaultJsonValue().empty_vector; }
    const object &      JsonValue::GetObject() const { return GetDefaultJsonValue().empty_map;    }
    //..
~~~

但你应该注意到，对于ARRAY和OBJECT来说，他们n的operator[]特化还没实现。这两个类别的operator[]都遵循一个逻辑，要判断想取的值是否存在，存在则返回目标，不存在也是返回默认值。那么这里的默认值类型为Json。这个默认值整个程序运行过程中都应该是不变的，不能被修改的了。所以我们如法炮制定义一个局部静态变量。

~~~cpp
    //..
    static const Json & GetJsonNull()
    {
        static const Json jsonnull; //会调用Json()，从而要使用GetDefaultJsonValue().null，所以将Json默认值跟JsonValue默认值分隔开，不然会有循环依赖
        return d;
    }
    //..
    Json::Json() noexcept                  : m_ptr(GetDefaultJsonValue().null) {}
    Json::Json(std::nullptr_t) noexcept    : m_ptr(GetDefaultJsonValue().null) {}
    Json::Json(bool value)                 : m_ptr(value ? GetDefaultJsonValue().t : GetDefaultJsonValue().f) {}
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
# 从一开始搭建json库教程（三）

上一章我们有了基础的代码框架，接下来就是各种细节的补充，我们先来实现取值函数。正如我们上一章在设计JsonValue的时候提到的，为了省去类型检查的工作，我们对没有正确使用的对应类型的取值返回默认值。

## 默认值的设置

我们返回的默认值在整个程序运行过程中都应该是不变的，不能被修改的。所以只需在内存中分配一次，之后可以重复使用，而不需要每次调用函数时都重新创建这些空的容器。

~~~cpp
    class Default_Value
    {
        public:
            const shared_ptr<JsonValue> null = make_shared<JsonNull>();
            const shared_ptr<JsonValue> t    = make_shared<JsonBool>(true);
            const shared_ptr<JsonValue> f    = make_shared<JsonBool>(false);
            const std::string                  empty_string;
            const array                        empty_vector;
            const object                       empty_map;
            Default_Value(){};       
    }

    static const Default_Value & GetDefault()
    {
        static const Default_Value d{};
        return d;
    }
~~~

然后对于取值函数的非特化实现

~~~cpp
    //Json
    double              Json::GetNumber()      const { return m_ptr->GetNumber();     }
    bool                Json::GetBool()        const { return m_ptr->GetBool();       }
    const std::string & Json::GetString()      const { return m_ptr->GetString();     }
    const array &       Json::GetArray()       const { return m_ptr->GetArray();      }
    const object &      Json::GeObject()       const { return m_ptr->GeObject();      }

    //JsonValue
    double              JsonValue::GetNumber() const { return 0;                      }
    bool                JsonValue::GetBool()   const { return false;                  }
    const std::string & JsonValue::GetString() const { return GetDefault().empty_string; }
    const array &       JsonValue::GetArray()  const { return GetDefault().empty_vector; }
    const object &      JsonValue::GeObject()  const { return GetDefault().empty_map;    }
~~~

但对于ARRAY和OBJECT来说，他们有返回值类别为Json的operator[]，上一章特化的版本还没有实现，普通的返回默认值的也还没实现。为啥放到现场才一起讲是因为这两个类别的operator[]都遵循一个逻辑，要判断想取的值是否存在，存在返回目标，不存在也是返回默认值。那么这里的默认值类型为Json，我们将默认值设定一个类别为JsonNull的对象。这个时候回想起来开头我们设置的默认值，其实JsonNull的对象存在一个也就行了。同理，JsonBool只需存在两个分别代表true和false即可，全局可以共享。

~~~cpp
    //..
    Json::Json() noexcept                  : m_ptr(GetDefault().null) {}
    Json::Json(std::nullptr_t) noexcept    : m_ptr(GetDefault().null) {}
    Json::Json(bool value)                 : m_ptr(value ? GetDefault().t : GetDefault().f) {}
    //..
    const Json & Json::operator[] (size_t i)          const { return (*m_ptr)[i];           }
    const Json & Json::operator[] (const string &key) const { return (*m_ptr)[key];         }
    //..
    const Json & JsonValue::operator[] (size_t)         const { return GetDefault().null; }
    const Json & JsonValue::operator[] (const string &) const { return GetDefault().null; }
~~~
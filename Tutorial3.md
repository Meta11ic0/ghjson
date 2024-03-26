# 从一开始搭建json库教程（三）

上一章我们有了基础的代码框架，接下来就是各种细节的补充，我们先来实现取值函数。正如我们上一章在设计JsonValue的时候提到的，为了省去类型检查的工作，我们对没有正确使用的对应类型的取值返回默认值。

## 默认值的设置

除了JsonNumber返回0之外，其他所有在整个程序运行过程中都是不变的，所以只需在内存中分配一次，之后可以重复使用，而不需要每次调用函数时都重新创建这些空的容器。

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

    static const Default_Value & Default()
    {
        static const Default_Value d{};
        return d;
    }
~~~

然后对于取值函数的非特化实现

~~~cpp
    //Json
    JsonType            Json::Type()                                const { return m_ptr->Type();      }
    double              Json::GetNumber()                           const { return m_ptr->GetNumber(); }
    bool                Json::GetBool()                             const { return m_ptr->GetBool();   }
    const std::string & Json::GetString()                           const { return m_ptr->GetString(); }
    const array &       Json::GetArray()                            const { return m_ptr->GetArray();  }
    const object &      Json::GeObject()                            const { return m_ptr->GeObject();  }
    const Json &        Json::operator[] (size_t i)                 const { return (*m_ptr)[i];        }
    const Json &        Json::operator[] (const std::string &key)   const { return (*m_ptr)[key];      }
    //JsonValue
    double              JsonValue::GetNumber()                      const { return 0; }
    bool                JsonValue::GetBool()                        const { return false; }
    const std::string & JsonValue::GetString()                      const { return Default().empty_string; }
    const array &       JsonValue::GetArray()                       const { return Default().empty_vector; }
    const object &      JsonValue::GeObject()                       const { return Default().empty_map; }
    const Json &        JsonValue::operator[] (size_t)              const { return static_null(); }
    const Json &        JsonValue::operator[] (const std::string &) const { return static_null(); }
~~~



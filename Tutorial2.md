# 从一开始搭建json库教程（二）

## JSON 数据结构细节补充

### 取值
在暴露出去的Json类中不限制取值函数的使用，每个派生类实现自己对应的特化的版本，一旦调用了不对应的取值函数，我们就返回默认值。

~~~cpp
    //Json类
    JsonType       Json::type()                           const { return m_ptr->type();      }
    double         Json::GetDouble()                      const { return m_ptr->GetDouble(); }
    int            Json::GetInt()                         const { return m_ptr->GetInt();    }
    bool           Json::GetBool()                        const { return m_ptr->GetBool();   }
    const          string & Json::GetString()             const { return m_ptr->GetString(); }
    const array &  Json::GetArray()                       const { return m_ptr->GetArray();  }
    const object & Json::GetObject()                      const { return m_ptr->GetObject(); }
    const Json &   Json::operator[] (size_t i)            const { return (*m_ptr)[i];        }
    const Json &   Json::operator[] (const string &key)   const { return (*m_ptr)[key];      }

    //JsonValue类 
    double         JsonValue::GetDouble()                 const { return 0; }
    int            JsonValue::GetInt()                    const { return 0; }
~~~

NUMBER类我们都可以直接返回原生的“0”来表示默认值，但是，原生的C++中无法。首先为了保证数据一致性，通过将所有同一类的都映射为同一个static对象。


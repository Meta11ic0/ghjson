#include "ghjson.hpp"

namespace ghjson
{
    template<JsonType tag, typename T>
    class Value : public JsonValue
    {
        protected:
            explicit Value(const T& value) : m_value(value) {};
            explicit Value(T&& value) : m_value(move(value)) {};
            // Get type tag
            const JsonType type() const override { return tag; }
            const T m_value;
    };

    class NullClass
    {
    public:
        bool operator == (NullClass) { return true; }
        bool operator < (NullClass) { return false; }
    };

    class JsonNull final : public Value<JsonType::NUL, NullClass>
    {
    public:
        JsonNull() : Value({}) {};
    };

    class JsonDouble final : public Value<JsonType::NUMBER, double>
    {
        public:
            explicit JsonDouble(double value) : Value(value) {}
        private:
            double GetDouble() const override { return m_value; }
            int GetInt() const override { return static_cast<int>(m_value); }
    };

    class JsonInt final : public Value<JsonType::NUMBER, int>
    {
    public:
        explicit JsonInt(int value) : Value(value) {}
    private:
        double GetDouble() const override { return m_value; }
        int GetInt() const override { return m_value; }
    };

    class JsonBoolean final : public Value<JsonType::BOOL, bool>
    {
    public:
        explicit JsonBoolean(bool value) : Value(value) {}
    private:
        bool GetBool() const override { return m_value; }
    };

    class JsonString final : public Value<JsonType::STRING, std::string>
    {
    public:
        explicit JsonString(const std::string& value) : Value(value) {}
        explicit JsonString(std::string&& value) : Value(move(value)) {}
    private:
        const std::string& GetString() const override { return m_value; }

    };

    class JsonArray final : public Value<JsonType::ARRAY, array>
    {
    public:
        explicit JsonArray(const array& value) : Value(value) {};
        explicit JsonArray(array&& value) : Value(move(value)) {};
    private:
        const array& GetArray() const override { return m_value; }
        const Json& operator[](size_t i) const override { return (*m_ptr)[i]; };
    };

    class JsonObject final : public Value<JsonType::OBJECT, object>
    {
    public:
        explicit JsonObject(const object& value) : Value(value) {};
        explicit JsonObject(object&& value) : Value(move(value)) {};
    private:
        const object& GetObject() const override { return m_value; }
        const Json& operator[](const std::string& key) const override { return (*m_ptr)[key] };
    };
}
#include "ghjson.hpp"

namespace ghjson
{
    
template<JsonType tag, typename>
class Value : public JsonValue
{

};

class JsonNumber : public Value<JsonType::NUMBER, double>
{

};

class JsonBool : public Value<JsonType::BOOL, bool>
{

};

class JsonString : public Value<JsonType::STRING, std::string>
{

};

class JsonArray : public Value<JsonType::ARRAY, ghjson::array>
{

};

class JsonObject: public Value<JsonType::OBJECT, ghjson::object>
{

};
} // namespace ghjson

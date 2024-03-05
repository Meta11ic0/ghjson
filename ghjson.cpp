#include "ghjson.hpp"

namespace ghjson
{
template<Json::JsonType tag, typename>
class Value : public JsonValue
{

};
class JsonNumber : public Value<Json::JsonType::NUMBER ,double>
{

};
class JsonBool : public Value<Json::JsonType::BOOL ,bool>
{

};
class JsonString : public Value<Json::JsonType::STRING ,std::string>
{

};
class JsonArray : public Value<Json::JsonType::ARRAY ,v>
{

};
class JsonObject: public Value<Json::JsonType::OBJECT ,double>
{

};
} // namespace ghjson

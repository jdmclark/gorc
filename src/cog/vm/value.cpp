#include "value.h"
#include "cog/constants.h"

using namespace Gorc::Cog::VM;

Value::Value()
	: type(Type::Void)
{
	return;
}

Value::Value(const Value& v)
{
	*this = v;
}

const Value& Value::operator=(const Value& v)
{
	type = v.type;
	data = v.data;

	return v;
}

Value::Value(int v)
{
	*this = v;
}

int Value::operator=(int v)
{
	type = Type::Integer;
	data.Integer = v;

	return v;
}

Value::operator int() const
{
	switch(type)
	{
	case Type::Void:
		return Constants::DefaultInt;

	case Type::Integer:
		return data.Integer;

	case Type::Float:
		return static_cast<int>(data.Floating);

	case Type::Boolean:
		return (data.Boolean) ? 1 : 0;

	case Type::String:
		return Constants::DefaultInt;

	case Type::Vector:
		return Constants::DefaultInt;

	default:
		return Constants::DefaultInt;
	}
}

Value::Value(float v)
{
	*this = v;
}

float Value::operator=(float v)
{
	type = Type::Float;
	data.Floating = v;

	return v;
}

Value::operator float() const
{
	switch(type)
	{
	case Type::Void:
		return Constants::DefaultFloat;

	case Type::Integer:
		return static_cast<float>(data.Integer);

	case Type::Float:
		return data.Floating;

	case Type::Boolean:
		return (data.Boolean) ? 1.0f : 0.0f;

	case Type::String:
		return Constants::DefaultFloat;

	case Type::Vector:
		return Constants::DefaultFloat;

	default:
		return Constants::DefaultFloat;
	}
}

Value::Value(bool v)
{
	*this = v;
}

bool Value::operator=(bool v)
{
	type = Type::Boolean;
	data.Boolean = v;

	return v;
}

Value::operator bool() const
{
	switch(type)
	{
	case Type::Void:
		return false;

	case Type::Integer:
		return data.Integer != 0;

	case Type::Float:
		return data.Floating != 0.0f;

	case Type::Boolean:
		return data.Boolean;

	case Type::String:
		return true;

	case Type::Vector:
		return true;

	default:
		return false;
	}
}

Value::Value(const char* v)
{
	*this = v;
}

const char* Value::operator=(const char* v)
{
	type = Type::String;
	data.String = v;

	return v;
}

Value::operator const char*() const
{
	switch(type)
	{
	case Type::Void:
		return Constants::DefaultString;

	case Type::Integer:
		return Constants::DefaultString;

	case Type::Float:
		return Constants::DefaultString;

	case Type::Boolean:
		return Constants::DefaultString;

	case Type::String:
		return data.String;

	case Type::Vector:
		return Constants::DefaultString;

	default:
		return Constants::DefaultString;
	}
}

Value::Value(const Math::Vector<float>& v)
{
	*this = v;
}

const Gorc::Math::Vector<float>& Value::operator=(const Math::Vector<float>& v)
{
	type = Type::Vector;
	data.Vector.X = v.X;
	data.Vector.Y = v.Y;
	data.Vector.Z = v.Z;

	return v;
}

Value::operator Gorc::Math::Vector<float>() const
{
	switch(type)
	{
	case Type::Void:
		return Math::Vector<float>();

	case Type::Integer:
		return Math::Vector<float>();

	case Type::Float:
		return Math::Vector<float>();

	case Type::Boolean:
		return Math::Vector<float>();

	case Type::String:
		return Math::Vector<float>();

	case Type::Vector:
		return Math::Vector<float>(data.Vector.X, data.Vector.Y, data.Vector.Z);

	default:
		return Math::Vector<float>();
	}
}

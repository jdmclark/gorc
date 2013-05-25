#include <nullunit/nullunit.h>
#include "cog/vm/value.h"

using namespace Gorc::Cog::VM;
using Gorc::Math::Vector;

BeginSuite(Value_Test);

Case(CastIntToInt) {
	Value v1(0);
	Test_Expect_Eq(static_cast<int>(v1), 0);

	Value v2(1);
	Test_Expect_Eq(static_cast<int>(v2), 1);

	Value v3(-1);
	Test_Expect_Eq(static_cast<int>(v3), -1);
}

Case(CastIntToFloat) {
	Value v1(0);
	Test_Expect_Eq(static_cast<float>(v1), 0.0f);

	Value v2(1);
	Test_Expect_Eq(static_cast<float>(v2), 1.0f);

	Value v3(-1);
	Test_Expect_Eq(static_cast<float>(v3), -1.0f);
}

Case(CastIntToBool) {
	Value v1(0);
	Test_Expect_Eq(static_cast<bool>(v1), false);

	Value v2(1);
	Test_Expect_Eq(static_cast<bool>(v2), true);

	Value v3(-1);
	Test_Expect_Eq(static_cast<bool>(v3), true);
}

Case(CastIntToString) {
	Value v1(0);
	Test_Expect_Eq(static_cast<const char*>(v1), std::string());

	Value v2(1);
	Test_Expect_Eq(static_cast<const char*>(v2), std::string());

	Value v3(-1);
	Test_Expect_Eq(static_cast<const char*>(v3), std::string());
}

Case(CastIntToVector) {
	Value v1(0);
	Test_Expect_Eq(static_cast<Vector<float>>(v1), Vector<float>());

	Value v2(1);
	Test_Expect_Eq(static_cast<Vector<float>>(v2), Vector<float>());

	Value v3(-1);
	Test_Expect_Eq(static_cast<Vector<float>>(v3), Vector<float>());
}

Case(CastFloatToInt) {
	Value v1(0.0f);
	Test_Expect_Eq(static_cast<int>(v1), 0);

	Value v2(1.0f);
	Test_Expect_Eq(static_cast<int>(v2), 1);

	Value v3(-1.0f);
	Test_Expect_Eq(static_cast<int>(v3), -1);
}

Case(CastFloatToFloat) {
	Value v1(0.0f);
	Test_Expect_Eq(static_cast<float>(v1), 0.0f);

	Value v2(1.0f);
	Test_Expect_Eq(static_cast<float>(v2), 1.0f);

	Value v3(-1.0f);
	Test_Expect_Eq(static_cast<float>(v3), -1.0f);
}

Case(CastFloatToBool) {
	Value v1(0.0f);
	Test_Expect_Eq(static_cast<bool>(v1), false);

	Value v2(1.0f);
	Test_Expect_Eq(static_cast<bool>(v2), true);

	Value v3(-1.0f);
	Test_Expect_Eq(static_cast<bool>(v3), true);
}

Case(CastFloatToString) {
	Value v1(0.0f);
	Test_Expect_Eq(static_cast<const char*>(v1), std::string());

	Value v2(1.0f);
	Test_Expect_Eq(static_cast<const char*>(v2), std::string());

	Value v3(-1.0f);
	Test_Expect_Eq(static_cast<const char*>(v3), std::string());
}

Case(CastFloatToVector) {
	Value v1(0.0f);
	Test_Expect_Eq(static_cast<Vector<float>>(v1), Vector<float>());

	Value v2(1.0f);
	Test_Expect_Eq(static_cast<Vector<float>>(v2), Vector<float>());

	Value v3(-1.0f);
	Test_Expect_Eq(static_cast<Vector<float>>(v3), Vector<float>());
}

Case(CastBoolToInt) {
	Value v1(true);
	Test_Expect_Eq(static_cast<int>(v1), 1);

	Value v2(false);
	Test_Expect_Eq(static_cast<int>(v2), 0);
}

Case(CastBoolToFloat) {
	Value v1(true);
	Test_Expect_Eq(static_cast<float>(v1), 1.0f);

	Value v2(false);
	Test_Expect_Eq(static_cast<float>(v2), 0.0f);
}

Case(CastBoolToBool) {
	Value v1(true);
	Test_Expect(static_cast<bool>(v1));

	Value v2(false);
	Test_Expect(!static_cast<bool>(v2));
}

Case(CastBoolToString) {
	Value v1(true);
	Test_Expect_Eq(static_cast<const char*>(v1), std::string());

	Value v2(false);
	Test_Expect_Eq(static_cast<const char*>(v2), std::string());
}

Case(CastBoolToVector) {
	Value v1(true);
	Test_Expect_Eq(static_cast<Vector<float>>(v1), Vector<float>());

	Value v2(false);
	Test_Expect_Eq(static_cast<Vector<float>>(v2), Vector<float>());
}

Case(CastStringToInt) {
	Value v1("Any string");
	Test_Expect_Eq(static_cast<int>(v1), -1);

	Value v2("Any other string");
	Test_Expect_Eq(static_cast<int>(v2), -1);

	Value v3("");
	Test_Expect_Eq(static_cast<int>(v3), -1);
}

Case(CastStringToFloat) {
	Value v1("Any string");
	Test_Expect_Eq(static_cast<float>(v1), 0.0f);

	Value v2("Any other string");
	Test_Expect_Eq(static_cast<float>(v2), 0.0f);

	Value v3("");
	Test_Expect_Eq(static_cast<float>(v3), 0.0f);
}

Case(CastStringToBool) {
	Value v1("Any string");
	Test_Expect(static_cast<bool>(v1));

	Value v2("Any other string");
	Test_Expect(static_cast<bool>(v2));

	Value v3("");
	Test_Expect(static_cast<bool>(v3));
}

Case(CastStringToString) {
	Value v1("Any string");
	Test_Expect_Eq(static_cast<const char*>(v1), std::string("Any string"));

	Value v2("Any other string");
	Test_Expect_Eq(static_cast<const char*>(v2), std::string("Any other string"));

	Value v3("");
	Test_Expect_Eq(static_cast<const char*>(v3), std::string());
}

Case(CastStringToVector) {
	Value v1("Any string");
	Test_Expect_Eq(static_cast<Vector<float>>(v1), Vector<float>());

	Value v2("Any other string");
	Test_Expect_Eq(static_cast<Vector<float>>(v2), Vector<float>());

	Value v3("");
	Test_Expect_Eq(static_cast<Vector<float>>(v3), Vector<float>());
}

Case(CastVectorToInt) {
	Value v1 = Value(Vector<float>());
	Test_Expect_Eq(static_cast<int>(v1), -1);

	Value v2(Vector<float>(10.0f, 20.0f, 30.0f));
	Test_Expect_Eq(static_cast<int>(v2), -1);

	Value v3(Vector<float>(-0.1f, 0.1f, 0.0f));
	Test_Expect_Eq(static_cast<int>(v3), -1);
}

Case(CastVectorToFloat) {
	Value v1 = Value(Vector<float>());
	Test_Expect_Eq(static_cast<float>(v1), 0.0f);

	Value v2(Vector<float>(10.0f, 20.0f, 30.0f));
	Test_Expect_Eq(static_cast<float>(v2), 0.0f);

	Value v3(Vector<float>(-0.1f, 0.1f, 0.0f));
	Test_Expect_Eq(static_cast<float>(v3), 0.0f);
}

Case(CastVectorToBool) {
	Value v1 = Value(Vector<float>());
	Test_Expect(static_cast<bool>(v1));

	Value v2(Vector<float>(10.0f, 20.0f, 30.0f));
	Test_Expect(static_cast<bool>(v2));

	Value v3(Vector<float>(-0.1f, 0.1f, 0.0f));
	Test_Expect(static_cast<bool>(v3));
}

Case(CastVectorToString) {
	Value v1 = Value(Vector<float>());
	Test_Expect_Eq(static_cast<const char*>(v1), std::string());

	Value v2(Vector<float>(10.0f, 20.0f, 30.0f));
	Test_Expect_Eq(static_cast<const char*>(v2), std::string());

	Value v3(Vector<float>(-0.1f, 0.1f, 0.0f));
	Test_Expect_Eq(static_cast<const char*>(v3), std::string());
}

Case(CastVectorToVector) {
	Value v1 = Value(Vector<float>());
	Test_Expect_Eq(static_cast<Vector<float>>(v1), Vector<float>());

	Value v2(Vector<float>(10.0f, 20.0f, 30.0f));
	Test_Expect_Eq(static_cast<Vector<float>>(v2), Vector<float>(10.0f, 20.0f, 30.0f));

	Value v3(Vector<float>(-0.1f, 0.1f, 0.0f));
	Test_Expect_Eq(static_cast<Vector<float>>(v3), Vector<float>(-0.1f, 0.1f, 0.0f));
}

EndSuite(Value_Test);

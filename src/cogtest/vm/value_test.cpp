#include <nullunit/nullunit.h>
#include "cog/vm/value.h"

using namespace gorc::cog::vm;
using namespace gorc::math;

BeginSuite(value_Test);

Case(CastIntToInt) {
	value v1(0);
	Test_Expect_Eq(static_cast<int>(v1), 0);

	value v2(1);
	Test_Expect_Eq(static_cast<int>(v2), 1);

	value v3(-1);
	Test_Expect_Eq(static_cast<int>(v3), -1);
}

Case(CastIntToFloat) {
	value v1(0);
	Test_Expect_Eq(static_cast<float>(v1), 0.0f);

	value v2(1);
	Test_Expect_Eq(static_cast<float>(v2), 1.0f);

	value v3(-1);
	Test_Expect_Eq(static_cast<float>(v3), -1.0f);
}

Case(CastIntToBool) {
	value v1(0);
	Test_Expect_Eq(static_cast<bool>(v1), false);

	value v2(1);
	Test_Expect_Eq(static_cast<bool>(v2), true);

	value v3(-1);
	Test_Expect_Eq(static_cast<bool>(v3), true);
}

Case(CastIntToString) {
	value v1(0);
	Test_Expect_Eq(static_cast<const char*>(v1), std::string());

	value v2(1);
	Test_Expect_Eq(static_cast<const char*>(v2), std::string());

	value v3(-1);
	Test_Expect_Eq(static_cast<const char*>(v3), std::string());
}

Case(CastIntToVector) {
	value v1(0);
	Test_Expect_Eq(static_cast<vector<3>>(v1), vector<3>());

	value v2(1);
	Test_Expect_Eq(static_cast<vector<3>>(v2), vector<3>());

	value v3(-1);
	Test_Expect_Eq(static_cast<vector<3>>(v3), vector<3>());
}

Case(CastFloatToInt) {
	value v1(0.0f);
	Test_Expect_Eq(static_cast<int>(v1), 0);

	value v2(1.0f);
	Test_Expect_Eq(static_cast<int>(v2), 1);

	value v3(-1.0f);
	Test_Expect_Eq(static_cast<int>(v3), -1);
}

Case(CastFloatToFloat) {
	value v1(0.0f);
	Test_Expect_Eq(static_cast<float>(v1), 0.0f);

	value v2(1.0f);
	Test_Expect_Eq(static_cast<float>(v2), 1.0f);

	value v3(-1.0f);
	Test_Expect_Eq(static_cast<float>(v3), -1.0f);
}

Case(CastFloatToBool) {
	value v1(0.0f);
	Test_Expect_Eq(static_cast<bool>(v1), false);

	value v2(1.0f);
	Test_Expect_Eq(static_cast<bool>(v2), true);

	value v3(-1.0f);
	Test_Expect_Eq(static_cast<bool>(v3), true);
}

Case(CastFloatToString) {
	value v1(0.0f);
	Test_Expect_Eq(static_cast<const char*>(v1), std::string());

	value v2(1.0f);
	Test_Expect_Eq(static_cast<const char*>(v2), std::string());

	value v3(-1.0f);
	Test_Expect_Eq(static_cast<const char*>(v3), std::string());
}

Case(CastFloatToVector) {
	value v1(0.0f);
	Test_Expect_Eq(static_cast<vector<3>>(v1), vector<3>());

	value v2(1.0f);
	Test_Expect_Eq(static_cast<vector<3>>(v2), vector<3>());

	value v3(-1.0f);
	Test_Expect_Eq(static_cast<vector<3>>(v3), vector<3>());
}

Case(CastBoolToInt) {
	value v1(true);
	Test_Expect_Eq(static_cast<int>(v1), 1);

	value v2(false);
	Test_Expect_Eq(static_cast<int>(v2), 0);
}

Case(CastBoolToFloat) {
	value v1(true);
	Test_Expect_Eq(static_cast<float>(v1), 1.0f);

	value v2(false);
	Test_Expect_Eq(static_cast<float>(v2), 0.0f);
}

Case(CastBoolToBool) {
	value v1(true);
	Test_Expect(static_cast<bool>(v1));

	value v2(false);
	Test_Expect(!static_cast<bool>(v2));
}

Case(CastBoolToString) {
	value v1(true);
	Test_Expect_Eq(static_cast<const char*>(v1), std::string());

	value v2(false);
	Test_Expect_Eq(static_cast<const char*>(v2), std::string());
}

Case(CastBoolToVector) {
	value v1(true);
	Test_Expect_Eq(static_cast<vector<3>>(v1), vector<3>());

	value v2(false);
	Test_Expect_Eq(static_cast<vector<3>>(v2), vector<3>());
}

Case(CastStringToInt) {
	value v1("Any string");
	Test_Expect_Eq(static_cast<int>(v1), -1);

	value v2("Any other string");
	Test_Expect_Eq(static_cast<int>(v2), -1);

	value v3("");
	Test_Expect_Eq(static_cast<int>(v3), -1);
}

Case(CastStringToFloat) {
	value v1("Any string");
	Test_Expect_Eq(static_cast<float>(v1), 0.0f);

	value v2("Any other string");
	Test_Expect_Eq(static_cast<float>(v2), 0.0f);

	value v3("");
	Test_Expect_Eq(static_cast<float>(v3), 0.0f);
}

Case(CastStringToBool) {
	value v1("Any string");
	Test_Expect(static_cast<bool>(v1));

	value v2("Any other string");
	Test_Expect(static_cast<bool>(v2));

	value v3("");
	Test_Expect(static_cast<bool>(v3));
}

Case(CastStringToString) {
	value v1("Any string");
	Test_Expect_Eq(static_cast<const char*>(v1), std::string("Any string"));

	value v2("Any other string");
	Test_Expect_Eq(static_cast<const char*>(v2), std::string("Any other string"));

	value v3("");
	Test_Expect_Eq(static_cast<const char*>(v3), std::string());
}

Case(CastStringToVector) {
	value v1("Any string");
	Test_Expect_Eq(static_cast<vector<3>>(v1), zero<3>());

	value v2("Any other string");
	Test_Expect_Eq(static_cast<vector<3>>(v2), zero<3>());

	value v3("");
	Test_Expect_Eq(static_cast<vector<3>>(v3), zero<3>());
}

Case(CastVectorToInt) {
	value v1 = value(zero<3>());
	Test_Expect_Eq(static_cast<int>(v1), -1);

	value v2(make_vector(10.0f, 20.0f, 30.0f));
	Test_Expect_Eq(static_cast<int>(v2), -1);

	value v3(make_vector(-0.1f, 0.1f, 0.0f));
	Test_Expect_Eq(static_cast<int>(v3), -1);
}

Case(CastVectorToFloat) {
	value v1 = value(zero<3>());
	Test_Expect_Eq(static_cast<float>(v1), 0.0f);

	value v2(make_vector(10.0f, 20.0f, 30.0f));
	Test_Expect_Eq(static_cast<float>(v2), 0.0f);

	value v3(make_vector(-0.1f, 0.1f, 0.0f));
	Test_Expect_Eq(static_cast<float>(v3), 0.0f);
}

Case(CastVectorToBool) {
	value v1 = value(zero<3>());
	Test_Expect(static_cast<bool>(v1));

	value v2(make_vector(10.0f, 20.0f, 30.0f));
	Test_Expect(static_cast<bool>(v2));

	value v3(make_vector(-0.1f, 0.1f, 0.0f));
	Test_Expect(static_cast<bool>(v3));
}

Case(CastVectorToString) {
	value v1 = value(zero<3>());
	Test_Expect_Eq(static_cast<const char*>(v1), std::string());

	value v2(make_vector(10.0f, 20.0f, 30.0f));
	Test_Expect_Eq(static_cast<const char*>(v2), std::string());

	value v3(make_vector(-0.1f, 0.1f, 0.0f));
	Test_Expect_Eq(static_cast<const char*>(v3), std::string());
}

Case(CastVectorToVector) {
	value v1 = value(zero<3>());
	Test_Expect_Eq(static_cast<vector<3>>(v1), zero<3>());

	value v2(make_vector(10.0f, 20.0f, 30.0f));
	Test_Expect_Eq(static_cast<vector<3>>(v2), make_vector(10.0f, 20.0f, 30.0f));

	value v3(make_vector(-0.1f, 0.1f, 0.0f));
	Test_Expect_Eq(static_cast<vector<3>>(v3), make_vector(-0.1f, 0.1f, 0.0f));
}

EndSuite(value_Test);

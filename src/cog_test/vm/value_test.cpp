#include "test/test.h"
#include "cog/vm/value.h"

using namespace gorc::cog::vm;
using namespace gorc::math;

begin_suite(value_Test);

test_case(CastIntToInt) {
    value v1(0);
    expect_eq(static_cast<int>(v1), 0);

    value v2(1);
    expect_eq(static_cast<int>(v2), 1);

    value v3(-1);
    expect_eq(static_cast<int>(v3), -1);
}

test_case(CastIntToFloat) {
    value v1(0);
    expect_eq(static_cast<float>(v1), 0.0f);

    value v2(1);
    expect_eq(static_cast<float>(v2), 1.0f);

    value v3(-1);
    expect_eq(static_cast<float>(v3), -1.0f);
}

test_case(CastIntToBool) {
    value v1(0);
    expect_eq(static_cast<bool>(v1), false);

    value v2(1);
    expect_eq(static_cast<bool>(v2), true);

    value v3(-1);
    expect_eq(static_cast<bool>(v3), true);
}

test_case(CastIntToString) {
    value v1(0);
    expect_eq(static_cast<const char*>(v1), std::string());

    value v2(1);
    expect_eq(static_cast<const char*>(v2), std::string());

    value v3(-1);
    expect_eq(static_cast<const char*>(v3), std::string());
}

test_case(CastIntToVector) {
    auto zerov = make_zero_vector<3, float>();

    value v1(0);
    expect_eq(static_cast<vector<3>>(v1), zerov);

    value v2(1);
    expect_eq(static_cast<vector<3>>(v2), zerov);

    value v3(-1);
    expect_eq(static_cast<vector<3>>(v3), zerov);
}

test_case(CastFloatToInt) {
    value v1(0.0f);
    expect_eq(static_cast<int>(v1), 0);

    value v2(1.0f);
    expect_eq(static_cast<int>(v2), 1);

    value v3(-1.0f);
    expect_eq(static_cast<int>(v3), -1);
}

test_case(CastFloatToFloat) {
    value v1(0.0f);
    expect_eq(static_cast<float>(v1), 0.0f);

    value v2(1.0f);
    expect_eq(static_cast<float>(v2), 1.0f);

    value v3(-1.0f);
    expect_eq(static_cast<float>(v3), -1.0f);
}

test_case(CastFloatToBool) {
    value v1(0.0f);
    expect_eq(static_cast<bool>(v1), false);

    value v2(1.0f);
    expect_eq(static_cast<bool>(v2), true);

    value v3(-1.0f);
    expect_eq(static_cast<bool>(v3), true);
}

test_case(CastFloatToString) {
    value v1(0.0f);
    expect_eq(static_cast<const char*>(v1), std::string());

    value v2(1.0f);
    expect_eq(static_cast<const char*>(v2), std::string());

    value v3(-1.0f);
    expect_eq(static_cast<const char*>(v3), std::string());
}

test_case(CastFloatToVector) {
    auto zerov = make_zero_vector<3, float>();

    value v1(0.0f);
    expect_eq(static_cast<vector<3>>(v1), zerov);

    value v2(1.0f);
    expect_eq(static_cast<vector<3>>(v2), zerov);

    value v3(-1.0f);
    expect_eq(static_cast<vector<3>>(v3), zerov);
}

test_case(CastBoolToInt) {
    value v1(true);
    expect_eq(static_cast<int>(v1), 1);

    value v2(false);
    expect_eq(static_cast<int>(v2), 0);
}

test_case(CastBoolToFloat) {
    value v1(true);
    expect_eq(static_cast<float>(v1), 1.0f);

    value v2(false);
    expect_eq(static_cast<float>(v2), 0.0f);
}

test_case(CastBoolToBool) {
    value v1(true);
    expect_true(static_cast<bool>(v1));

    value v2(false);
    expect_true(!static_cast<bool>(v2));
}

test_case(CastBoolToString) {
    value v1(true);
    expect_eq(static_cast<const char*>(v1), std::string());

    value v2(false);
    expect_eq(static_cast<const char*>(v2), std::string());
}

test_case(CastBoolToVector) {
    auto zerov = make_zero_vector<3, float>();

    value v1(true);
    expect_eq(static_cast<vector<3>>(v1), zerov);

    value v2(false);
    expect_eq(static_cast<vector<3>>(v2), zerov);
}

test_case(CastStringToInt) {
    value v1("Any string");
    expect_eq(static_cast<int>(v1), -1);

    value v2("Any other string");
    expect_eq(static_cast<int>(v2), -1);

    value v3("");
    expect_eq(static_cast<int>(v3), -1);
}

test_case(CastStringToFloat) {
    value v1("Any string");
    expect_eq(static_cast<float>(v1), 0.0f);

    value v2("Any other string");
    expect_eq(static_cast<float>(v2), 0.0f);

    value v3("");
    expect_eq(static_cast<float>(v3), 0.0f);
}

test_case(CastStringToBool) {
    value v1("Any string");
    expect_true(static_cast<bool>(v1));

    value v2("Any other string");
    expect_true(static_cast<bool>(v2));

    value v3("");
    expect_true(static_cast<bool>(v3));
}

test_case(CastStringToString) {
    value v1("Any string");
    expect_eq(static_cast<const char*>(v1), std::string("Any string"));

    value v2("Any other string");
    expect_eq(static_cast<const char*>(v2), std::string("Any other string"));

    value v3("");
    expect_eq(static_cast<const char*>(v3), std::string());
}

test_case(CastStringToVector) {
    auto zerov = make_zero_vector<3, float>();

    value v1("Any string");
    expect_eq(static_cast<vector<3>>(v1), zerov);

    value v2("Any other string");
    expect_eq(static_cast<vector<3>>(v2), zerov);

    value v3("");
    expect_eq(static_cast<vector<3>>(v3), zerov);
}

test_case(CastVectorToInt) {
    value v1 = value(make_zero_vector<3, float>());
    expect_eq(static_cast<int>(v1), -1);

    value v2(make_vector(10.0f, 20.0f, 30.0f));
    expect_eq(static_cast<int>(v2), -1);

    value v3(make_vector(-0.1f, 0.1f, 0.0f));
    expect_eq(static_cast<int>(v3), -1);
}

test_case(CastVectorToFloat) {
    value v1 = value(make_zero_vector<3, float>());
    expect_eq(static_cast<float>(v1), 0.0f);

    value v2(make_vector(10.0f, 20.0f, 30.0f));
    expect_eq(static_cast<float>(v2), 0.0f);

    value v3(make_vector(-0.1f, 0.1f, 0.0f));
    expect_eq(static_cast<float>(v3), 0.0f);
}

test_case(CastVectorToBool) {
    value v1 = value(make_zero_vector<3, float>());
    expect_true(static_cast<bool>(v1));

    value v2(make_vector(10.0f, 20.0f, 30.0f));
    expect_true(static_cast<bool>(v2));

    value v3(make_vector(-0.1f, 0.1f, 0.0f));
    expect_true(static_cast<bool>(v3));
}

test_case(CastVectorToString) {
    value v1 = value(make_zero_vector<3, float>());
    expect_eq(static_cast<const char*>(v1), std::string());

    value v2(make_vector(10.0f, 20.0f, 30.0f));
    expect_eq(static_cast<const char*>(v2), std::string());

    value v3(make_vector(-0.1f, 0.1f, 0.0f));
    expect_eq(static_cast<const char*>(v3), std::string());
}

test_case(CastVectorToVector) {
    auto zerov = make_zero_vector<3, float>();

    value v1 = value(make_zero_vector<3, float>());
    expect_eq(static_cast<vector<3>>(v1), zerov);

    value v2(make_vector(10.0f, 20.0f, 30.0f));
    expect_eq(static_cast<vector<3>>(v2), make_vector(10.0f, 20.0f, 30.0f));

    value v3(make_vector(-0.1f, 0.1f, 0.0f));
    expect_eq(static_cast<vector<3>>(v3), make_vector(-0.1f, 0.1f, 0.0f));
}

end_suite(value_Test);

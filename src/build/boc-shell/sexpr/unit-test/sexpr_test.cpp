#include "test/test.hpp"
#include "../sexpr.hpp"

using namespace gorc;

begin_suite(sexpr_test);

test_case(make_nil)
{
    sexpr e = make_sexpr();
    assert_eq(e->type, sexpr_node_type::atom);
    assert_true(e->atom.empty());
    assert_true(!e->left);
    assert_true(!e->right);
}

test_case(make_atom)
{
    sexpr e = make_sexpr("foobar");
    assert_eq(e->type, sexpr_node_type::atom);
    assert_eq(e->atom, std::string("foobar"));
    assert_true(!e->left);
    assert_true(!e->right);
}

test_case(make_bool_atom)
{
    sexpr e = make_sexpr(true);
    assert_eq(e->type, sexpr_node_type::atom);
    assert_eq(e->atom, std::string("true"));
    assert_true(!e->left);
    assert_true(!e->right);

    sexpr f = make_sexpr(false);
    assert_eq(f->type, sexpr_node_type::atom);
    assert_eq(f->atom, std::string("false"));
    assert_true(!f->left);
    assert_true(!f->right);
}

test_case(make_list)
{
    sexpr l = make_sexpr();
    sexpr r = make_sexpr();
    sexpr e = make_sexpr(l, r);

    assert_eq(e->type, sexpr_node_type::cons);
    assert_true(e->atom.empty());
    assert_eq(e->left, l);
    assert_eq(e->right, r);
}

test_case(nil_props)
{
    assert_true(atom(make_sexpr()));
    assert_true(null(make_sexpr()));
}

test_case(atom_props)
{
    assert_true(atom(make_sexpr("foo")));
    assert_true(!null(make_sexpr("foo")));
}

test_case(cons_props)
{
    sexpr e = make_sexpr(make_sexpr(), make_sexpr());

    assert_true(!atom(e));
    assert_true(!null(e));
}

test_case(car_atom)
{
    sexpr e = make_sexpr("foo");

    assert_throws_logged(car(e));
    assert_log_message(log_level::error, "expected data structure, found atom");
    assert_log_empty();
}

test_case(cdr_atom)
{
    sexpr e = make_sexpr("foo");

    assert_throws_logged(cdr(e));
    assert_log_message(log_level::error, "expected data structure, found atom");
    assert_log_empty();
}

test_case(car_cdr_simple)
{
    sexpr l = make_sexpr("foo");
    sexpr r = make_sexpr("bar");
    sexpr e = make_sexpr(l, r);

    assert_eq(car(e), l);
    assert_eq(cdr(e), r);
}

test_case(make_sexpr_from_empty_range)
{
    std::vector<std::string> rng;
    sexpr e = make_sexpr_from_range(rng);

    assert_true(null(e));
}

test_case(make_sexpr_from_range_simple)
{
    std::vector<std::string> rng({ "foo", "bar", "baz" });
    sexpr e = make_sexpr_from_range(rng);

    assert_eq(car(e)->atom, std::string("foo"));
    assert_eq(car(cdr(e))->atom, std::string("bar"));
    assert_eq(car(cdr(cdr(e)))->atom, std::string("baz"));
    assert_true(null(cdr(cdr(cdr(e)))));
}

end_suite(sexpr_test);

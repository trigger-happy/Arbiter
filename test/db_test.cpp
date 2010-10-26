#include <iostream>
#include <string>

#define BOOST_TEST_MODULE DBTest
#include <boost/test/included/unit_test.hpp>

#include "../src/server/sqlitedb.h"

using namespace std;

static SqliteDB* g_db = NULL;

struct fixture{
	fixture();
	~fixture();

	SqliteDB db;
};

fixture::fixture(){
	g_db = &db;
	g_db->open("test.db");
	
	User usr;
	usr.username = "Test";
	usr.password = "Test";
	usr.teamname = "hello";
	usr.acl = "c";
	g_db->add_user(usr);
}

fixture::~fixture(){
	g_db = NULL;
}

BOOST_GLOBAL_FIXTURE(fixture);

BOOST_AUTO_TEST_CASE(open_test){
	g_db->open("test.db");
	BOOST_REQUIRE(g_db->is_open());
}

BOOST_AUTO_TEST_CASE(auth_test){
	bool should_work = g_db->authenticate("Test", "Test");
	BOOST_CHECK(should_work);

	bool should_fail = g_db->authenticate("Hi", "Hi");
	BOOST_CHECK(!should_fail);
}

BOOST_AUTO_TEST_CASE(acl_test){
	string acl = g_db->get_acl("Test");
	BOOST_CHECK(acl == "c");

	acl = g_db->get_acl("Hi");
	BOOST_CHECK(acl == "");
}

BOOST_AUTO_TEST_CASE(close_test){
	g_db->close();
	BOOST_REQUIRE(!g_db->is_open());
}
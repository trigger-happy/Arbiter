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

	SqliteDB* db;
};

fixture::fixture(){
	// yeah, this is a weird initialization setup but deal with it
	g_db = new SqliteDB();
	g_db->open("test.db");
	
	User usr;
	usr.username = "Test";
	usr.password = "Test";
	usr.teamname = "hello";
	usr.acl = "c";
	g_db->add_user(usr);
	
	delete g_db;
	g_db = new SqliteDB();
}

fixture::~fixture(){
	delete g_db;
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

BOOST_AUTO_TEST_CASE(update_test){
	User usr;
	usr.username = "Test";
	usr.password = "newpass";
	usr.acl = "j";
	usr.teamname = "";
	g_db->update_user(usr);

	bool should_work = g_db->authenticate(usr.username, usr.password);
	BOOST_CHECK(should_work);

	should_work = false;
	try{
		usr.username = "non_existent";
		g_db->update_user(usr);
	}catch(db_error& e){
		BOOST_CHECK(*boost::get_error_info<err_info>(e) == "User not found");
		should_work = true;
	}
	BOOST_CHECK(should_work);
}

BOOST_AUTO_TEST_CASE(delete_test){
	bool should_work = true;
	try{
		g_db->delete_user("Test");
	}catch(...){
		should_work = false;
	}

	BOOST_CHECK(should_work);

	should_work = false;
	try{
		g_db->delete_user("Test");
	}catch(db_error& e){
		BOOST_CHECK(*boost::get_error_info<err_info>(e) == "User not found");
		should_work = true;
	}
	
	BOOST_CHECK(should_work);
}

BOOST_AUTO_TEST_CASE(get_users_test){
	// add some users to get things running
	vector<User> orig, result;
	for(int i = 0; i < 4; ++i){
		User temp;
		temp.username = boost::lexical_cast<string>(i);
		temp.password = boost::lexical_cast<string>(i);
		orig.push_back(temp);
		g_db->add_user(temp);
	}

	g_db->get_users(result);
	for(int i = 0; i < 4; ++i){
		BOOST_CHECK(orig[i].username == result[i].username);
	}
}

BOOST_AUTO_TEST_CASE(language_test){
	// unlike the user test cases, this one does everything in 1 go
	Language l;
	l.compile_cmd = "g++";
	l.link_cmd = "ld";
	l.name = "C++";
	l.run_cmd = "./";

	g_db->add_language(l);

	vector<Language> lv;
	g_db->get_languages(lv);

	BOOST_CHECK(lv.size() == 1);

	if(lv.size() == 1){
		BOOST_CHECK(l.compile_cmd == lv[0].compile_cmd);
		// no need to check for the rest of the stuff
	}

	g_db->delete_language(l.name);

	bool exception_thrown = false;
	try{
		g_db->delete_language(l.name);
	}catch(db_error& e){
		exception_thrown = true;
	}
	BOOST_CHECK(exception_thrown);
}

BOOST_AUTO_TEST_CASE(close_test){
	g_db->close();
	BOOST_REQUIRE(!g_db->is_open());
}
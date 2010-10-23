#include <iostream>
#include <boost/test/minimal.hpp>
#include <Wt/Dbo/backend/Sqlite3>
#include <Wt/Dbo/Dbo>
#include "../src/common/datatypes_orm.h"

using namespace Wt::Dbo;
using namespace std;

int test_main(int argc, char* argv[]){
	backend::Sqlite3 sql3("test.db");
	Session s;
	s.setConnection(sql3);
	
	s.mapClass<User>("users");
	s.mapClass<Language>("languages");
	s.mapClass<Clarification>("clars");
	s.mapClass<Run>("runs");
	s.mapClass<Problem>("problems");
	s.mapClass<File>("files");
	
	bool exp_thrown = false;
	try{
		s.createTables();
	}catch(...){
		exp_thrown = true;
		BOOST_ERROR("Tables already created");
	}
	
	BOOST_CHECK(exp_thrown == false);
	
	try{
		s.createTables();
	}catch(...){
		exp_thrown = true;
	}
	BOOST_REQUIRE(exp_thrown = true);
	
	Transaction t(s);
	User* usr = new User();
	usr->username = "Test";
	usr->password = "Test";
	usr->teamname = "Hello";
	usr->acl = "c";
	auto uptr = s.add(usr);
	
	ptr<User> uqry = s.find<User>().where("username = ?").bind("Test");
	BOOST_REQUIRE(uqry->teamname == "Hello");
	
	uptr.modify()->teamname = "Hi";
	uptr.flush();
	
	uqry = s.find<User>().where("username = ?").bind("Test");
	BOOST_REQUIRE(uqry->teamname == "Hi");
	
	Language* lang = new Language();
	lang->compile_cmd = "g++ -c @infile";
	lang->link_cmd = "g++ -o @outfile @infile";
	lang->run_cmd = "./@exefile";
	lang->name = "C++";
	ptr<Language> lang_ptr = s.add(lang);
	
	ptr<Language> lang_qry = s.find<Language>().where("name = ?").bind("C++");
	BOOST_REQUIRE(lang_qry->run_cmd == lang->run_cmd);
	
	User* judge = new User();
	judge->acl = "j";
	judge->password = "thejudge";
	judge->username = "thejudge";
	auto j_ptr = s.add(judge);
	
	Run* r = new Run();
	r->contestant = uptr;
	//r->judge = j_ptr;
	r->lang = lang_ptr;
	r->problem_id = 1;
	r->submit_time = 0;
	auto r_ptr = s.add(r);
	
	Problem* prob = new Problem();
	prob->author = "Bob";
	prob->time_limit = 1000;
	prob->title = "Stacking Cylinders";
	ptr<Problem> p_ptr = s.add(prob);
	
	File* in_file = new File();
	File* out_file = new File();
	in_file->file = "cylinders.in";
	out_file->file = "cylinders.out";
	in_file->problem = p_ptr;
	out_file->problem = p_ptr;
	
	p_ptr.modify()->files.insert(s.add(in_file));
	p_ptr.modify()->files.insert(s.add(out_file));
	
	ptr<Problem> p_qry = s.find<Problem>().where("time_limit = ?").bind(1000);
	BOOST_CHECK(p_qry->files.size() == 2);
	
	BOOST_CHECK(t.commit());
	return 0;
}
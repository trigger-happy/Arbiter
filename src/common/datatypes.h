#ifndef DATATYPES_H
#define DATATYPES_H
#include <string>
#include <vector>
#include <boost/cstdint.hpp>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/SqlTraits>

struct Contest{
	std::string name;
	std::vector<std::string> problems;
	boost::uint32_t time_limit; // in seconds
	
	struct DB{
		std::string location;
		std::string type;
		std::string username;
		std::string password;
		std::string db_name; // for those server type databases
	} db;
};

struct Problem;

struct File{
	Wt::Dbo::ptr<Problem> problem;
	std::string file;
	
	enum class FILE_TYPE : boost::uint8_t{
		INPUT_FILE = 0,
		OUTPUT_FILE
	} file_type;
	
	template<typename Action>
	void persist(Action& a){
		Wt::Dbo::belongsTo(a, problem, "problem");
		Wt::Dbo::field(a, file, "file");
		Wt::Dbo::field(a, reinterpret_cast<boost::int32_t&>(file_type), "file_type");
	}
};

struct Problem{
	std::string author;
	std::string title;
	Wt::Dbo::collection<Wt::Dbo::ptr<File> > files;
	
	enum class checking_type : boost::uint8_t{
		DIFF = 0,
		CHECKER = 1
	} ctype;
	
	boost::uint32_t time_limit;
	
	template<typename Action>
	void persist(Action& a){
		Wt::Dbo::field(a, author, "author");
		Wt::Dbo::field(a, title, "title");
		Wt::Dbo::field(a, reinterpret_cast<boost::int32_t&>(ctype), "checking_type");
		Wt::Dbo::field(a, reinterpret_cast<boost::int32_t&>(time_limit), "time_limit");
		
		Wt::Dbo::hasMany(a, files, Wt::Dbo::ManyToOne, "problem");
	}
};

struct User{
	std::string username;
	std::string password;
	std::string teamname;
	std::string acl;
	
	template<typename Action>
	void persist(Action& a){
		Wt::Dbo::field(a, username, "username");
		Wt::Dbo::field(a, password, "password");
		Wt::Dbo::field(a, teamname, "teamname");
		Wt::Dbo::field(a, acl, "acl");
	}
};

struct Language{
	std::string name;
	std::string compile_cmd;
	std::string link_cmd;
	std::string run_cmd;
	
	template<typename Action>
	void persist(Action& a){
		Wt::Dbo::field(a, name, "name");
		Wt::Dbo::field(a, compile_cmd, "compile_cmd");
		Wt::Dbo::field(a, link_cmd, "link_cmd");
		Wt::Dbo::field(a, run_cmd, "run_cmd");
	}
};

struct User;

struct Clarification{
	Wt::Dbo::ptr<User> asker;
	Wt::Dbo::ptr<User> answerer;
	boost::uint64_t ask_time; // in milliseconds since the contest start
	std::string question;
	std::string answer;
	boost::int32_t category;
	
	template<typename Action>
	void persist(Action& a){
		Wt::Dbo::belongsTo(a, asker, "asker");
		Wt::Dbo::belongsTo(a, answerer, "answerer");
		Wt::Dbo::field(a, reinterpret_cast<double&>(ask_time), "ask_time");
		Wt::Dbo::field(a, question, "question");
		Wt::Dbo::field(a, answer, "answer");
		Wt::Dbo::field(a, category, "category");
	}
};

struct Run{
	Wt::Dbo::ptr<User> contestant;
	Wt::Dbo::ptr<User> judge;
	Wt::Dbo::ptr<Language> lang;
	boost::uint64_t submit_time; // in milliseconds since the contest start
	boost::uint32_t problem_id;
	
	enum class RESPONSE : boost::uint8_t{
		YES = 0,
		NO,
		COMPILE_ERROR,
		RUNTIME_ERROR,
		TIMELIMIT_EXCEEDED,
		OUTPUT_FORMAT_ERROR,
		CONTACT_STAFF
	} response;
	
	enum class STATUS : boost::uint8_t{
		PENDING = 0,
		JUDGED
	} status;
	
	template<typename Action>
	void persist(Action& a){
		Wt::Dbo::belongsTo(a, contestant, "contestant");
		Wt::Dbo::belongsTo(a, judge, "judge");
		Wt::Dbo::belongsTo(a, lang, "lang");
		Wt::Dbo::field(a, reinterpret_cast<double&>(submit_time), "submit_time");
		Wt::Dbo::field(a, reinterpret_cast<boost::int32_t&>(problem_id), "problem_id");
		Wt::Dbo::field(a, reinterpret_cast<boost::int32_t&>(response), "response");
		Wt::Dbo::field(a, reinterpret_cast<boost::int32_t&>(status), "status");
	}
};

#endif // DATATYPES_H
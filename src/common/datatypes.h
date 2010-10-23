#ifndef DATATYPES_H
#define DATATYPES_H
#include <string>
#include <vector>
#include <boost/cstdint.hpp>
#include <Wt/Dbo/Dbo>

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
	};
};

struct Problem{	
	boost::uint32_t id;
	std::string author;
	std::string title;
	std::vector<std::string> input_files;
	std::vector<std::string> output_files;
	
	enum class checking_type : boost::uint8_t{
		DIFF = 0,
		CHECKER = 1
	};
	
	std::string run_cmd;
	boost::uint32_t time_limit;
};

struct User{
	boost::uint32_t id;
	std::string username;
	std::string password;
	std::string teamname;
	std::string acl;
	
	template<typename Action>
	void persist(Action& a){
		Wt::Dbo::field(a, id, "id");
		Wt::Dbo::field(a, username, "username");
		Wt::Dbo::field(a, password, "password");
		Wt::Dbo::field(a, teamname, "teamname");
		Wt::Dbo::field(a, acl, "acl");
	}
};

struct Language{
	boost::uint32_t id;
	std::string name;
	std::string compile_cmd;
	std::string link_cmd;
	std::string run_cmd;
	
	template<typename Action>
	void persist(Action& a){
		Wt::Dbo::field(a, id, "id");
		Wt::Dbo::field(a, name, "name");
		Wt::Dbo::field(a, compile_cmd, "compile_cmd");
		Wt::Dbo::field(a, link_cmd, "link_cmd");
		Wt::Dbo::field(a, run_cmd, "run_cmd");
	}
};

struct Clarification{
	boost::uint32_t id;
	boost::int32_t asker_id;
	boost::int32_t answerer_id;
	boost::uint64_t ask_time; // in milliseconds since the contest start
	std::string question;
	std::string answer;
	boost::int32_t category;
	
	template<typename Action>
	void persist(Action& a){
		Wt::Dbo::field(a, id, "id");
		Wt::Dbo::field(a, asker_id, "asker_id");
		Wt::Dbo::field(a, answerer_id, "answerer_id");
		Wt::Dbo::field(a, ask_time, "ask_time");
		Wt::Dbo::field(a, question, "question");
		Wt::Dbo::field(a, answer, "answer");
		Wt::Dbo::field(a, category, "category");
	}
};

struct Run{
	boost::uint32_t id;
	boost::uint32_t contestant_id;
	boost::uint64_t submit_time; // in milliseconds since the contest start
	boost::uint32_t problem_id;
	boost::uint32_t judge_id;
	
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
		Wt::Dbo::field(a, id, "id");
		Wt::Dbo::field(a, contestant_id, "contestant_id");
		Wt::Dbo::field(a, submit_time, "submit_time");
		Wt::Dbo::field(a, problem_id, "submit_time");
		Wt::Dbo::field(a, judge_id, "judge_id");
		Wt::Dbo::field(a, response, "response");
		Wt::Dbo::field(a, status, "status");
	}
};

#endif // DATATYPES_H
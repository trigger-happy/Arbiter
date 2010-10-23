#ifndef DATATYPES_H
#define DATATYPES_H
#include <string>
#include <vector>
#include <boost/cstdint.hpp>

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
};

struct Language{
	boost::uint32_t id;
	std::string name;
	std::string compile_cmd;
	std::string link_cmd;
	std::string run_cmd;
};

struct Clarification{
	boost::uint32_t id;
	boost::int32_t asker_id;
	boost::int32_t answerer_id;
	boost::uint64_t ask_time; // in milliseconds since the contest start
	std::string question;
	std::string answer;
	boost::int32_t category;
};

struct Run{
	boost::uint32_t id;
	boost::uint32_t contestant_id;
	boost::uint64_t submit_time; // in milliseconds since the contest start
	boost::uint32_t problem_id;
	boost::uint32_t judge_id;
	
	enum class response : boost::uint8_t{
		YES = 0,
		NO,
		COMPILE_ERROR,
		RUNTIME_ERROR,
		TIMELIMIT_EXCEEDED,
		OUTPUT_FORMAT_ERROR,
		CONTACT_STAFF
	};
	
	enum class status : boost::uint8_t{
		PENDING = 0,
		JUDGED
	};
};

#endif // DATATYPES_H
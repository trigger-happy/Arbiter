#ifndef DATATYPES_H
#define DATATYPES_H
#include <string>
#include <vector>
#include <boost/cstdint.hpp>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/SqlTraits>

namespace plain{

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

struct File{
	std::string file;
	
	enum class FILE_TYPE : boost::uint8_t{
		INPUT_FILE = 0,
		OUTPUT_FILE
	} file_type;
};

struct Problem{
	std::string author;
	std::string title;
	std::vector<File> files;
	
	enum class checking_type : boost::uint8_t{
		DIFF = 0,
		CHECKER = 1
	} ctype;
	
	boost::uint32_t time_limit;
};

struct User{
	std::string username;
	std::string password;
	std::string teamname;
	std::string acl;
};

struct Language{
	std::string name;
	std::string compile_cmd;
	std::string link_cmd;
	std::string run_cmd;
};

struct User;

struct Clarification{
	std::string asker;
	std::string answerer;
	boost::uint64_t ask_time; // in milliseconds since the contest start
	std::string question;
	std::string answer;
	boost::int32_t category;
};

struct Run{
	User* contestant;
	User* judge;
	Language* lang;
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
};

}

#endif // DATATYPES_H
#ifndef DB_H
#define DB_H
#include <string>
#include <vector>
#include <boost/exception/all.hpp>
#include "../common/datatypes.h"

typedef boost::error_info<struct tag_db_error_info, std::string> err_info;

struct db_error : virtual boost::exception, virtual std::exception{};

class DB{
public:
	/*!
	Destructor
	*/
	virtual ~DB(){}

	/*!
	Open the database connection
	\param location The path to the database (either server URL or file)
	\param username The username to use to connect to the DB (if applicable)
	\param password The password to use to connect to the DB (if applicable)
	\param dbname The name of the database (if applicable)
	\exception db_error is thrown when an error occurs
	*/
	virtual void open(const std::string& location,
					  const std::string& username = "",
					  const std::string& password = "",
					  const std::string& dbname = "") = 0;

	/*!
	Close the database connection
	\note Does nothing other than set a flag at the moment
	*/
	virtual void close() = 0;

	/*!
	Authenticate the user given the username and password
	\param username The username
	\param password The password
	\return If the username and password matches the one in the datbase
	*/
	virtual bool authenticate(const std::string& username,
							  const std::string& password) = 0;

	/*!
	Get the access control list of the specified user
	\param username The user to query
	\return An std::string containing the ACL if the user is found, empty otherwise
	*/
	virtual std::string get_acl(const std::string& username) = 0;

	/*!
	Check if the database connection is open
	\return true if connected, false otherwise
	*/
	inline bool is_open() const{
		return m_connected;
	}

	/*!
	Add a new user to the database
	\param user The user to add
	\exception db_error is thrown when the user exists
	*/
	virtual void add_user(User& user) = 0;

	/*!
	Delete the user with the specified username
	\param username The user to delete
	*/
	virtual void delete_user(const std::string& username) = 0;

	/*!
	Update the information of user (DB is searched using the username)
	\param user The user information to update into the DB
	\exception db_error is thrown when the user does not exist
	*/
	virtual void update_user(User& user) = 0;

	/*!
	Get all the users in the DB
	\param users A reference to a vector to fill up with users
	*/
	virtual void get_users(std::vector<User>& users) = 0;

	/*!
	Add a language to the database
	\param lang The language to add
	\note Unlike the user functions, this one updates existing languages
	*/
	virtual void add_language(Language& lang) = 0;

	/*!
	Delete a langauage from the database
	\param lname The name of the language to delete
	\exception db_error is thrown if the language doesn't exist
	*/
	virtual void delete_language(const std::string& lname) = 0;

	/*!
	Get all the languages in the database
	\param lv A reference to a vector to fill up with languages
	*/
	virtual void get_languages(std::vector<Language>& lv) = 0;
	
protected:
	bool m_connected;
};

#endif // DB_H

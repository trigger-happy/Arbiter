#ifndef DB_H
#define DB_H
#include <string>
#include "../common/datatypes.h"

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
	\exception std::exception is thrown when an error occurs
	*/
	virtual void open(const std::string& location,
					  const std::string& username = "",
					  const std::string& password = "",
					  const std::string& dbname = "") = 0;

	/*!
	Close the database connection
	\exception std::exception if the close fails
	*/
	virtual void close() = 0;

	/*!
	Authenticate the user given the username and password
	\param username The username
	\param password The password
	\return If the username and password matches the one in the datbase
	\exception std::exception is thrown when an error occurs
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
	\exception std::exception on error
	*/
	virtual void add_user(User& user) = 0;

protected:
	bool m_connected;
};

#endif // DB_H

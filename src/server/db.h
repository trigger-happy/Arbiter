#ifndef DB_H
#define DB_H
#include <string>
#include <vector>
#include <boost/exception/all.hpp>
#include "../common/datatypes_orm.h"
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
	virtual void add_user(plain::User& user) = 0;

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
	virtual void update_user(plain::User& user) = 0;

	/*!
	Get all the users in the DB
	\param users A reference to a vector to fill up with users
	*/
	virtual void get_users(std::vector<plain::User>& users) = 0;

	/*!
	Add a language to the database
	\param lang The language to add
	\note Unlike the user functions, this one updates existing languages
	*/
	virtual void add_language(plain::Language& lang) = 0;

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
	virtual void get_languages(std::vector<plain::Language>& lv) = 0;
	
	/*!
	Add a new clarification to the database
	\param clar The clarification to add.
	\exception db_error is thrown if the asker can't be found in the database.
	*/
	virtual void add_clarification(const plain::Clarification& clar) = 0;
								   
	/*!
	Get all the clarifications in the database
	\param clars A reference to a vector to fill up with clarifications
	*/
	virtual void get_clarifications(std::vector<plain::Clarification>& clars) = 0;
	
	/*!
	 * Add a new run
	 * \param run The run data
	 * \exception db_error is thrown if the contestant is not found in the DB.
	 */
	virtual void add_run(const plain::Run& run) = 0;
						 
	/*!
	 * Get all the runs in the DB
	 * \param rv The vector that will contain the run data
	 */
	virtual void get_runs(std::vector<plain::Run>& rv) = 0;

	/*!
	 * Add a file to the database
	 * \param f The file to add to the database.
	 */
	virtual void add_file(const plain::File& f) = 0;

	/*!
	 * Get all the files in the database
	 * \param vf A reference to a vector to fill up with file information.
	 */
	virtual void get_files(std::vector<plain::File>& vf) = 0;

	/*!
	 * Add a problem definition to the database, just make sure they're not personal ;)
	 * \param p The problem to add in the database
	 * \note p.files is automatically added to the database as well
	 */
	virtual void add_problem(const plain::Problem& p) = 0;

	/*!
	 * Get all the problem definitions in the database
	 * \param vp A reference to a vector that will contain the problems
	 */
	virtual void get_problems(std::vector<plain::Problem>& vp) = 0;
	
protected:
	bool m_connected;
};

#endif // DB_H

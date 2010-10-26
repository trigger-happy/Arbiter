#ifndef SQLITEDB_H
#define SQLITEDB_H
#include <string>
#include "db.h"
#include <Wt/Dbo/backend/Sqlite3>
#include <Wt/Dbo/Dbo>
#include <boost/scoped_ptr.hpp>


class SqliteDB : public DB{
public:
	SqliteDB();
    virtual std::string get_acl(const std::string& username);
    virtual bool authenticate(const std::string& username,
							  const std::string& password);
    virtual void close();
    virtual void open(const std::string& location,
					  const std::string& username = "",
					  const std::string& password = "",
					  const std::string& dbname = "");
	virtual void add_user(User& user);
	virtual void delete_user(const std::string& username);
	virtual void update_user(User& user);
	virtual void get_users(std::vector<User>& users);
	virtual void add_language(Language& lang);
	virtual void delete_language(const std::string& lname);
	virtual void get_languages(std::vector<Language>& lv);
	
private:
	Wt::Dbo::ptr<User> find_user(const std::string& user);

private:
	boost::scoped_ptr<Wt::Dbo::backend::Sqlite3> m_db;
	Wt::Dbo::Session m_session;
};

#endif // SQLITEDB_H

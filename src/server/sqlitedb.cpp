#include <exception>
#include "sqlitedb.h"
#include "../common/datatypes.h"

using namespace Wt::Dbo;
using namespace std;

SqliteDB::SqliteDB(){
	m_connected = false;
}

string SqliteDB::get_acl(const string& username){
	Transaction t(m_session);
	ptr<User> qry = m_session.find<User>().where("username = ?").bind(username);
	if(!qry){
		return "";
	}
	return qry->acl;
}

bool SqliteDB::authenticate(const string& username,
							const string& password){
	Transaction t(m_session);
	ptr<User> qry = find_user(username);
	if(!qry){
		return false;
	}
	if(qry->password == password){
		return true;
	}
	return false;
}

void SqliteDB::close(){
	m_connected = false;
}

void SqliteDB::open(const string& location,
					const string& username,
					const string& password,
					const string& dbname){
	if(!m_connected){
		m_db.reset(new backend::Sqlite3(location));
		m_session.setConnection(*m_db);

		// perform the mappings
		m_session.mapClass<User>("users");
		m_session.mapClass<Language>("languages");
		m_session.mapClass<Clarification>("clars");
		m_session.mapClass<Run>("runs");
		m_session.mapClass<Problem>("problems");
		m_session.mapClass<File>("files");

		// attempt to create the tables if they don't exist yet
		try{
			m_session.createTables();
		}catch(...){
			// we don't really have to care
		}

		// we're connected
		m_connected = true;
	}else{
		throw db_error() << err_info("Database already open");
	}
}

void SqliteDB::add_user(User& user){
	Transaction t(m_session);
	ptr<User> qry = find_user(user.username);
	if(!qry){
		m_session.add(new User(user));
		t.commit();
	}else{
// 		qry.modify()->password = user.password;
// 		qry.modify()->acl = user.acl;
// 		qry.modify()->teamname = user.teamname;
		throw db_error() << err_info("User already exists!");
	}
}

void SqliteDB::delete_user(const string& username){
	Transaction t(m_session);
	ptr<User> qry = find_user(username);
	if(!qry){
		throw db_error() << err_info("User not found");
	}
	qry.remove();
	t.commit();
}

void SqliteDB::update_user(User& user){
	Transaction t(m_session);
	ptr<User> qry = find_user(user.username);
	if(!qry){
		throw db_error() << err_info("User not found");
	}
	qry.modify()->acl = user.acl;
	qry.modify()->password = user.password;
	qry.modify()->teamname = user.teamname;
	t.commit();
}

Wt::Dbo::ptr<User> SqliteDB::find_user(const string& user){
// 	Transaction t(m_session);
	return m_session.find<User>().where("username = ?").bind(user);
}

void SqliteDB::get_users(vector<User>& users){
	users.clear();
	Transaction t(m_session);
	typedef collection<ptr<User> > usr_col;
	usr_col uc = m_session.find<User>();
	for(usr_col::const_iterator i = uc.begin(); i != uc.end(); ++i){
		users.push_back(User(**i));
	}
}

void SqliteDB::add_language(Language& lang){
	Transaction t(m_session);
	ptr<Language> qry = m_session.find<Language>().where("name = ?").bind(lang.name);
	if(!qry){
		// language not found, we add a new one
		m_session.add(new Language(lang));
	}else{
		qry.modify()->compile_cmd = lang.compile_cmd;
		qry.modify()->link_cmd = lang.link_cmd;
		qry.modify()->run_cmd = lang.run_cmd;
	}
	t.commit();
}

void SqliteDB::delete_language(const string& lname){
	Transaction t(m_session);
	ptr<Language> qry = m_session.find<Language>().where("name = ?").bind(lname);
	if(!qry){
		throw db_error() << err_info("Languge "+lname+" not found");
	}
	qry.remove();
	t.commit();
}

void SqliteDB::get_languages(vector<Language>& lv){
	lv.clear();
	Transaction t(m_session);
	typedef collection<ptr<Language> > lang_col;
	lang_col lc = m_session.find<Language>();
	for(lang_col::const_iterator i = lc.begin(); i != lc.end(); ++i){
		lv.push_back(Language(**i));
	}
}
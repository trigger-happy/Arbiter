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
	ptr<User> qry = m_session.find<User>().where("username = ?").bind(username.c_str());
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
	}
}

void SqliteDB::add_user(User& user){
	Transaction t(m_session);
	ptr<User> qry = m_session.find<User>().where("username = ?").bind(user.username);
	if(!qry){
		m_session.add(new User(user));
	}else{
		qry.modify()->password = user.password;
		qry.modify()->acl = user.acl;
		qry.modify()->teamname = user.teamname;
	}
	t.commit();
}


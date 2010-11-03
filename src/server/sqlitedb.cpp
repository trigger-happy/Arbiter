#include <exception>
#include "sqlitedb.h"
#include "../common/datatypes_orm.h"

using namespace Wt::Dbo;
using namespace std;

SqliteDB::SqliteDB(){
	m_connected = false;
}

string SqliteDB::get_acl(const string& username){
	Transaction t(m_session);
	ptr<orm::User> qry = m_session.find<orm::User>().where("username = ?").bind(username);
	if(!qry){
		return "";
	}
	return qry->acl;
}

bool SqliteDB::authenticate(const string& username,
							const string& password){
	Transaction t(m_session);
	ptr<orm::User> qry = find_user(username);
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
		m_session.mapClass<orm::User>("users");
		m_session.mapClass<orm::Language>("languages");
		m_session.mapClass<orm::Clarification>("clars");
		m_session.mapClass<orm::Run>("runs");
		m_session.mapClass<orm::Problem>("problems");
		m_session.mapClass<orm::File>("files");

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

void SqliteDB::add_user(plain::User& user){
	Transaction t(m_session);
	ptr<orm::User> qry = find_user(user.username);
	if(!qry){
		orm::User* u = new orm::User();
		u->acl = user.acl;
		u->password = user.password;
		u->username = user.username;
		u->teamname = user.teamname;
		m_session.add(u);
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
	ptr<orm::User> qry = find_user(username);
	if(!qry){
		throw db_error() << err_info("User not found");
	}
	qry.remove();
	t.commit();
}

void SqliteDB::update_user(plain::User& user){
	Transaction t(m_session);
	ptr<orm::User> qry = find_user(user.username);
	if(!qry){
		throw db_error() << err_info("User not found");
	}
	qry.modify()->acl = user.acl;
	qry.modify()->password = user.password;
	qry.modify()->teamname = user.teamname;
	t.commit();
}

Wt::Dbo::ptr<orm::User> SqliteDB::find_user(const string& user){
// 	Transaction t(m_session);
	return m_session.find<orm::User>().where("username = ?").bind(user);
}

void SqliteDB::get_users(vector<plain::User>& users){
	users.clear();
	Transaction t(m_session);
	typedef collection<ptr<orm::User> > usr_col;
	usr_col uc = m_session.find<orm::User>();
	for(usr_col::const_iterator i = uc.begin(); i != uc.end(); ++i){
		plain::User temp;
		temp.acl = (*i)->acl;
		temp.password = (*i)->password;
		temp.teamname = (*i)->teamname;
		temp.username = (*i)->username;
		users.push_back(temp);
	}
}

void SqliteDB::add_language(plain::Language& lang){
	Transaction t(m_session);
	ptr<orm::Language> qry = m_session.find<orm::Language>().where("name = ?").bind(lang.name);
	if(!qry){
		// language not found, we add a new one
		orm::Language* l = new orm::Language();
		l->compile_cmd = lang.compile_cmd;
		l->link_cmd = lang.link_cmd;
		l->name = lang.name;
		l->run_cmd = lang.run_cmd;
		m_session.add(l);
	}else{
		qry.modify()->compile_cmd = lang.compile_cmd;
		qry.modify()->link_cmd = lang.link_cmd;
		qry.modify()->run_cmd = lang.run_cmd;
	}
	t.commit();
}

void SqliteDB::delete_language(const string& lname){
	Transaction t(m_session);
	ptr<orm::Language> qry = m_session.find<orm::Language>().where("name = ?").bind(lname);
	if(!qry){
		throw db_error() << err_info("Languge "+lname+" not found");
	}
	qry.remove();
	t.commit();
}

void SqliteDB::get_languages(vector<plain::Language>& lv){
	lv.clear();
	Transaction t(m_session);
	typedef collection<ptr<orm::Language> > lang_col;
	lang_col lc = m_session.find<orm::Language>();
	for(lang_col::const_iterator i = lc.begin(); i != lc.end(); ++i){
		plain::Language temp;
		temp.compile_cmd = (*i)->compile_cmd;
		temp.link_cmd = (*i)->link_cmd;
		temp.name = (*i)->name;
		temp.run_cmd = (*i)->run_cmd;
		lv.push_back(temp);
	}
}

void SqliteDB::add_clarification(const plain::Clarification& clar){
	Transaction t(m_session);
	ptr<orm::User> usr = find_user(clar.asker);
	ptr<orm::User> ans = find_user(clar.answerer);
	if(usr){
		orm::Clarification* c = new orm::Clarification();
		c->asker = usr;
		c->answerer = ans;
		c->answer = clar.answer;
		c->ask_time = clar.ask_time;
		c->category = clar.category;
		c->question = clar.question;
		m_session.add(c);
	}else{
		throw db_error() << err_info("Asker: "+clar.asker+" not found");
	}
	t.commit();
}

void SqliteDB::get_clarifications(vector<plain::Clarification>& clars){
	clars.clear();
	Transaction t(m_session);
	typedef collection<ptr<orm::Clarification> > clar_col;
	clar_col cc = m_session.find<orm::Clarification>();
	for(clar_col::const_iterator i = cc.begin(); i != cc.end(); ++i){
		plain::Clarification temp;
		temp.answer = (*i)->answer;
		if((*i)->answerer){
			temp.answerer = (*i)->answerer->username;
		}
		if((*i)->asker){
			temp.asker = (*i)->asker->username;
		}
		temp.ask_time = (*i)->ask_time;
		temp.category = (*i)->category;
		temp.question = (*i)->question;
		clars.push_back(temp);
	}
}

void SqliteDB::add_run(const plain::Run& run){
	Transaction t(m_session);
	ptr<orm::User> usr = m_session.find<orm::User>().where("username = ?").bind(run.contestant);
	if(!usr){
		throw db_error() << err_info("Contestant: "+run.contestant+" not found");
	}else{
		orm::Run* r = new orm::Run();
		r->contestant = usr;
		r->lang = m_session.find<orm::Language>().where("name = ?").bind(run.lang);
		r->problem = m_session.find<orm::Problem>().where("title = ?").bind(run.problem);
		r->submit_time = run.submit_time;
		m_session.add(r);
	}
	t.commit();
}

void SqliteDB::get_runs(vector<plain::Run>& rv){
	rv.clear();
	Transaction t(m_session);
	typedef collection<ptr<orm::Run> > run_col;
	run_col rc = m_session.find<orm::Run>();
	for(run_col::const_iterator i = rc.begin(); i != rc.end(); ++i){
		plain::Run temp;
		if((*i)->contestant){
			temp.contestant = (*i)->contestant->username;
		}
		if((*i)->judge){
			temp.judge = (*i)->judge->username;
		}
		if((*i)->lang){
			temp.lang = (*i)->lang->name;
		}
		if((*i)->problem){
			temp.problem = (*i)->problem->title;
		}
		temp.submit_time = (*i)->submit_time;
		rv.push_back(temp);
	}
}

#ifndef RUNNER_EXECUTOR_H__
#define RUNNER_EXECUTOR_H__

#include <apr.h>
#include <apr_thread_proc.h>
#include <boost/asio.hpp>

class Problem;

class Executor {
public:
	enum Status { NOT_STARTED, COMPILING, RUNNING, COMPLETED };
private:
	Status status;
	apr_proc_t proc;
	boost::asio::deadline_timer timer;
	void execute();
public:
	Executor(const std::string &source, const Problem &ps);
	void start();
};

#endif

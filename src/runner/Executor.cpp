#include "Executor.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include "../common/Language.h"

using namespace boost::asio;

Executor::Executor(const std::string &problem_path, const std::string &language_xml, const std::string &sourcefile, io_service &io) :
		status(NOT_STARTED), timer(io) {
	Language lp("");
}

void Executor::execute() {
	apr_procattr_t *procAttr;
	rlimit limit;
	limit.rlim_cur = 100;
	apr_procattr_create(&procAttr, 0);
	apr_procattr_limit_set(procAttr, APR_LIMIT_MEM, &limit);
}

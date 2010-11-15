#include "Executor.h"
#include <boost/date_time/posix_time/posix_time.hpp>

void Executor::execute() {
	apr_procattr_t *procAttr;
	rlimit limit;
	limit.rlim_cur = 100;
	apr_procattr_create(&procAttr, 0);
	apr_procattr_limit_set(procAttr, APR_LIMIT_MEM, &limit);
}

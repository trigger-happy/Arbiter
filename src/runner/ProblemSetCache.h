#ifndef RUNNER_PROBLEMSETCACHE_H__
#define RUNNER_PROBLEMSETCACHE_H__

#include <polarssl/sha2.h>
#include <string>

class ProblemSetCache {
public:
	bool hasProblemSet(const std::string &hash);
};

#endif

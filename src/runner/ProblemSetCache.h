#ifndef RUNNER_PROBLEMSETCACHE_H__
#define RUNNER_PROBLEMSETCACHE_H__

#include <polarssl/sha2.h>
#include <string>
#include <vector>
#include <tr1/unordered_map>
#include <tr1/memory>


/**
  * Handles storing and unpacking of problem sets in preparation for running.
  *
  *
  */
class ProblemSetCache {
private:
	const std::string working_directory;
public:
	ProblemSetCache(const std::string &working_directory);

	/**
	  * Queries the directory if the problem ID with the hash exists and verifies it.
	  */
	bool hasVerifiedProblem(const std::string &problemId, const std::vector<uint8_t> &hash);

	/**
	  * Unpacks the problem and returns the path to the directory it is packed.
	  * Note that it does not try to verify the integrity of the problem and that it will always unpack
	  * the problem set in the same directory.
	  */
	std::string unpackProblemSet(const std::string &problemId);

	void deleteProblemSet(const std::string &problemId);

	/**
	  * Returns the path of where the problem set archived should be located. Used for determining where
	  * to save the recieved problem set.
	  */
	std::string getProblemSetPath(const std::string &problemId);
};

#endif

#include "ProblemSetCache.h"
#include <polarssl/sha2.h>
#include <fstream>
#include <minizip/unzip.h>
#include <apr_file_io.h>
#include <apr_file_info.h>
#include <apr_pools.h>

using namespace std;

bool ProblemSetCache::hasVerifiedProblem(const string &problemId, const vector<uint8_t> &hash) {
	//SHA-256 hashes are ALWAYS 32 bytes.
	if ( hash.size() != 32 ) return false;

	const string &path = getProblemSetPath(problemId);
	{
		ifstream file(path.c_str());
		if ( !file ) return false;
	}
	uint8_t output[32];
	sha2_file(path.c_str(), output, false);

	for ( size_t i = 0; i < 32; ++i )
		if ( output[i] != hash[i] ) return false;
	return true;
}

string ProblemSetCache::unpackProblemSet(const string &problemId) {
	const string &path = getProblemSetPath(problemId);

	return path;
}


namespace {
	void rm_rf(const char *path, apr_pool_t *pool) {
		apr_dir_t *dir;
		apr_dir_open(&dir, path, 0);
		apr_finfo_t fileInfo;

		while (apr_dir_read(&fileInfo, APR_FINFO_TYPE | APR_FINFO_NAME | APR_FINFO_DIRENT, dir) == APR_SUCCESS) {
			if ( strcmp(fileInfo.name, ".") == 0 || strcmp(fileInfo.name, "..") == 0 ) continue;

			char *newPath;
			apr_filepath_merge( &newPath, path, fileInfo.name, APR_FILEPATH_NATIVE, pool);

			if ( fileInfo.filetype != APR_DIR ) {
				//if it's not a directory, just unlink it
				apr_file_remove(newPath, pool);
			} else {
				//recurse the deletion
				rm_rf(newPath, pool);
			}
		}
		apr_dir_close(dir);
		apr_dir_remove(path, pool);
	}

struct AprPoolGuard {
	apr_pool_t *pool;
	AprPoolGuard(apr_pool_t *pool) : pool(pool) {}
	AprPoolGuard() {
		apr_pool_create(&pool, 0);
	}

	~AprPoolGuard() {
		apr_pool_destroy(pool);
	}

};
}

void ProblemSetCache::deleteProblemSet(const string &problemId) {
	AprPoolGuard pool;

	rm_rf(getProblemSetPath(problemId).c_str(), pool.pool);
}

string ProblemSetCache::getProblemSetPath(const string &problemId) {
	AprPoolGuard pool;

	char *path;
	apr_filepath_merge(&path, working_directory.c_str(), problemId.c_str(), APR_FILEPATH_NATIVE, pool.pool);
	//temporarily assign to make sure that we use the allocated string before it gets released
	string ret = path;
	return ret;
}

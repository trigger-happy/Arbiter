#ifndef RUNNER_LANGUAGECACHE_H__
#define RUNNER_LANGUAGECACHE_H__

#include <string>
#include <map>
#include <vector>

class LanguageCache {
	std::map<uint32_t, std::string> cache;
public:
	LanguageCache() {}
	void saveLanguage(uint32_t language_id, const std::string &data);
	bool hasLanguage(uint32_t language_id, const std::vector<uint8_t> &hash) const;
	std::string getLanguage(uint32_t language_id) const;
};

#endif

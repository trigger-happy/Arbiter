#include "LanguageCache.h"
#include <polarssl/sha2.h>

using namespace std;

void LanguageCache::saveLanguage(uint32_t language_id, const std::string &data) {
	cache[language_id] = data;
}

bool LanguageCache::hasLanguage(uint32_t language_id, const vector<uint8_t> &hash) const {
	if ( hash.size() != 32) return false;
	auto iter = cache.find(language_id);
	if ( iter == cache.end() ) return false;
	const std::string &str = iter->second;
	if ( str.empty() ) return false;
	uint8_t output[32];
	sha2((uint8_t*)str.data(), str.length(), output, false);
	for ( int i = 0; i < 32; ++i )
		if ( output[i] != hash[i] )
			return false;
	return true;
}

std::string LanguageCache::getLanguage(uint32_t language_id) const {
	auto iter = cache.find(language_id);
	if ( iter == cache.end() )
		return "";
	return iter->second;
}

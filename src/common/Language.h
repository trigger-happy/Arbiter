#ifndef COMMON_LANGUAGE_H__
#define COMMON_LANGUAGE_H__

#include <string>
#include <sstream>
#include <unordered_map>
#include <stdexcept>
#include <cstring>
#include <boost/property_tree/detail/rapidxml.hpp>

/**
  * A class that parses xml language definition files.
  *
  * In the directive, the syntax for replacement is @{var(:filter)*}
  *
  * Accepted environment variables, assuming that the full path is C:\arbiter\extract\prob1.cpp:
  * - filename -> prob1.cpp
  * - fullpath -> C:\arbiter\extract\prob1.cpp
  * - filedir -> C:\arbiter\extract
  * - / -> \
  * - lang -> C++
  *
  * Accepted filters are (assuming the input is C:\arbiter\extract\prob1.cpp):
  * - base -> C:\arbiter\extract\prob1 (extracts the string before the last ".")
  * - parent -> C:\arbiter\extract (extracts the parent directory of the current path)
  * - ext -> cpp (extracts the string after the last ".")
  *
  * Short xml example:
  * @code
  * <language name="C++">
  * <make>g++ @{filename} -o @{filename:base}</make>
  * <run>./@{filename:base}</run>
  * <platform list="win32-msvc win64-msvc">
  *   <make>cl @{filename}</make>
  *   <run>@{filename:base}.exe</run>
  * </platform>
  * </language>
  * @endcode
  */
class Language {
public:
	class SyntaxErrorException : public std::exception {
		size_t pos;
		char ch;
		std::string message;
	public:
		SyntaxErrorException(size_t pos, char ch, const std::string &info = "") : pos(pos), ch(ch) {
			std::ostringstream msg;
			msg << "Error at " << pos <<  '(' << ch << ')';
			if ( !info.empty() )
				msg << ": " << info;
			message = msg.str();
		}
		const char *what() const throw() {
			return message.c_str();
		}

		~SyntaxErrorException() throw() {}
	};

	class TokenException : public std::exception {
		size_t pos;
		std::string message;
	public:
		const std::string token;
		TokenException(size_t pos, const std::string &token, const std::string &info = "") :
				pos(pos), token(token) {
			std::ostringstream msg;
			msg << "Unexpected token at " << pos <<  '(' << token << ')';
			if ( !info.empty() )
				msg << ": " << info;
			message = msg.str();
		}
		const char *what() const throw() {
			return message.c_str();
		}

		~TokenException() throw() {}
	};

	/**
	  * Stores environment variables that is used by the directive translator to replace directives.
	  */
	struct Environment {
		std::unordered_map<std::string, std::string> dictionary;

		static const std::string TOK_FULLPATH, TOK_FILENAME, TOK_FILEDIR, TOK_SEPARATOR, TOK_LANG;
		std::string getValue(const std::string &token, bool *bFound = 0) const {
			auto iter = dictionary.find(token);
			if ( bFound )
				*bFound = (iter != dictionary.end());

			if ( iter == dictionary.end() ) {
				return "";
			} else {
				return iter->second;
			}
		}
	};

	struct Directive {
		std::string make;
		std::string run;

		Directive() {}
		Directive(Directive &&rhs) :
				make(std::move(rhs.make)), run(std::move(rhs.run)) {}

		static Directive parse(rapidxml::xml_node<> *node) {
			Directive ret;
			if ( !node ) return ret;
			do {
				if ( strcmp(node->name(), "make") == 0 ) {
					ret.make = node->value();
				} else if ( strcmp(node->name(), "run") == 0 ) {
					ret.run = node->value();
				}
			} while (node = node->next_sibling());
			return ret;
		}

		std::string evaluateMake(const Environment &e) const {
			return evaluate(e, make);
		}

		std::string evaluateRun(const Environment &e) const {
			return evaluate(e, run);
		}

	private:
		typedef std::string (*Filter)(const std::string &);
		static std::unordered_map<std::string, Filter> filters;

		static std::string base(const std::string &s) {
			return s.substr(0, s.find_last_of('.'));
		}

		static std::string ext(const std::string &s) {
			auto i = s.find_last_of('.');
			if ( i == std::string::npos )
				return "";
			return s.substr(i + 1);
		}

		static std::string parent(const std::string &s) {
			if ( s.empty() ) return "";
			auto i = s.find_last_of("/\\:");
			if ( i == s.length() - 1 )
				i = s.find_last_of("/\\:", s.length() - 2);
			if ( i == std::string::npos )
				return s;
			return s.substr(0, i - 1);
		}

		static std::string applyFilter(const std::string &target, const std::string &filter, bool *bApplied = 0) {
			bool init = false;
			if ( !init ) {
				filters["base"] = base;
				filters["ext"] = ext;
				filters["parent"] = parent;
				init = true;
			}
			auto iter = filters.find(filter);
			if ( bApplied )
				*bApplied = (iter != filters.end());
			if ( iter == filters.end() )
				return target;
			else
				return iter->second(target);
		}

		enum EvalState {Normal, PreEvaluation, Evaluation, Extension};
		std::string evaluate(const Environment &e, const std::string &str, bool strict = false) const {
			std::ostringstream ret;

			//parse with a state machine!
			const size_t sz = str.size();
			size_t lastPosNormal = 0, lastPosToken = 0;
			EvalState state = Normal;
			std::string token;

			for ( size_t i = 0; i < sz; ++i ) {
				const char ch = str[i];
				switch(state) {
				case Normal:
					switch (ch) {
					case '@':
						ret << str.substr(lastPosNormal, i - lastPosNormal);
						lastPosNormal = i;
						state = PreEvaluation;
						break;
					default:
						break;
					}
					break;
				case PreEvaluation:
					switch (ch) {
					case '@':
						//escape the @ symbol
						lastPosNormal = i;
						state = Normal;
						break;
					case '{':
						lastPosToken = i + 1;
						state = Evaluation;
						break;
					default:
						if ( strict ) {
							throw SyntaxErrorException(i, ch, "expected { or @.");
						} else {
							//treat what has been parsed as "normal"
							state = Normal;
						}
						break;
					}
					break;
				case Evaluation:
					switch (ch) {
					case ':':
						token.assign(str, lastPosToken, i - lastPosToken);
						lastPosToken = i + 1;
						state = Extension;
						break;
					case '}':
						ret << e.getValue(str.substr(lastPosToken, i - lastPosToken));
						lastPosNormal = i + 1;
						state = Normal;
						break;
					}
					break;
				case Extension:
					switch (ch) {
					case '}': {
							bool bFound;
							const std::string &val = e.getValue(token, &bFound);
							if ( !bFound && strict ) {
								throw TokenException(lastPosNormal + 2, token);
							}

							const std::string &filterName = str.substr(lastPosToken, i - lastPosToken);
							ret << applyFilter(val, filterName, &bFound);
							if ( !bFound && strict )
								throw TokenException(lastPosToken, filterName, "Filter not found.");
							}

								lastPosNormal = i + 1;
								state = Normal;
								break;
							}
				}
				break;
			}
			if ( state != Normal && strict ) {
				throw SyntaxErrorException(lastPosNormal, '@', "No matching } brace.");
			}
			ret << str.substr(lastPosNormal);
			return ret.str();
		}
	};

private:
	std::string name;
	std::unordered_map<std::string,Directive> directives;
public:
	Directive getDirective(const std::string &platform) {
		auto iter = directives.find(platform);
		if ( iter == directives.end() )
			return iter->second;
		return directives[""];
	}

	Language(const std::string &xml);

	~Language() {

	}
};

#endif

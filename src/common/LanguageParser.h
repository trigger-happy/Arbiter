#ifndef COMMON_LANGUAGEPARSER_H__
#define COMMON_LANGUAGEPARSER_H__

#include <string>
#include <sstream>
#include <unordered_map>
#include <stdexcept>

class LanguageParser {
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

	struct Environment {
		std::unordered_map<std::string, std::string> dictionary;
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

		Directive(Directive &&rhs) :
				make(std::move(rhs.make)), run(std::move(rhs.run)) {}

		std::string evaluateMake(const Environment &e) const {
			return evaluate(e, make);
		}

		std::string evaluateRun(const Environment &e) const {
			return evaluate(e, run);
		}

	private:

		static std::string applyFilter(const std::string &target, const std::string &filter) {
			return target;
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
							throw TokenException(lastPosToken, token);
						}
						try {
							ret << applyFilter(val, str.substr(lastPosToken, i - lastPosToken));
						} catch ( ... ) {
							if ( strict ) throw;
							ret << val;
						}

						lastPosNormal = i + 1;
						state = Normal;
						break;
					}
					}
					break;
				}
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
	const Directive& getDirective(const std::string &platform);

	~LanguageParser() {

	}
};

#endif

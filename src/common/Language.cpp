#include "Language.h"
#include <boost/tokenizer.hpp>

const std::string
Language::Environment::TOK_FULLPATH = "fullpath",
Language::Environment::TOK_FILENAME = "filename",
Language::Environment::TOK_FILEDIR = "filedir",
Language::Environment::TOK_SEPARATOR = "/",
Language::Environment::TOK_LANG = "lang";

Language::Language(const std::string &xml) {
	using namespace rapidxml;
	xml_document<> doc;
	std::vector<char> buffer(xml.begin(), xml.end());
	buffer.push_back(0);
	doc.parse<0>(&buffer[0]);
	if ( strcmp(doc.first_node()->name(), "language") != 0 ) {
		throw std::runtime_error("Root node is not \"language\"");
	}
	auto lang = doc.first_node()->first_attribute("name");
	if ( !lang )
		throw std::runtime_error("No language name specified in root tag.");
	name = lang->value();

	auto *node = doc.first_node();
	if ( !node )
		throw std::runtime_error("Language definitions cannot have empty directives.");

	do {
		if ( strcmp(node->name(), "platform") == 0 ) {
			auto platforms = node->first_attribute("list")->value();
			const Directive &directive = Directive::parse(node->first_node());

			if ( platforms ) {
				std::string s(platforms);
				boost::tokenizer<> tok(s);
				for (auto iter = tok.begin(); iter != tok.end(); ++iter) {
					directives[*iter] = directive;
				}
			} else {
				directives[""] = directive;
			}
		} else if ( strcmp(node->name(), "make") == 0 ) {
			directives[""].make = node->value();
		} else if ( strcmp(node->name(), "run") == 0 ) {
			directives[""].run = node->value();
		}
	} while ( node = node->next_sibling() );
}


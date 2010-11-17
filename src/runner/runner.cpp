#include <iostream>
#include <apr.h>
#include <apr_general.h>
#include "OptionsParser.h"
#include "ProblemSetCache.h"
#include "../src/network/RunnerNetwork.h"
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include "Executor.h"
#include "LanguageCache.h"

#define VERSION_STRING "0.0.1"


using namespace std;
using namespace boost::asio;

OptionsParser g_options;

class NetworkResponder : public RunnerNetworkListener
{
public:
	RunnerNetwork &rn;
	ProblemSetCache &cache;
	RunOrder *currentRun;
	Executor *currentExecution;
	LanguageCache languageCache;

	NetworkResponder(RunnerNetwork& rn, ProblemSetCache &cache) :
			rn(rn), cache(cache), currentRun(0), currentExecution(0) {
		rn.addListener(*this);
	}

	bool isExecuting() { return currentExecution != 0; }
	bool isPending() { return currentRun != 0 && currentExecution == 0; }
	bool isFree() { return currentRun == 0; }


	static std::vector<uint8_t> convertToVector(const std::string &hash) {
		return std::vector<uint8_t>(hash.begin(), hash.end());
	}

	void launchRunOrder() {
		//Check ALL requirements to make extra sure we have what we need.

		if ( !languageCache.hasLanguage(currentRun->language_id(), convertToVector(currentRun->language_hash())) ) {
			//TODO: Wail and complain
			//for the moment, just re-request the language
			rn.requestLanguage(currentRun->language_id());
			return;
		}

		if ( !cache.hasVerifiedProblem(currentRun->problem_id(), convertToVector(currentRun->problem_hash())) ) {
			//TODO: ditto
			rn.requestProblemSet(currentRun->problem_id());
			return;
		}

		try {
			const string &path = cache.unpackProblemSet(currentRun->problem_id());

		} catch (std::exception &e) {
			cout << e.what() << endl;
			//TODO: retry x times then flag the server that this is dysfunctional after.
			//uh....... do
		}
	}

	~NetworkResponder() {
		rn.removeListener(*this);
	}

	virtual void connected() {
		std::cout << "Connected" << std::endl;
	}
	virtual void authenticated() {
		std::cout << "Authenticated" << std::endl;
	}
	virtual void disconnected() {
		std::cout << "Disconnected" << std::endl;
	}
	virtual void connectionFailed() {
		std::cout << "Connection failed" << std::endl;
	}

	virtual void receiveRunOrder(RunOrder ro) {
		std::cout << "Received run order" << std::endl;
		std::cout << ro.run_id() << std::endl;
		std::cout << ro.problem_id() << std::endl;
		std::cout << ro.problem_hash() << std::endl;
		std::cout << ro.language_id() << std::endl;
		std::cout << ro.language_hash() << std::endl;
		std::cout << ro.attachment() << std::endl;


		if ( !isFree() ) {
			//TODO: there's a current run, complain
		}
		currentRun = new RunOrder(ro);
		if ( !cache.hasVerifiedProblem(ro.problem_id(), convertToVector(ro.problem_hash())) )
			rn.requestProblemSet(ro.problem_id());
	}

	virtual void receiveProblemSet(ProblemSet ps) {
		//Log
		std::cout << "Received problem set" << std::endl;
		std::cout << ps.problem_id() << std::endl;
		std::cout << ps.problem_hash() << std::endl;
		//Write
		if ( !cache.saveProblemSet(ps.attachment(), ps.problem_id()) ) {
			//TODO: Handle error.
			//give up and cry?
		}

		//Verify contents
		std::vector<uint8_t> hash(ps.problem_hash().begin(), ps.problem_hash().end());
		assert(hash.size() == 32);

		if ( !cache.hasVerifiedProblem(ps.problem_id(), hash) ) {
			//TODO: Handle error.
			//log then complain?
		}

		//if we have a run pending, check if we have the proper language then execute the run
		//else, request the language from the server.
		if ( isPending() ) {
			if ( !languageCache.hasLanguage(currentRun->language_id(), convertToVector(currentRun->language_hash())) ) {
				rn.requestLanguage(currentRun->language_id());
			} else {
				launchRunOrder();
			}
		}
	}


	virtual void receiveLanguage(Language l) {
		std::cout << "Received language" << std::endl;
		std::cout << l.language_id() << std::endl;
		std::cout << l.language_hash() << std::endl;
		std::cout << l.attachment() << std::endl;

		languageCache.saveLanguage(l.language_id(), l.attachment());
		//verify if the hash is correct
		if ( !languageCache.hasLanguage(l.language_id(), convertToVector(l.language_hash())) ) {
			//TODO: Wail and cry
		}

		if ( isPending() ) {
			launchRunOrder();
		}
	}
};

int main(int argc, char **argv) {
	apr_app_initialize(&argc, (const char * const **)&argv, 0);
	atexit(apr_terminate);

	try {
		g_options.parseArgs(argc,argv);
		if ( g_options.isHelpToggled() ) {
			cout << "Usage: " << argv[0] << " [options]\n";
			g_options.listOptions();
			return 0;
		} else if ( g_options.isVersionToggled() ) {
			cout << VERSION_STRING << endl;
			return 0;
		}
	} catch(std::exception& e) {
		cout << e.what() << endl;
		return 1;
	}

	io_service io;
	io_service::work work(io);
	ProblemSetCache cache(g_options.getDirectory());

	//loop until the hostname is resolved
	bool resolved = false;
	while ( !resolved ) {
		try {
			RunnerNetwork connection(io, g_options.getServer(), g_options.getPort(), g_options.getPingInterval());
			resolved = true;
			NetworkResponder responder(connection, cache);
			io.run();
		} catch ( std::exception &e ) {
			cout << e.what() << endl;
		}
	}
	return 0;
}

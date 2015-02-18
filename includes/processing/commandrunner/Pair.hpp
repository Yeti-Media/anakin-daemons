// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*-
/*
 * Pair.hpp
 *
 *  Created on: 13/02/2015
 *      Author: Paul Fitzpatrick
 */

#ifndef PAIR_HPP_
#define PAIR_HPP_

#include <processing/commandrunner/CommandRunner.hpp>

namespace Anakin {

/**
 *
 * Combine two services.
 *
 */
template <class R1, class R2>
class Pair : public CommandRunner {
public:
	Pair(const string & threadName) : r1(threadName), r2(threadName),
									  CommandRunner("common", threadName) {
		active = 0;
	}

	virtual ~Pair() {
	}

	void validateRequest(const Ptr<vector<string>>& input) {
		inputError = false;
		active = 0;
		if (checkHint(r1.getFlagHint(),input)) {
			r1.validateRequest(input);
			if (!r1.isError()) {
				active = 1;
				return;
			}
		}
		if (checkHint(r1.getFlagHint(),input)||active==0) {
			r2.validateRequest(input);
			if (!r2.isError()) {
				active = 2;
				return;
			}
		}
		inputError = true;
		active = 2;
	}

	void extendServerCommandsWith(const Ptr<Flags>& serverFlags) {
		r1.extendServerCommandsWith(serverFlags);
		r2.extendServerCommandsWith(serverFlags);
	}

	void parseServerFlags(const Ptr<Flags>& serverFlags) {
		r1.parseServerFlags(serverFlags);
		r2.parseServerFlags(serverFlags);
	}

	void initializeCommandRunner(const Ptr<DataOutput> & out,
								 const Ptr<SFBMCache> & cache) {
		r1.initializeCommandRunner(out,cache);
		r2.initializeCommandRunner(out,cache);
	}

	void run() {
		if (active==1) r1.run();
		if (active==2) r2.run();
	}

	Ptr<Help> getHelp() {
		return r1.getHelp();
	}

private:
	R1 r1;       ///< first service
	R2 r2;       ///< second service
	int active;  ///< index of service selected by latest request

	/**
	 *
	 * Scan arguments to see which if a particular service is required.
	 *
	 * @param hint an argument name that identifies the service
	 * @param input the arguments supplied
	 *
	 * @return true if the given service is required.
	 *
	 */
	bool checkHint(const string& hint, const Ptr<vector<string>>& input) {
		if (hint=="") return true;
		string minus_hint = string("-") + hint;
		for (auto in: *input) {
			if (in == minus_hint) return true;
		}
		return false;
	}
};

} /* namespace Anakin */

#endif /* PAIR_HPP_ */

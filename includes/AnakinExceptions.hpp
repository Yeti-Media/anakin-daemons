/*
 * ANAKINEXCEPTIONS.hpp
 *
 *  Created on: 19/05/2014
 *      Author: franco
 */

#ifndef ANAKINEXCEPTIONS_HPP_
#define ANAKINEXCEPTIONS_HPP_

#include <exception>

using namespace std;

namespace Anakin {

/**
 * Used when flags can't be parsed
 */
class EInvalidFlags: public exception {
public:
	/** Constructor (C strings).
	 *  @param message C-style string error message.
	 *                 The string contents are copied upon construction.
	 *                 Hence, responsibility for deleting the \c char* lies
	 *                 with the caller.
	 */
	explicit EInvalidFlags(const char* message) :
			msg_(message) {
	}
	;

	/**
	 *  @param message The error message.
	 */
	explicit EInvalidFlags(const std::string& message) :
			msg_(message) {
	}
	;

	/** Destructor.
	 * Virtual to allow for subclassing.
	 */
	virtual ~EInvalidFlags() throw () {
	}

	/** Returns a pointer to the (constant) error description.
	 *  @return A pointer to a \c const \c char*. The underlying memory
	 *          is in posession of the \c Exception object. Callers \a must
	 *          not attempt to free the memory.
	 */
	virtual const char* what() const throw () {
		return msg_.c_str();
	}

protected:
	/** Error message.
	 */
	std::string msg_;
};

}
;

#endif /* ANAKINEXCEPTIONS_HPP_ */

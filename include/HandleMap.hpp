#pragma once

#include <Handled.hpp>

#include <map>
#include <boost/integer.hpp>
#include <boost/cast.hpp>

using boost::numeric_cast;
using boost::numeric::bad_numeric_cast;

/**
 * Provides unique handles to objects and allows access by that handle.
 * The objects must extend the "Handled" class. Objects are handled and
 * stored as shared_ptr references to allow flexible lifecycle management.
 *
 * The handle 0 is never used, so that valid handles always evaluate to
 * true in Game Maker.
 */
class HandleMap {
	typedef std::shared_ptr<Handled> HandledPtr;
public:
	HandleMap() : nextHandle_(1), content_() {}

	/**
	 * Associate the element with a unique handle, which is returned.
	 */
	uint32_t allocate(HandledPtr element) {
		while(content_.count(nextHandle_) > 0 || nextHandle_ == 0) {
			nextHandle_++;
		}
		content_[nextHandle_] = element;
		return nextHandle_++;
	}

	/**
	 * Return the element associated with the given handle, or a NULL pointer
	 * if this Manager does not hold an object with the given handle and of
	 * the requested type.
	 *
	 * Note that you will also get NULL as result if the given handle has
	 * previously been associated with NULL.
	 */
	template<typename RequestedType>
	std::shared_ptr<RequestedType> find(uint32_t handle) {
		auto iter = content_.find(handle);
		if(iter == content_.end()) {
			return nullptr;
		} else {
			return std::dynamic_pointer_cast<RequestedType>(iter->second);
		}
	}

	/**
	 * Convenience method for use from the Game Maker API which uses double for all numbers.
	 * Returns a NULL pointer if the double value cannot be represented as uint32_t.
	 */
	template<typename RequestedType>
	std::shared_ptr<RequestedType> find(double handle) {
		if(trunc(handle) != handle) {
			return std::shared_ptr<RequestedType>();
		}
		try {
			return find<RequestedType>(numeric_cast<uint32_t>(handle));
		} catch(bad_numeric_cast &e) {
			return nullptr;
		}
	}

	/**
	 * Release the handle-element association.
	 */
	void release(uint32_t handle) {
		content_.erase(handle);
	}

	/**
	 * Convenience method for use from the Game Maker API which uses double for all numbers.
	 * Does nothing if the double value cannot be represented as uint32_t
	 */
	void release(double handle) {
		if(trunc(handle) != handle) {
			return;
		}
		try {
			release(numeric_cast<uint32_t>(handle));
			return;
		} catch(bad_numeric_cast &e) {
			return;
		}
	}

	void releaseAll() {
		content_.clear();
	}

	uint32_t size() {
		return content_.size();
	}
private:
	uint32_t nextHandle_;
	std::map<uint32_t, HandledPtr> content_;
};

//*****************************************************************
// PipeLogger.exe - Enhanced Logging Tool for IL-2 1946
// Copyright (C) 2021 SAS~Storebror
//
// This file is part of PipeLogger.exe.
//
// PipeLogger.exe is free software.
// It is distributed under the DWTFYWTWIPL license:
//
// DO WHAT THE FUCK YOU WANT TO WITH IT PUBLIC LICENSE
// Version 1, March 2012
//
// Copyright (C) 2013 SAS~Storebror <mike@sas1946.com>
//
// Everyone is permitted to copy and distribute verbatim or modified
// copies of this license document, and changing it is allowed as long
// as the name is changed.
//
// DO WHAT THE FUCK YOU WANT TO WITH IT PUBLIC LICENSE
// TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
//
// 0. You just DO WHAT THE FUCK YOU WANT TO WITH IT.
//
//*****************************************************************

#pragma once
#include <deque>
#include <mutex>
#include <condition_variable>
#include <optional>

//! \brief A templated *thread-safe* collection based on dequeue
//!
//!        pop_front() waits for the notification of a filling method if the collection is empty.
//!        The various "emplace" operations are factorized by using the generic "addData_protected".
//!        This generic asks for a concrete operation to use, which can be passed as a lambda.
template< typename T >
class TQueueConcurrent {

	using const_iterator = typename std::deque<T>::const_iterator;

public:
	//! \brief Emplaces a new instance of T in front of the deque
	template<typename... Args>
	void emplace_front(Args&&... args)
	{
		addData_protected([&] {
			_collection.emplace_front(std::forward<Args>(args)...);
			});
	}

	//! \brief Emplaces a new instance of T at the back of the deque
	template<typename... Args>
	void emplace_back(Args&&... args)
	{
		addData_protected([&] {
			_collection.emplace_back(std::forward<Args>(args)...);
			});
	}

	//! \brief Returns the front element and removes it from the collection
	//!
	//!        No exception is ever returned as we garanty that the deque is not empty
	//!        before trying to return data.
	std::optional<T> pop_front(void) noexcept
	{
		if (_collection.empty()) {
			return std::nullopt;
		}
		std::unique_lock<std::mutex> lock{ _mutex };
		auto elem = std::move(_collection.front());
		_collection.pop_front();
		return elem;
	}



private:

	//! \brief Protects the deque, calls the provided function and notifies the presence of new data
	//! \param The concrete operation to be used. It MUST be an operation which will add data to the deque,
	//!        as it will notify that new data are available!
	template<class F>
	void addData_protected(F&& fct)
	{
		std::unique_lock<std::mutex> lock{ _mutex };
		fct();
		lock.unlock();
	}

	std::deque<T> _collection;                     ///< Concrete, not thread safe, storage.
	std::mutex   _mutex;                    ///< Mutex protecting the concrete storage
};
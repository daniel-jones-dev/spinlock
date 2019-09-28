// MIT License
//
// Copyright (c) 2019 Daniel Jones
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Created by Daniel Jones on 28.09.19.
//

#ifndef INCLUDE_SPINLOCK_SPINLOCK_H_
#define INCLUDE_SPINLOCK_SPINLOCK_H_

#include <atomic>
#include <thread>

namespace spinlock {

// Different implementations of SpinLock
//! No yield, using default memory order (Sequential consistency)
class SpinLock;
//! Uses yield, using default memory order (Sequential consistency)
class SpinLock_Yield;
//! No yield, using acquire/release memory order
class SpinLock_MemoryOrder;
//! Uses yield, using acquire/release memory order
class SpinLock_MemoryOrder_Yield;




class SpinLock {
 private:
  std::atomic_flag locked{ATOMIC_FLAG_INIT};

 public:
  constexpr SpinLock() noexcept = default;
  ~SpinLock() noexcept = default;

  SpinLock(SpinLock&) = delete;
  SpinLock(SpinLock&&) = delete;
  void operator=(SpinLock&) = delete;
  void operator=(SpinLock&&) = delete;

  void lock() noexcept {
    while (locked.test_and_set()) {
    }
  }

  bool try_lock() noexcept {
    return !locked.test_and_set();
  }

  void unlock() noexcept { locked.clear(); }
};


class SpinLock_Yield {
 private:
  std::atomic_flag locked{ATOMIC_FLAG_INIT};

 public:
  constexpr SpinLock_Yield() noexcept = default;
  ~SpinLock_Yield() noexcept = default;

  SpinLock_Yield(SpinLock_Yield&) = delete;
  SpinLock_Yield(SpinLock_Yield&&) = delete;
  void operator=(SpinLock_Yield&) = delete;
  void operator=(SpinLock_Yield&&) = delete;

  void lock() noexcept {
    while (locked.test_and_set()) {
      std::this_thread::yield();
    }
  }

  bool try_lock() noexcept {
    return !locked.test_and_set();
  }

  void unlock() noexcept { locked.clear(); }
};


class SpinLock_MemoryOrder {
 private:
  std::atomic_flag locked{ATOMIC_FLAG_INIT};

 public:
  constexpr SpinLock_MemoryOrder() noexcept = default;
  ~SpinLock_MemoryOrder() noexcept = default;

  SpinLock_MemoryOrder(SpinLock_MemoryOrder&) = delete;
  SpinLock_MemoryOrder(SpinLock_MemoryOrder&&) = delete;
  void operator=(SpinLock_MemoryOrder&) = delete;
  void operator=(SpinLock_MemoryOrder&&) = delete;

  void lock() noexcept {
    while (locked.test_and_set(std::memory_order_acquire)) {
    }
  }

  bool try_lock() noexcept {
    return !locked.test_and_set(std::memory_order_acquire);
  }

  void unlock() noexcept { locked.clear(std::memory_order_release); }
};


class SpinLock_MemoryOrder_Yield {
 private:
  std::atomic_flag locked{ATOMIC_FLAG_INIT};

 public:
  constexpr SpinLock_MemoryOrder_Yield() noexcept = default;
  ~SpinLock_MemoryOrder_Yield() noexcept = default;

  SpinLock_MemoryOrder_Yield(SpinLock_MemoryOrder_Yield&) = delete;
  SpinLock_MemoryOrder_Yield(SpinLock_MemoryOrder_Yield&&) = delete;
  void operator=(SpinLock_MemoryOrder_Yield&) = delete;
  void operator=(SpinLock_MemoryOrder_Yield&&) = delete;

  void lock() noexcept {
    while (locked.test_and_set(std::memory_order_acquire)) {
      std::this_thread::yield();
    }
  }

  bool try_lock() noexcept {
    return !locked.test_and_set(std::memory_order_acquire);
  }

  void unlock() noexcept { locked.clear(std::memory_order_release); }
};

}  // namespace spinlock

#endif  // INCLUDE_SPINLOCK_SPINLOCK_H_

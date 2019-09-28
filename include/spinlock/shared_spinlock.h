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

#ifndef INCLUDE_SPINLOCK_SHARED_SPINLOCK_H_
#define INCLUDE_SPINLOCK_SHARED_SPINLOCK_H_

#include <atomic>
#include <thread>

namespace spinlock {

// Different implementations of SharedSpinLock
//! No yield, using default memory order (Sequential consistency)
class SharedSpinLock;

// TODO is_lock_free()

class SharedSpinLock {
 private:
  std::atomic<bool> locked{ATOMIC_FLAG_INIT};
  std::atomic<uint16_t> readers{0};

 public:
  constexpr SharedSpinLock() noexcept = default;
  ~SharedSpinLock() noexcept = default;

  SharedSpinLock(SharedSpinLock&) = delete;
  SharedSpinLock(SharedSpinLock&&) = delete;
  void operator=(SharedSpinLock&) = delete;
  void operator=(SharedSpinLock&&) = delete;

  void lock() noexcept {
    bool desired{true};
    while (locked.exchange(desired)) {
      desired = true;
    }
    while (readers.load()) {
    }
  }

  bool try_lock() noexcept {
    bool desired{true};
    if (!locked.exchange(desired)) {
      if (readers.load()) {
        locked.store(false);
        return false;
      }
      return true;
    } else {
      return false;
    }
  }

  void unlock() noexcept { locked.store(false); }

  void lock_shared() noexcept {
    while (true) {
      while (locked.load()) {
      }
      readers.fetch_add(1);
      if (locked.load()) {
        readers.fetch_sub(1);
      } else {
        return;
      }
    }
  }

  bool try_lock_shared() noexcept {
    if (locked.load()) {
      return false;
    }
    readers.fetch_add(1);
    if (locked.load()) {
      readers.fetch_sub(1);
      return false;
    } else {
      return true;
    }
  }

  void unlock_shared() noexcept { readers.fetch_sub(1); }
};

}  // namespace spinlock

#endif  // INCLUDE_SPINLOCK_SHARED_SPINLOCK_H_

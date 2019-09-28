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

#include <spinlock/shared_spinlock.h>
#include <spinlock/spinlock.h>

#include <gtest/gtest.h>

#include <mutex>
#include <shared_mutex>

////////////////////////////////////////////////////////////////////////////////

template <typename _LockType>
class LockableTest : public ::testing::Test {
 protected:
  using Lock = _LockType;
};

template <typename _LockType>
class SharedLockableTest : public LockableTest<_LockType> {};

template <typename _LockType>
class SpinLockTest : public LockableTest<_LockType> {};

template <typename _LockType>
class SharedSpinLockTest : public SharedLockableTest<_LockType> {};

////////////////////////////////////////////////////////////////////////////////

// Define types to test with
using LockableTypes =
    ::testing::Types<spinlock::SpinLock, spinlock::SpinLock_Yield,
                     spinlock::SpinLock_MemoryOrder,
                     spinlock::SpinLock_MemoryOrder_Yield,
                     spinlock::SharedSpinLock, std::mutex, std::timed_mutex,
                     std::shared_timed_mutex>;

using SharedLockableTypes =
    ::testing::Types<spinlock::SharedSpinLock, std::shared_timed_mutex>;

using SpinLockTypes =
    ::testing::Types<spinlock::SpinLock, spinlock::SpinLock_Yield,
                     spinlock::SpinLock_MemoryOrder,
                     spinlock::SpinLock_MemoryOrder_Yield>;

using SharedSpinLockTypes = ::testing::Types<spinlock::SharedSpinLock>;

TYPED_TEST_CASE(LockableTest, LockableTypes);
TYPED_TEST_CASE(SharedLockableTest, SharedLockableTypes);
TYPED_TEST_CASE(SpinLockTest, SpinLockTypes);
TYPED_TEST_CASE(SharedSpinLockTest, SharedSpinLockTypes);

////////////////////////////////////////////////////////////////////////////////

TYPED_TEST(LockableTest, lock) {
  using LockType = typename TestFixture::Lock;
  LockType lock;
  lock.lock();
  lock.unlock();
}

TYPED_TEST(LockableTest, try_lock) {
  using LockType = typename TestFixture::Lock;
  LockType lock;
  ASSERT_TRUE(lock.try_lock());
  lock.unlock();
}

TYPED_TEST(LockableTest, fail_try_lock) {
  using LockType = typename TestFixture::Lock;
  LockType lock;
  lock.lock();
  ASSERT_FALSE(lock.try_lock());
  lock.unlock();
}

TYPED_TEST(LockableTest, lock_guard) {
  using LockType = typename TestFixture::Lock;
  LockType lock;
  std::lock_guard<LockType> guard(lock);
  ASSERT_FALSE(lock.try_lock());
}

template <typename LockType>
void LockableTest_HighContention_ThreadFunc(int kIterations, LockType& lock,
                                            int32_t& counter) {
  for (int i = 0; i < kIterations; ++i) {
    std::lock_guard<LockType> guard(lock);
    if (counter >= 0)  // Note: needed to prevent compiler optimizing this away
      ++counter;
  }
}

TYPED_TEST(LockableTest, HighContention) {
  using LockType = typename TestFixture::Lock;
  constexpr int kIterations = 100000;
  constexpr int kThreads = 4;
  LockType lock;
  int32_t counter = 0;

  std::vector<std::thread> threadVector;
  for (int n = 0; n < kThreads; ++n) {
    threadVector.emplace_back(LockableTest_HighContention_ThreadFunc<LockType>,
                              kIterations, std::ref(lock), std::ref(counter));
  }
  for (auto& t : threadVector) {
    t.join();
  }

  EXPECT_EQ(kIterations * kThreads, counter);
}

////////////////////////////////////////////////////////////////////////////////

TYPED_TEST(SharedLockableTest, lock_shared) {
  using LockType = typename TestFixture::Lock;
  LockType lock;
  lock.lock_shared();
  lock.unlock_shared();
}

TYPED_TEST(SharedLockableTest, lock_shared_many) {
  using LockType = typename TestFixture::Lock;
  LockType lock;
  constexpr int kCount = 10000;  // Minimum requirement for SharedMutex
  for (int i = 0; i < kCount; ++i) {
    lock.lock_shared();
  }
  for (int i = 0; i < kCount; ++i) {
    lock.unlock_shared();
  }
}

TYPED_TEST(SharedLockableTest, try_lock_shared) {
  using LockType = typename TestFixture::Lock;
  {
    LockType lock;
    ASSERT_TRUE(lock.try_lock_shared());
    lock.unlock_shared();
  }
  {
    LockType lock;
    lock.lock_shared();
    ASSERT_TRUE(lock.try_lock_shared());
    lock.unlock_shared();
    lock.unlock_shared();
  }
}

TYPED_TEST(SharedLockableTest, try_lock_shared_many) {
  using LockType = typename TestFixture::Lock;
  LockType lock;
  constexpr int kCount = 10000;  // Minimum requirement for SharedMutex
  for (int i = 0; i < kCount; ++i) {
    EXPECT_TRUE(lock.try_lock_shared());
  }
  for (int i = 0; i < kCount; ++i) {
    lock.unlock_shared();
  }
}

TYPED_TEST(SharedLockableTest, fail_try_lock_shared) {
  using LockType = typename TestFixture::Lock;

  LockType lock;
  lock.lock();
  ASSERT_FALSE(lock.try_lock_shared());
  lock.unlock();
}

template <typename LockType>
void SharedLockableTest_HighContention_ReaderThreadFunc(int kIterations,
                                                        LockType& lock,
                                                        int32_t& counter) {
  for (int i = 0; i < kIterations; ++i) {
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    std::shared_lock<LockType> guard(lock);
    EXPECT_EQ(1, counter);
  }
}

template <typename LockType>
void SharedLockableTest_HighContention_WriterThreadFunc(int kIterations,
                                                        LockType& lock,
                                                        int32_t& counter) {
  for (int i = 0; i < kIterations; ++i) {
    std::lock_guard<LockType> guard(lock);
    counter = 0;
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    counter = 1;
  }
}

TYPED_TEST(SharedLockableTest, HighContention) {
  using LockType = typename TestFixture::Lock;
  constexpr int kReaders = 4;
  constexpr int kWriters = 1;
  constexpr int kIterations = 1000;

  LockType lock;
  int32_t counter = 1;

  std::vector<std::thread> threadVector;
  for (int n = 0; n < kWriters; ++n) {
    threadVector.emplace_back(
        SharedLockableTest_HighContention_WriterThreadFunc<LockType>,
        kIterations, std::ref(lock), std::ref(counter));
  }
  for (int n = 0; n < kReaders; ++n) {
    threadVector.emplace_back(
        SharedLockableTest_HighContention_ReaderThreadFunc<LockType>,
        kIterations, std::ref(lock), std::ref(counter));
  }
  for (auto& t : threadVector) {
    t.join();
  }
}

////////////////////////////////////////////////////////////////////////////////

TYPED_TEST(SpinLockTest, SizeOf) {
  using LockType = typename TestFixture::Lock;
  LockType lock;
  EXPECT_EQ(1, sizeof(LockType));
}

////////////////////////////////////////////////////////////////////////////////

TYPED_TEST(SharedSpinLockTest, SizeOf) {
  using LockType = typename TestFixture::Lock;
  LockType lock;
  EXPECT_GE(4, sizeof(LockType));
}

////////////////////////////////////////////////////////////////////////////////

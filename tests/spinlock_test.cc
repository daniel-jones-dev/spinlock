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

#include <spinlock/spinlock.h>

#include <gtest/gtest.h>

using spinlock::SpinLock;

////////////////////////////////////////////////////////////////////////////////

template <typename _SpinLockType>
class SpinLockTest : public ::testing::Test {
 protected:
  using SpinLock = _SpinLockType;

};

////////////////////////////////////////////////////////////////////////////////

// Define types to test with
using SpinLockTypes = ::testing::Types<
    spinlock::SpinLock,
    spinlock::SpinLock_Yield,
    spinlock::SpinLock_MemoryOrder,
    spinlock::SpinLock_MemoryOrder_Yield
>;

TYPED_TEST_CASE(SpinLockTest, SpinLockTypes);

////////////////////////////////////////////////////////////////////////////////

TYPED_TEST(SpinLockTest, lock) {
  using SpinLockType  = typename TestFixture::SpinLock;
  SpinLockType lock;
  lock.lock();
  lock.unlock();
}

TYPED_TEST(SpinLockTest, try_lock) {
  using SpinLockType  = typename TestFixture::SpinLock;
  SpinLockType lock;
  ASSERT_TRUE(lock.try_lock());
  lock.unlock();
}

TYPED_TEST(SpinLockTest, fail_try_lock) {
  using SpinLockType  = typename TestFixture::SpinLock;
  SpinLockType lock;
  lock.lock();
  ASSERT_FALSE(lock.try_lock());
  lock.unlock();
}

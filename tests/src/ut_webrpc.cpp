/*
 * Copyright (c) 2024-2044, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines and implements the BofImgui unit tests
 *
 * Author:      Bernard HARMEL: b.harmel@evs.com
 *
 * History:
 * V 1.00  Feb 19 2024  BHA : Initial release
 */
#include "gtestrunner.h"
#include <bofwebrpc/bofwebrpc.h>

class bofwebrpc_tests : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Initialize any common resources before each test
  }

  void TearDown() override
  {
    // Cleanup common resources after each test
  }
};

TEST_F(bofwebrpc_tests, Test)
{
  EXPECT_TRUE(true);
}

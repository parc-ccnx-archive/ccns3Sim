/*
 * Copyright (c) 2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX OR PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/* ################################################################################
 * #
 * # PATENT NOTICE
 * #
 * # This software is distributed under the BSD 2-clause License (see LICENSE
 * # file).  This BSD License does not make any patent claims and as such, does
 * # not act as a patent grant.  The purpose of this section is for each contributor
 * # to define their intentions with respect to intellectual property.
 * #
 * # Each contributor to this source code is encouraged to state their patent
 * # claims and licensing mechanisms for any contributions made. At the end of
 * # this section contributors may each make their own statements.  Contributor's
 * # claims and grants only apply to the pieces (source code, programs, text,
 * # media, etc) that they have contributed directly to this software.
 * #
 * # There is no guarantee that this section is complete, up to date or accurate. It
 * # is up to the contributors to maintain their portion of this section and up to
 * # the user of the software to verify any claims herein.
 * #
 * # Do not remove this header notification.  The contents of this section must be
 * # present in all distributions of the software.  You may only modify your own
 * # intellectual property statements.  Please provide contact information.
 *
 * - Palo Alto Research Center, Inc
 * This software distribution does not grant any rights to patents owned by Palo
 * Alto Research Center, Inc (PARC). Rights to these patents are available via
 * various mechanisms. As of January 2016 PARC has committed to FRAND licensing any
 * intellectual property used by its contributions to this software. You may
 * contact PARC at cipo@parc.com for more information or visit http://www.ccnx.org
 */




#ifndef CCNS3SIM_TESTMACROS_H
#define CCNS3SIM_TESTMACROS_H

/**
 * \defgroup ccnx-test Unit Tests
 * \ingroup ccnx
 *
 * \section Writing NS3-style unit tests for CCNx
 *
 * - A TestSuite is made up of TestCase classes and a TestSuite class that wraps them together.
 * - Because all tests are compiled in to the same library, all TestCase and TestSuite must have unique names.
 *
 * CCNx conventions:
 * - In a test module for class CCNxFoo:
 *    - Use a namespace to wrap all unit tests and the test suite.  Name it `ns3::ccnx::CCNxFooTests`.
 *    - Use BeginTest() and EndTest() to wrap the TestCase classes.  To test a method named Bar, use a name like
 *      `TestBar` or `TestBar_ConditionX` and `TestBar_ConditionY`.
 *    - Use a TestSuite named `TestSuiteCCNxFoo` and a global variable g_TestSuiteCCNxFoo.
 *
 */

/**
 * \ingroup ccnx-test
 *
 * Macros to make writing test easier.  Use them like this:
 *
 * Example:
 * @code
 *
 * namespace TestSuiteFoo {
 *
 * BeginTest(TestGetVersion)
 *   {
 *       printf("DoRun TestGetVersion\n");
 *       NS_TEST_EXPECT_MSG_EQ (true, true, "The truth");
 *   }
 * EndTest()
 *
 * static class TestSuiteFoo : public TestSuite
 * {
 * public:
 *   TestSuiteFoo() : TestSuite ("TestSuiteFoo", UNIT)
 *   {
 *       AddTestCase (new TestGetVersion (), TestCase::QUICK);
 *   }
 * } g_TestSuiteFoo;
 *
 * }; // namespace
 *
 * @endcode
 */
#define BeginTest(test_name) \
  class test_name : public TestCase { \
public: \
    test_name () : TestCase ("Test " #test_name) {}; \
    ~test_name () {}; \
    virtual void DoRun (void)

/**
 * \ingroup ccnx-test
 *
 * Begin a test that is derived from a base cass in addition to ns3::TestCase
 *
 * \param test_name The class name of the test
 * \param base_class The second derived class of the test
 */
#define BeginDerivedTest(test_name, base_class) \
  class test_name : public TestCase, \
                    public base_class { \
public: \
    test_name () : TestCase ("Test " #test_name) {}; \
    ~test_name () {}; \
    virtual void DoRun (void)

/**
 * \ingroup ccnx-test
 *
 * Ends a ns3::TestCase class block
 *
 * Example:
 * @code
 * namespace TestSuiteFoo {
 *
 * BeginTest(TestGetVersion)
 *   {
 *       printf("DoRun TestGetVersion\n");
 *       NS_TEST_EXPECT_MSG_EQ (true, true, "The truth");
 *   }
 * EndTest()
 * };
 * @endcode
*/
#define EndTest() \
  };

/**
 * \ingroup ccnx-test
 *
 * Hexdump a buffer
 * \param label A text string to display before the buffer
 * \param size The number of bytes to display
 * \param data The buffer
 */
static inline void
hexdump (const char *label, size_t size, const uint8_t *data)
{
  printf ("hexdump %s:\n", label);
  for (size_t i = 0; i < size; i++)
    {
      printf ("%02x ", data[i]);
      if ((i + 1) % 16 == 0)
        {
          printf ("\n");
        }
    }
  if (size % 16 != 0)
    {
      printf ("\n");
    }
}

#endif //CCNS3SIM_TESTMACROS_H

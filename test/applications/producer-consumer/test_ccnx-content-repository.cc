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
#include "ns3/test.h"
#include "ns3/ccnx-name.h"
#include "ns3/ccnx-content-repository.h"
#include "../../TestMacros.h"

using namespace ns3;
using namespace ns3::ccnx;

namespace TestSuiteCCNxContentRepository {

BeginTest (Constructor)
{
  printf ("TestSuiteCCNxContentRepository Constructor_withAllParams DoRun\n");
  /*
   * Creating the constructor with all params.
   */
  Ptr <const CCNxName> prefix = Create <CCNxName> ("ccnx:/name=test/name=producer");
  uint32_t size = 10;
  uint32_t count = 10;
  Ptr <CCNxContentRepository> globalContentRepository = Create <CCNxContentRepository> (prefix,size,count);
  bool exists = (globalContentRepository);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null pointer");
}
EndTest ()

BeginTest (Test_GetContentObjectCount)
{
  printf ("TestSuiteCCNxContentRepository GetContentObjectSize DoRun\n");
  /*
   * Creating the constructor with all params.
   */
  Ptr <const CCNxName> prefix = Create <CCNxName> ("ccnx:/name=test/name=producer");
  uint32_t size = 10;
  uint32_t count = 10;
  Ptr <CCNxContentRepository> globalContentRepository = Create <CCNxContentRepository> (prefix,size,count);
  bool exists = (globalContentRepository);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null pointer");
  uint32_t repoCount = globalContentRepository->GetContentObjectCount ();
  NS_TEST_EXPECT_MSG_EQ (count, repoCount, "The repository content object count did not match");
}
EndTest ()

BeginTest (Test_GetContentObjectSize)
{
  printf ("TestSuiteCCNxContentRepository GetContentObjectSize DoRun\n");
  /*
   * Creating the constructor with all params.
   */
  Ptr <const CCNxName> prefix = Create <CCNxName> ("ccnx:/name=test/name=producer");
  uint32_t size = 10;
  uint32_t count = 10;
  Ptr <CCNxContentRepository> globalContentRepository = Create <CCNxContentRepository> (prefix,size,count);
  bool exists = (globalContentRepository);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null pointer");
  uint32_t repoSize = globalContentRepository->GetContentObjectSize ();
  NS_TEST_EXPECT_MSG_EQ (count, repoSize, "The respository content object size did not match");
}
EndTest ()

BeginTest (Test_GetRandomName)
{
  printf ("TestSuiteCCNxContentRepository Test_GetRandomName DoRun\n");
  /*
   * Creating the constructor with all params.
   */
  Ptr <const CCNxName> prefix = Create <CCNxName> ("ccnx:/name=test/name=producer");
  uint32_t size = 10;
  uint32_t count = 10;
  Ptr <CCNxContentRepository> globalContentRepository = Create <CCNxContentRepository> (prefix,size,count);
  bool exists = (globalContentRepository);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null pointer");
  Ptr <const CCNxName> randName = globalContentRepository->GetRandomName ();
  exists = (randName);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null pointer");
  std::cout << *randName << std::endl;
}
EndTest ()

BeginTest (Test_GetRepositoryPrefix)
{
  printf ("TestSuiteCCNxContentRepository Test_GetRepositoryPrefix DoRun\n");
  /*
   * Test case to see if a new prefix containing a chunk was ever a part of the repository.
   */
  Ptr <const CCNxName> truth = Create <CCNxName> ("ccnx:/name=test/name=producer");
  uint32_t size = 10;
  uint32_t count = 10;
  Ptr <CCNxContentRepository> globalContentRepository = Create <CCNxContentRepository> (truth,size,count);
  bool exists = (globalContentRepository);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null pointer");
  Ptr <const CCNxName> value = globalContentRepository->GetRepositoryPrefix ();
  bool result = truth->Equals (*value);
  NS_TEST_EXPECT_MSG_EQ (result, true, "The repository has not been setup right");

}
EndTest ()

BeginTest (Test_GetContentObject)
{
  printf ("TestSuiteCCNxContentRepository Test_GetContentObject DoRun\n");
  /*
   * Test case to see if a new prefix containing a chunk was ever a part of the repository.
   */
  Ptr <const CCNxName> prefix = Create <CCNxName> ("ccnx:/name=test/name=producer");
  uint32_t size = 10;
  uint32_t count = 10;
  bool exists;
  Ptr <CCNxContentRepository> globalContentRepository = Create <CCNxContentRepository> (prefix,size,count);
  exists = (globalContentRepository);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Got null pointer");
  Ptr <const CCNxName> randName = globalContentRepository->GetRandomName ();
  std::cout << "Random Name Generated " << *randName << std::endl;
  exists = (randName);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Random name failed to generate");

  Ptr <CCNxContentObject> contentResponse = globalContentRepository->GetContentObject (randName);
  exists = (contentResponse);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Failed to get content Object");

  Ptr <const CCNxName> contentResponseName = contentResponse->GetName ();
  exists = (contentResponseName);
  NS_TEST_EXPECT_MSG_EQ (exists, true, "Failed to get extract name from content Object");
  std::cout << "Name from contentResponseName  " << *contentResponseName << std::endl;
  if (randName->Equals (*contentResponseName))
    {
      exists = true;
    }
  else
    {
      exists = false;
    }

  NS_TEST_EXPECT_MSG_EQ (exists, true, "Bad Content Object Generated");
}
EndTest ()



/*
 *
  * Ptr<CCNxContentObject> GetContentObject(Ptr <const CCNxName>contentObjectName);
 *
 */
/**
 * \ingroup ccnx-test
 *
 * \brief Test Suite for CCNxContentRepository
 */
static class TestCCNxContentRepository : public TestSuite
{
public:
  TestCCNxContentRepository () : TestSuite ("ccnx-content-repository", UNIT)
  {
    AddTestCase (new Constructor (), TestCase::QUICK);
    AddTestCase (new Test_GetContentObjectCount (), TestCase::QUICK);
    AddTestCase (new Test_GetContentObjectSize (), TestCase::QUICK);
    AddTestCase (new Test_GetRandomName (), TestCase::QUICK);
    AddTestCase (new Test_GetRepositoryPrefix, TestCase::QUICK);
    AddTestCase (new Test_GetContentObject, TestCase::QUICK);
  }
} g_TestSuiteCCNxContentRepository;

}




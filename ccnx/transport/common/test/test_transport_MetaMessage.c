/*
 * Copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * 
 * ################################################################################
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
/**
 * @author Alan Walendowski, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include "../transport_MetaMessage.c"
#include <stdio.h>

#include <LongBow/unit-test.h>

#include <parc/algol/parc_SafeMemory.h>

#include <ccnx/common/validation/ccnxValidation_CRC32C.h>

LONGBOW_TEST_RUNNER(ccnx_MetaMessage)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);

    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnx_MetaMessage)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnx_MetaMessage)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxMetaMessage_Acquire_Release);
    LONGBOW_RUN_TEST_CASE(Global, ccnxMetaMessage_CreateFromContentObject);
    LONGBOW_RUN_TEST_CASE(Global, ccnxMetaMessage_CreateFromControl);
    LONGBOW_RUN_TEST_CASE(Global, ccnxMetaMessage_CreateFromInterest);
    LONGBOW_RUN_TEST_CASE(Global, ccnxMetaMessage_Display);
    LONGBOW_RUN_TEST_CASE(Global, ccnxMetaMessage_GetContentObject);
    LONGBOW_RUN_TEST_CASE(Global, ccnxMetaMessage_GetControl);
    LONGBOW_RUN_TEST_CASE(Global, ccnxMetaMessage_GetInterest);
    LONGBOW_RUN_TEST_CASE(Global, ccnxMetaMessage_IsContentObject);
    LONGBOW_RUN_TEST_CASE(Global, ccnxMetaMessage_IsControl);
    LONGBOW_RUN_TEST_CASE(Global, ccnxMetaMessage_IsInterest);
    LONGBOW_RUN_TEST_CASE(Global, ccnxMetaMessage_EncodeDecode);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, ccnxMetaMessage_Acquire_Release)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar");
    CCNxInterest *interest = ccnxInterest_CreateSimple(name);
    CCNxMetaMessage *portalMessage = ccnxMetaMessage_CreateFromInterest(interest);

    CCNxMetaMessage *ref1 = ccnxMetaMessage_Acquire(portalMessage);
    CCNxMetaMessage *ref2 = ccnxMetaMessage_Acquire(portalMessage);
    CCNxMetaMessage *ref3 = ccnxMetaMessage_Acquire(portalMessage);

    ccnxMetaMessage_Release(&ref1);
    assertNull(ref1, "Expected pointer to pointer to be null after Release()");

    ccnxMetaMessage_Release(&ref2);
    assertNull(ref2, "Expected pointer to pointer to be null after Release()");

    ccnxMetaMessage_Release(&ref3);
    assertNull(ref3, "Expected pointer to pointer to be null after Release()");

    ccnxMetaMessage_Release(&portalMessage);
    ccnxInterest_Release(&interest);
    ccnxName_Release(&name);
}

LONGBOW_TEST_CASE(Global, ccnxMetaMessage_CreateFromContentObject)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar");
    PARCBuffer *payload = parcBuffer_WrapCString("This is some data. It's not good data, but it is data.");
    CCNxContentObject *contentObject = ccnxContentObject_CreateWithNameAndPayload(name, payload);

    CCNxMetaMessage *portalMessage = ccnxMetaMessage_CreateFromContentObject(contentObject);
    assertNotNull(portalMessage, "Expected a non-null portal message");
    ccnxMetaMessage_Release(&portalMessage);

    ccnxContentObject_Release(&contentObject);
    parcBuffer_Release(&payload);
    ccnxName_Release(&name);
}

#ifndef BUGZID_712
LONGBOW_TEST_CASE(Global, ccnxMetaMessage_CreateFromControl)
{
    testUnimplemented("");
}
#endif // !BUGZID_712

LONGBOW_TEST_CASE(Global, ccnxMetaMessage_CreateFromInterest)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar");
    CCNxInterest *interest = ccnxInterest_CreateSimple(name);

    CCNxMetaMessage *portalMessage = ccnxMetaMessage_CreateFromInterest(interest);
    assertNotNull(portalMessage, "Expected a non-null portal message");
    ccnxMetaMessage_Release(&portalMessage);

    ccnxInterest_Release(&interest);
    ccnxName_Release(&name);
}

#ifndef BUGZID_712
LONGBOW_TEST_CASE(Global, ccnxMetaMessage_Display)
{
    testUnimplemented("");
}
#endif // !BUGZID_712

LONGBOW_TEST_CASE(Global, ccnxMetaMessage_GetContentObject)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar");
    PARCBuffer *payload = parcBuffer_WrapCString("This is some data. It's not good data, but it is data.");
    CCNxContentObject *contentObject = ccnxContentObject_CreateWithNameAndPayload(name, payload);

    CCNxMetaMessage *portalMessage = ccnxMetaMessage_CreateFromContentObject(contentObject);

    CCNxContentObject *reference = ccnxMetaMessage_GetContentObject(portalMessage);

#ifndef BUGZID_712
    // TODO: We need a ccnxContentObject_Equals()!
    // assertTrue(ccnxContentObject_Equals(contentObject, reference), "Expected reference to equal original contentObject");
#endif // !BUGZID_712
    ccnxContentObject_AssertValid(reference);

    ccnxMetaMessage_Release(&portalMessage);

    ccnxContentObject_Release(&contentObject);
    parcBuffer_Release(&payload);
    ccnxName_Release(&name);
}

#ifndef BUGZID_712
LONGBOW_TEST_CASE(Global, ccnxMetaMessage_GetControl)
{
    testUnimplemented("");
}
#endif // !BUGZID_712

LONGBOW_TEST_CASE(Global, ccnxMetaMessage_GetInterest)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar");
    CCNxInterest *interest = ccnxInterest_CreateSimple(name);
    CCNxMetaMessage *portalMessage = ccnxMetaMessage_CreateFromInterest(interest);
    CCNxInterest *reference = ccnxMetaMessage_GetInterest(portalMessage);

    assertTrue(ccnxInterest_Equals(interest, reference), "Expected reference to equal original interest");
    ccnxInterest_AssertValid(reference);

    ccnxInterest_Release(&reference);
    ccnxMetaMessage_Release(&portalMessage);
    ccnxName_Release(&name);
}

LONGBOW_TEST_CASE(Global, ccnxMetaMessage_IsContentObject)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar");
    PARCBuffer *payload = parcBuffer_WrapCString("This is some data. It's not good data, but it is data.");
    CCNxContentObject *contentObject = ccnxContentObject_CreateWithNameAndPayload(name, payload);

    CCNxMetaMessage *portalMessage = ccnxMetaMessage_CreateFromContentObject(contentObject);

    assertTrue(ccnxMetaMessage_IsContentObject(portalMessage), "Expected portal message to be an ContentObject");
    assertFalse(ccnxMetaMessage_IsInterest(portalMessage), "Did not expect portal message to be an Interest");
    assertFalse(ccnxMetaMessage_IsControl(portalMessage), "Did not expect portal message to be a Control message");

    ccnxMetaMessage_Release(&portalMessage);
    ccnxContentObject_Release(&contentObject);
    parcBuffer_Release(&payload);
    ccnxName_Release(&name);
}

#ifndef BUGZID_712
LONGBOW_TEST_CASE(Global, ccnxMetaMessage_IsControl)
{
    testUnimplemented("");
}
#endif // !BUGZID_712

LONGBOW_TEST_CASE(Global, ccnxMetaMessage_IsInterest)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar");
    CCNxInterest *interest = ccnxInterest_CreateSimple(name);
    CCNxMetaMessage *portalMessage = ccnxMetaMessage_CreateFromInterest(interest);

    assertTrue(ccnxMetaMessage_IsInterest(portalMessage), "Expected portal message to be an Interest");
    assertFalse(ccnxMetaMessage_IsContentObject(portalMessage), "Did not expect portal message to be a ContentObject");
    assertFalse(ccnxMetaMessage_IsControl(portalMessage), "Did not expect portal message to be a Control message");

    ccnxMetaMessage_Release(&portalMessage);
    ccnxInterest_Release(&interest);
    ccnxName_Release(&name);
}

LONGBOW_TEST_CASE(Global, ccnxMetaMessage_EncodeDecode)
{
    CCNxName *name = ccnxName_CreateFromCString("lci:/foo/bar");
    CCNxInterest *interest = ccnxInterest_CreateSimple(name);
    ccnxName_Release(&name);

    PARCSigner *signer = ccnxValidationCRC32C_CreateSigner(); // Would really be SHA256 or something.

    // Encode it to wire format.
    PARCBuffer *wireFormatBuffer = ccnxMetaMessage_CreateWireFormatBuffer(interest, signer);

    // Now decode it from wireformat.
    CCNxMetaMessage *decodedMessage = ccnxMetaMessage_CreateFromWireFormatBuffer(wireFormatBuffer);

    // At this point, the unpacked dictionary should be equivalent to the original interest.
    assertTrue(ccnxInterest_Equals(interest, decodedMessage), "Expected an equivalent interest to be unpacked");

    parcBuffer_Release(&wireFormatBuffer);
    ccnxInterest_Release(&interest);
    ccnxMetaMessage_Release(&decodedMessage);
    parcSigner_Release(&signer);
}

#ifndef BUGZID_712
LONGBOW_TEST_CASE(Global, ccnxMetaMessage_Release)
{
    testUnimplemented("");
}
#endif // !BUGZID_712

LONGBOW_TEST_FIXTURE(Local)
{
}

LONGBOW_TEST_FIXTURE_SETUP(Local)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Local)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnx_MetaMessage);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}

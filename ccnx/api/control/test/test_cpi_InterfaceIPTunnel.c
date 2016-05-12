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

#include "../cpi_InterfaceIPTunnel.c"
#include <LongBow/unit-test.h>
#include <parc/algol/parc_SafeMemory.h>


LONGBOW_TEST_RUNNER(cpi_InterfaceIPTunnel)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(cpi_InterfaceIPTunnel)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(cpi_InterfaceIPTunnel)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, cpiInterfaceIPTunnel_Copy);
    LONGBOW_RUN_TEST_CASE(Global, cpiInterfaceIPTunnel_Create_Destroy);
    LONGBOW_RUN_TEST_CASE(Global, cpiInterfaceIPTunnel_GetAddresses);
    LONGBOW_RUN_TEST_CASE(Global, cpiInterfaceIPTunnel_GetIndex);
    LONGBOW_RUN_TEST_CASE(Global, cpiInterfaceIPTunnel_GetState);
    LONGBOW_RUN_TEST_CASE(Global, cpiInterfaceIPTunnel_ToJSON);
    LONGBOW_RUN_TEST_CASE(Global, cpiInterfaceIPTunnel_FromJSON);
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

LONGBOW_TEST_CASE(Global, cpiInterfaceIPTunnel_Copy)
{
    CPIAddress *src = cpiAddress_CreateFromInet(&((struct sockaddr_in) { .sin_addr.s_addr = 0x01020304 }));
    CPIAddress *dst = cpiAddress_CreateFromInet(&((struct sockaddr_in) { .sin_addr.s_addr = 0x05060708 }));
    CPIInterfaceIPTunnel *iptun = cpiInterfaceIPTunnel_Create(1, src, dst, IPTUN_TCP, "tun0");

    CPIInterfaceIPTunnel *copy = cpiInterfaceIPTunnel_Copy(iptun);

    assertTrue(cpiInterfaceIPTunnel_GetIndex(copy) == cpiInterfaceIPTunnel_GetIndex(iptun),
               "ifidx did not match, expected %u got %u",
               cpiInterfaceIPTunnel_GetIndex(iptun),
               cpiInterfaceIPTunnel_GetIndex(copy));

    assertTrue(cpiInterfaceIPTunnel_GetState(copy) == cpiInterfaceIPTunnel_GetState(iptun),
               "states did not match, expected %d got %d",
               cpiInterfaceIPTunnel_GetState(iptun),
               cpiInterfaceIPTunnel_GetState(copy));

    assertTrue(cpiAddress_Equals(cpiInterfaceIPTunnel_GetSourceAddress(copy), cpiInterfaceIPTunnel_GetSourceAddress(iptun)),
               "did not get same source address");
    assertTrue(cpiAddress_Equals(cpiInterfaceIPTunnel_GetDestinationAddress(copy), cpiInterfaceIPTunnel_GetDestinationAddress(iptun)),
               "did not get same destination address");

    assertTrue(cpiInterfaceIPTunnel_GetTunnelType(copy) == cpiInterfaceIPTunnel_GetTunnelType(iptun),
               "did not get same tunnel types!");

    assertNotNull(copy->symbolic, "Copy has null symbolic name");
    assertTrue(strcmp(iptun->symbolic, copy->symbolic) == 0, "symbolics name wrong expected '%s' got '%s'",
               iptun->symbolic, copy->symbolic);

    cpiInterfaceIPTunnel_Release(&copy);
    cpiInterfaceIPTunnel_Release(&iptun);
}

LONGBOW_TEST_CASE(Global, cpiInterfaceIPTunnel_Create_Destroy)
{
    CPIAddress *src = cpiAddress_CreateFromInet(&((struct sockaddr_in) { .sin_addr.s_addr = 0x01020304 }));
    CPIAddress *dst = cpiAddress_CreateFromInet(&((struct sockaddr_in) { .sin_addr.s_addr = 0x05060708 }));
    CPIInterfaceIPTunnel *iptun = cpiInterfaceIPTunnel_Create(1, src, dst, IPTUN_GRE, "tun0");
    cpiInterfaceIPTunnel_Release(&iptun);

    assertTrue(parcMemory_Outstanding() == 0, "Imbalance after destroying");
}

LONGBOW_TEST_CASE(Global, cpiInterfaceIPTunnel_GetAddresses)
{
    CPIAddress *src = cpiAddress_CreateFromInet(&((struct sockaddr_in) { .sin_addr.s_addr = 0x01020304 }));
    CPIAddress *dst = cpiAddress_CreateFromInet(&((struct sockaddr_in) { .sin_addr.s_addr = 0x05060708 }));
    CPIInterfaceIPTunnel *iptun = cpiInterfaceIPTunnel_Create(1, src, dst, IPTUN_TCP, "tun0");

    const CPIAddress *test;

    test = cpiInterfaceIPTunnel_GetSourceAddress(iptun);
    assertTrue(cpiAddress_Equals(src, test), "Address lists did not match");

    test = cpiInterfaceIPTunnel_GetDestinationAddress(iptun);
    assertTrue(cpiAddress_Equals(dst, test), "Address lists did not match");

    cpiInterfaceIPTunnel_Release(&iptun);
}

LONGBOW_TEST_CASE(Global, cpiInterfaceIPTunnel_GetIndex)
{
    CPIAddress *src = cpiAddress_CreateFromInet(&((struct sockaddr_in) { .sin_addr.s_addr = 0x01020304 }));
    CPIAddress *dst = cpiAddress_CreateFromInet(&((struct sockaddr_in) { .sin_addr.s_addr = 0x05060708 }));
    CPIInterfaceIPTunnel *iptun = cpiInterfaceIPTunnel_Create(1, src, dst, IPTUN_TCP, "tun0");

    assertTrue(cpiInterfaceIPTunnel_GetIndex(iptun) == 1, "ifidx did not match");

    cpiInterfaceIPTunnel_Release(&iptun);
}

LONGBOW_TEST_CASE(Global, cpiInterfaceIPTunnel_GetState)
{
    CPIAddress *src = cpiAddress_CreateFromInet(&((struct sockaddr_in) { .sin_addr.s_addr = 0x01020304 }));
    CPIAddress *dst = cpiAddress_CreateFromInet(&((struct sockaddr_in) { .sin_addr.s_addr = 0x05060708 }));
    CPIInterfaceIPTunnel *iptun = cpiInterfaceIPTunnel_Create(1, src, dst, IPTUN_TCP, "tun0");

    assertTrue(cpiInterfaceIPTunnel_GetState(iptun) == CPI_IFACE_UNKNOWN, "state did not match");

    cpiInterfaceIPTunnel_SetState(iptun, CPI_IFACE_UP);
    assertTrue(cpiInterfaceIPTunnel_GetState(iptun) == CPI_IFACE_UP, "state did not match");

    cpiInterfaceIPTunnel_SetState(iptun, CPI_IFACE_DOWN);
    assertTrue(cpiInterfaceIPTunnel_GetState(iptun) == CPI_IFACE_DOWN, "state did not match");

    cpiInterfaceIPTunnel_Release(&iptun);
}

LONGBOW_TEST_CASE(Global, cpiInterfaceIPTunnel_ToJSON)
{
    // The JSON representation depends on the system sockaddr_in format, which
    // varies platform to platform.
#if defined(__APPLE__)
    char *expected = "{\"TUNNEL\":{\"IFIDX\":1,\"SYMBOLIC\":\"tun0\",\"TUNTYPE\":\"TCP\",\"SRC\":{\"ADDRESSTYPE\":\"INET\",\"DATA\":\"AAIAAAQDAgEAAAAAAAAAAA==\"},\"DST\":{\"ADDRESSTYPE\":\"INET\",\"DATA\":\"AAIAAAgHBgUAAAAAAAAAAA==\"}}}";
#elif defined(__linux__)
    char *expected = "{\"TUNNEL\":{\"IFIDX\":1,\"SYMBOLIC\":\"tun0\",\"TUNTYPE\":\"TCP\",\"SRC\":{\"ADDRESSTYPE\":\"INET\",\"DATA\":\"AgAAAAQDAgEAAAAAAAAAAA==\"},\"DST\":{\"ADDRESSTYPE\":\"INET\",\"DATA\":\"AgAAAAgHBgUAAAAAAAAAAA==\"}}}";
#else
    // Case 1033
    testUnimplemented("Platform not supported");
    return;
#endif

    CPIAddress *src = cpiAddress_CreateFromInet(&((struct sockaddr_in) { .sin_addr.s_addr = 0x01020304 }));
    CPIAddress *dst = cpiAddress_CreateFromInet(&((struct sockaddr_in) { .sin_addr.s_addr = 0x05060708 }));
    CPIInterfaceIPTunnel *iptun = cpiInterfaceIPTunnel_Create(1, src, dst, IPTUN_TCP, "tun0");

    PARCJSON *test_json = cpiInterfaceIPTunnel_ToJson(iptun);

    char *actual = parcJSON_ToCompactString(test_json);
    assertEqualStrings(expected, actual);

    parcMemory_Deallocate((void **) &actual);
    parcJSON_Release(&test_json);
    cpiInterfaceIPTunnel_Release(&iptun);
}

LONGBOW_TEST_CASE(Global, cpiInterfaceIPTunnel_FromJSON)
{
    // The JSON representation depends on the system sockaddr_in format, which
    // varies platform to platform.
#if defined(__APPLE__)
    char truth_json_str[] = "{\"TUNNEL\":{\"IFIDX\":1,\"SYMBOLIC\":\"tun0\",\"STATE\":\"UP\",\"TUNTYPE\":\"TCP\",\"SRC\":{\"ADDRESSTYPE\":\"INET\",\"DATA\":\"AAIAAAQDAgEAAAAAAAAAAA==\"},\"DST\":{\"ADDRESSTYPE\":\"INET\",\"DATA\":\"AAIAAAgHBgUAAAAAAAAAAA==\"}}}";
#elif defined(__linux__)
    char truth_json_str[] = "{\"TUNNEL\":{\"IFIDX\":1,\"SYMBOLIC\":\"tun0\",\"STATE\":\"UP\",\"TUNTYPE\":\"TCP\",\"SRC\":{\"ADDRESSTYPE\":\"INET\",\"DATA\":\"AgAAAAQDAgEAAAAAAAAAAA==\"},\"DST\":{\"ADDRESSTYPE\":\"INET\",\"DATA\":\"AgAAAAgHBgUAAAAAAAAAAA==\"}}}";
#else
    // Case 1033
    testUnimplemented("Platform not supported");
    return;
#endif


    CPIAddress *src = cpiAddress_CreateFromInet(&((struct sockaddr_in) { .sin_addr.s_addr = 0x01020304 }));
    CPIAddress *dst = cpiAddress_CreateFromInet(&((struct sockaddr_in) { .sin_addr.s_addr = 0x05060708 }));
    CPIInterfaceIPTunnel *truth = cpiInterfaceIPTunnel_Create(1, src, dst, IPTUN_TCP, "tun0");
    cpiInterfaceIPTunnel_SetState(truth, CPI_IFACE_UP);

    PARCJSON *json = parcJSON_ParseString(truth_json_str);

    CPIInterfaceIPTunnel *test = cpiInterfaceIPTunnel_CreateFromJson(json);
    assertTrue(cpiInterfaceIPTunnel_Equals(truth, test), "IPTunnel interfaces do not match");

    parcJSON_Release(&json);
    cpiInterfaceIPTunnel_Release(&truth);
    cpiInterfaceIPTunnel_Release(&test);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(cpi_InterfaceIPTunnel);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}

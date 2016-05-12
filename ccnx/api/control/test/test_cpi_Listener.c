/*
 * Copyright (c) 2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include "../cpi_Listener.c"
#include <LongBow/unit-test.h>
#include <parc/algol/parc_SafeMemory.h>
#include <arpa/inet.h>
#include <errno.h>

typedef struct test_data {
    CPIListener *listener;

    // the truth values of the connection
    uint8_t macArray[6];
    CPIAddress *macAddress;
    uint16_t ethertype;
    char ifname[16];
    char symbolic[16];
} TestData;

static CPIListener *
_conjureIPObject(CPIInterfaceIPTunnelType type, const char *addressString, uint16_t port, const char *symbolic)
{
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    int result = inet_aton(addressString, &sin.sin_addr);
    assertTrue(result == 1, "failed inet_aton: (%d) %s", errno, strerror(errno));

    CPIAddress *address = cpiAddress_CreateFromInet(&sin);
    CPIListener *listener = cpiListener_CreateIP(type, address, symbolic);
    cpiAddress_Destroy(&address);

    return listener;
}

LONGBOW_TEST_RUNNER(cpi_ConnectionEthernet)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(cpi_ConnectionEthernet)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(cpi_ConnectionEthernet)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, cpiListener_CreateEther);
    LONGBOW_RUN_TEST_CASE(Global, cpiListener_CreateIP);
    LONGBOW_RUN_TEST_CASE(Global, cpiListener_Equals_Ether);
    LONGBOW_RUN_TEST_CASE(Global, cpiListener_Equals_IP);
    LONGBOW_RUN_TEST_CASE(Global, cpiListener_CreateAddMessage);
    LONGBOW_RUN_TEST_CASE(Global, cpiListener_CreateRemoveMessage);
    LONGBOW_RUN_TEST_CASE(Global, cpiListener_IsAddMessage);
    LONGBOW_RUN_TEST_CASE(Global, cpiListener_IsRemoveMessage);
    LONGBOW_RUN_TEST_CASE(Global, cpiListener_FromControl_Ether);
    LONGBOW_RUN_TEST_CASE(Global, cpiListener_FromControl_IP);
    LONGBOW_RUN_TEST_CASE(Global, cpiListener_IsEtherEncap);
    LONGBOW_RUN_TEST_CASE(Global, cpiListener_IsIPEncap);
    LONGBOW_RUN_TEST_CASE(Global, cpiListener_GetEtherType);

    LONGBOW_RUN_TEST_CASE(Global, cpiListener_GetInterfaceName);
    LONGBOW_RUN_TEST_CASE(Global, cpiListener_GetSymbolicName);
    LONGBOW_RUN_TEST_CASE(Global, cpiListener_GetAddress);

    LONGBOW_RUN_TEST_CASE(Global, cpiListener_IsProtocolUdp);
    LONGBOW_RUN_TEST_CASE(Global, cpiListener_IsProtocolTcp);
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


LONGBOW_TEST_CASE(Global, cpiListener_CreateEther)
{
    CPIListener *listener = cpiListener_CreateEther("eth0", 0x0801, "puppy");
    assertNotNull(listener, "Got null IP based listener");
    cpiListener_Release(&listener);
}

LONGBOW_TEST_CASE(Global, cpiListener_CreateIP)
{
    CPIListener *listener = _conjureIPObject(IPTUN_UDP, "127.0.0.1", 9596, "puppy");
    assertNotNull(listener, "Got null IP based listener");
    cpiListener_Release(&listener);
}

LONGBOW_TEST_CASE(Global, cpiListener_Equals_Ether)
{
    CPIListener *x = cpiListener_CreateEther("eth0", 0x0801, "puppy");
    CPIListener *y = cpiListener_CreateEther("eth0", 0x0801, "puppy");
    CPIListener *z = cpiListener_CreateEther("eth0", 0x0801, "puppy");

    CPIListener *t = cpiListener_CreateEther("eth1", 0x0801, "puppy");
    CPIListener *u = cpiListener_CreateEther("eth0", 0x0802, "puppy");
    CPIListener *v = cpiListener_CreateEther("eth0", 0x0801, "kitten");

    assertEqualsContract(cpiListener_Equals, x, y, z, t, u, v);

    cpiListener_Release(&x);
    cpiListener_Release(&y);
    cpiListener_Release(&z);
    cpiListener_Release(&t);
    cpiListener_Release(&u);
    cpiListener_Release(&v);
}

LONGBOW_TEST_CASE(Global, cpiListener_Equals_IP)
{
    CPIListener *x = _conjureIPObject(IPTUN_UDP, "127.0.0.1", 9596, "puppy");
    CPIListener *y = _conjureIPObject(IPTUN_UDP, "127.0.0.1", 9596, "puppy");
    CPIListener *z = _conjureIPObject(IPTUN_UDP, "127.0.0.1", 9596, "puppy");

    CPIListener *t = _conjureIPObject(IPTUN_TCP, "127.0.0.1", 9596, "puppy");
    CPIListener *u = _conjureIPObject(IPTUN_UDP, "127.0.2.1", 9596, "puppy");
    CPIListener *v = _conjureIPObject(IPTUN_UDP, "127.0.0.1", 1111, "puppy");
    CPIListener *w = _conjureIPObject(IPTUN_UDP, "127.0.0.1", 9596, "kitten");

    assertEqualsContract(cpiListener_Equals, x, y, z, t, u, v, w);

    cpiListener_Release(&x);
    cpiListener_Release(&y);
    cpiListener_Release(&z);
    cpiListener_Release(&t);
    cpiListener_Release(&u);
    cpiListener_Release(&v);
    cpiListener_Release(&w);
}

LONGBOW_TEST_CASE(Global, cpiListener_CreateAddMessage)
{
    const char *truthFormat = "{\"CPI_REQUEST\":{\"SEQUENCE\":%d,\"%s\":{\"IFNAME\":\"eth0\",\"ETHERTYPE\":2049,\"SYMBOLIC\":\"puppy\"}}}";

    char buffer[1024];

    // Create the add message
    CPIListener *listener = cpiListener_CreateEther("eth0", 0x0801, "puppy");
    CCNxControl *control = cpiListener_CreateAddMessage(listener);
    assertNotNull(control, "Got null control message");

    // extract the sequence number to put in the truth string
    PARCJSON *json = ccnxControl_GetJson(control);
    uint64_t seqnum = controlPlaneInterface_GetSequenceNumber(json);
    sprintf(buffer, truthFormat, (int) seqnum, KEY_ADDLISTENER);

    char *testString = parcJSON_ToCompactString(json);
    assertTrue(strcmp(buffer, testString) == 0, "Got wrong JSON, expected\n%s\nGot\n%s\n", buffer, testString);
    parcMemory_Deallocate((void **) &testString);

    ccnxControl_Release(&control);
    cpiListener_Release(&listener);
}

LONGBOW_TEST_CASE(Global, cpiListener_CreateRemoveMessage)
{
#if defined(__APPLE__)
    const char *truthFormat = "{\"CPI_REQUEST\":{\"SEQUENCE\":%d,\"%s\":{\"IPROTO\":\"UDP\",\"ADDR\":{\"ADDRESSTYPE\":\"INET\",\"DATA\":\"AAIlfH8AAAEAAAAAAAAAAA==\"},\"SYMBOLIC\":\"puppy\"}}}";
#elif defined(__linux__)
    const char *truthFormat = "{\"CPI_REQUEST\":{\"SEQUENCE\":%d,\"%s\":{\"IPROTO\":\"UDP\",\"ADDR\":{\"ADDRESSTYPE\":\"INET\",\"DATA\":\"AgAlfH8AAAEAAAAAAAAAAA==\"},\"SYMBOLIC\":\"puppy\"}}}";
#else
    // Case 1033
    testUnimplemented("Platform not supported");
    return;
#endif

    char buffer[1024];

    // Create the remove message
    CPIListener *listener = _conjureIPObject(IPTUN_UDP, "127.0.0.1", 9596, "puppy");
    CCNxControl *control = cpiListener_CreateRemoveMessage(listener);
    assertNotNull(control, "Got null control message");

    // extract the sequence number to put in the truth string
    PARCJSON *json = ccnxControl_GetJson(control);
    uint64_t seqnum = controlPlaneInterface_GetSequenceNumber(json);
    sprintf(buffer, truthFormat, (int) seqnum, KEY_REMOVELISTENER);

    char *testString = parcJSON_ToCompactString(json);
    assertTrue(strcmp(buffer, testString) == 0, "Got wrong JSON, expected\n%s\nGot\n%s\n", buffer, testString);
    parcMemory_Deallocate((void **) &testString);

    ccnxControl_Release(&control);
    cpiListener_Release(&listener);
}

LONGBOW_TEST_CASE(Global, cpiListener_IsAddMessage)
{
    CPIListener *listener = cpiListener_CreateEther("eth0", 0x0801, "puppy");
    CCNxControl *control = cpiListener_CreateAddMessage(listener);
    assertNotNull(control, "Got null control message");

    bool test = cpiListener_IsAddMessage(control);
    assertTrue(test, "Add message denies it is one.");

    ccnxControl_Release(&control);
    cpiListener_Release(&listener);
}

LONGBOW_TEST_CASE(Global, cpiListener_IsRemoveMessage)
{
    CPIListener *listener = cpiListener_CreateEther("eth0", 0x0801, "puppy");
    CCNxControl *control = cpiListener_CreateRemoveMessage(listener);
    assertNotNull(control, "Got null control message");

    bool test = cpiListener_IsRemoveMessage(control);
    assertTrue(test, "Add message denies it is one.");

    ccnxControl_Release(&control);
    cpiListener_Release(&listener);
}

LONGBOW_TEST_CASE(Global, cpiListener_FromControl_Ether)
{
    CPIListener *listener = cpiListener_CreateEther("eth0", 0x0801, "puppy");
    CCNxControl *control = cpiListener_CreateAddMessage(listener);

    CPIListener *test = cpiListener_FromControl(control);
    assertTrue(cpiListener_Equals(listener, test), "Listeners do not match")
    {
        printf("Expenected:\n");
        char *str = parcJSON_ToString(ccnxControl_GetJson(control));
        printf("   %s\n", str);
        parcMemory_Deallocate((void **) &str);

        printf("Got:\n");
        CCNxControl *testControl = cpiListener_CreateAddMessage(test);
        str = parcJSON_ToString(ccnxControl_GetJson(testControl));
        printf("   %s\n", str);
        parcMemory_Deallocate((void **) &str);
        ccnxControl_Release(&testControl);
    }

    ccnxControl_Release(&control);
    cpiListener_Release(&test);
    cpiListener_Release(&listener);
}


LONGBOW_TEST_CASE(Global, cpiListener_FromControl_IP)
{
    CPIListener *listener = _conjureIPObject(IPTUN_UDP, "127.0.0.1", 9596, "puppy");
    CCNxControl *control = cpiListener_CreateAddMessage(listener);

    CPIListener *test = cpiListener_FromControl(control);
    assertTrue(cpiListener_Equals(listener, test), "Listeners do not match")
    {
        printf("Expenected:\n");
        char *str = parcJSON_ToString(ccnxControl_GetJson(control));
        printf("   %s\n", str);
        parcMemory_Deallocate((void **) &str);

        printf("Got:\n");
        CCNxControl *testControl = cpiListener_CreateAddMessage(test);
        str = parcJSON_ToString(ccnxControl_GetJson(testControl));
        printf("   %s\n", str);
        parcMemory_Deallocate((void **) &str);
        ccnxControl_Release(&testControl);
    }

    ccnxControl_Release(&control);
    cpiListener_Release(&test);
    cpiListener_Release(&listener);
}

LONGBOW_TEST_CASE(Global, cpiListener_IsEtherEncap)
{
    CPIListener *x = cpiListener_CreateEther("eth0", 0x0801, "puppy");
    bool isTrue = cpiListener_IsEtherEncap(x);
    assertTrue(isTrue, "Ether listener says it is not ether");
    cpiListener_Release(&x);
}

LONGBOW_TEST_CASE(Global, cpiListener_IsIPEncap)
{
    CPIListener *x = _conjureIPObject(IPTUN_UDP, "127.0.0.1", 9596, "puppy");
    bool isTrue = cpiListener_IsIPEncap(x);
    assertTrue(isTrue, "IP listener says it is not IP");
    cpiListener_Release(&x);
}

LONGBOW_TEST_CASE(Global, cpiListener_GetAddress)
{
    CPIListener *x = _conjureIPObject(IPTUN_UDP, "127.0.0.1", 9596, "puppy");
    CPIAddress *test = cpiListener_GetAddress(x);
    assertNotNull(test, "Got null address for IP listener");

    struct sockaddr_in sin;
    cpiAddress_GetInet(test, &sin);
    assertTrue(htons(sin.sin_port) == 9596, "Wrong port expected %u got %u", 9695, htons(sin.sin_port));

    uint32_t testip = htonl(sin.sin_addr.s_addr);
    uint32_t truthip = 0x7F000001;

    assertTrue(testip == truthip, "Wrong IP address expected %#08x got %#08x", truthip, testip);

    cpiListener_Release(&x);
}

LONGBOW_TEST_CASE(Global, cpiListener_GetEtherType)
{
    CPIListener *x = cpiListener_CreateEther("eth0", 0x0801, "puppy");
    uint16_t test = cpiListener_GetEtherType(x);
    assertTrue(test == 0x0801, "Wrong ethertype, got %04x expected %04x", test, 0x0801);
    cpiListener_Release(&x);
}

LONGBOW_TEST_CASE(Global, cpiListener_GetInterfaceName)
{
    CPIListener *x = cpiListener_CreateEther("eth0", 0x0801, "puppy");
    const char *test = cpiListener_GetInterfaceName(x);
    assertTrue(strcmp(test, "eth0") == 0, "Wrong interface name, got '%s' expected '%s'", test, "eth0");
    cpiListener_Release(&x);
}

LONGBOW_TEST_CASE(Global, cpiListener_GetSymbolicName)
{
    CPIListener *x = cpiListener_CreateEther("eth0", 0x0801, "puppy");
    const char *test = cpiListener_GetSymbolicName(x);
    assertTrue(strcmp(test, "puppy") == 0, "Wrong symbolic name, got '%s' expected '%s'", test, "puppy");
    cpiListener_Release(&x);
}

LONGBOW_TEST_CASE(Global, cpiListener_IsProtocolUdp)
{
    CPIListener *x = _conjureIPObject(IPTUN_UDP, "127.0.0.1", 9596, "puppy");
    assertTrue(cpiListener_IsProtocolUdp(x), "UDP listener did not say it was UDP");
    cpiListener_Release(&x);
}

LONGBOW_TEST_CASE(Global, cpiListener_IsProtocolTcp)
{
    CPIListener *x = _conjureIPObject(IPTUN_TCP, "127.0.0.1", 9596, "puppy");
    assertTrue(cpiListener_IsProtocolTcp(x), "TCP listener did not say it was TCP");
    cpiListener_Release(&x);
}

// =========================================

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(cpi_ConnectionEthernet);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}

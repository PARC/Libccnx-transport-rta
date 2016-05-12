/*
 * Copyright (c) 2013-2014, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * Generate packets
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2014, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <LongBow/runtime.h>

typedef enum {
    MODE_SEND,
    MODE_REPLY
} PktGenMode;

typedef enum {
    ENCAP_ETHER,
    ENCAP_UDP
} PktGenEncap;

typedef enum {
    PKTGEN_STREAM,
    PKTGEN_STOPWAIT
} PktGenFlow;

typedef struct {
    PktGenMode mode;
    PktGenEncap encap;
    PktGenFlow flow;

    char *ifname;
    char *etherOrIp;
    char *etherType;
    unsigned count;

    struct timeval startTime;
    struct timeval stopTime;
    unsigned packetCount;
} PktGen;

// ======================================================================

static void
usage(void)
{
    printf("usage: \n");
    printf("  This program functions as a requester and a responder.  They operate in a pair.\n");
    printf("  The test can run over raw Ethernet encapsulation or over UDP\n");
    printf("  The <count> parameter can be an integer or use a 'kmg' suffix for 1000, 1E+6, or 1E+9\n");
    printf("\n");
    printf("  pktgen send  ether <ifname> <dstmac> [ethertype] count <n> (stream | stopwait)\n");
    printf("  pktgen reply ether <ifname> [count <n>]\n");
    printf("\n");
    printf("  This mode sends either a stream or stop-and-wait request to an Ethernet peer\n");
    printf("  pktgen send  udp <ifname> <dstip> <dstport> count <n> (stream | stopwait)\n");
    printf("  pktgen reply udp <ifname> [count <n>]\n");
    printf("\n");
    printf("  Examples:\n");
    printf("    This uses the standard Ethertype of 0x0801.  The replier will stay running forever.\n");
    printf("       pktgen send  ether em1 bc:30:5b:f2:2f:60 count 1M stream\n");
    printf("       pktgen reply ether em1\n");
    printf("\n");
    printf("    This uses a custom ethertype.  The replier will stay running forever.\n");
    printf("       pktgen send  ether em1 bc:30:5b:f2:2f:60 0x9000 count 1M stream\n");
    printf("       pktgen reply ether em1\n");
    printf("\n");
    printf("    An example with UDP\n");
    printf("       pktgen send  udp em1 10.1.0.2 9695 count 1M stopwait\n");
    printf("       pktgen reply udp em1\n");
    printf("\n");
}

static PktGen
parseCommandLine(int argc, char *argv[argc])
{
    PktGen pktgen;
    memset(&pktgen, 0, sizeof(PktGen));

    usage();

    return pktgen;
}

// ======================================================================

static void
generateEther(PktGen *pktgen)
{
    printf("Generating %u ethernet interest messages\n", pktgen->count);
}

static void
replyEther(PktGen *pktgen)
{
    printf("replying up to %u ethernet content objects messages\n", pktgen->count);
}

// ======================================================================

static void
generateUdp(PktGen *pktgen)
{
    printf("Generating %u UDP interest messages\n", pktgen->count);
}

static void
replyUdp(PktGen *pktgen)
{
    printf("replying up to %u UDP content objects messages\n", pktgen->count);
}


// ======================================================================

static void
displayStatistics(PktGen *pktgen)
{
    printf("stats.... coming soon\n");
}

// ======================================================================

static void
runSender(PktGen *pktgen)
{
    switch (pktgen->encap) {
        case ENCAP_ETHER:
            generateEther(pktgen);
            break;

        case ENCAP_UDP:
            generateUdp(pktgen);
            break;

        default:
            trapIllegalValue(pktgen.encap, "Unknown encapsulation: %d", pktgen->encap);
    }
}

static void
runReplier(PktGen *pktgen)
{
    switch (pktgen->encap) {
        case ENCAP_ETHER:
            replyEther(pktgen);
            break;

        case ENCAP_UDP:
            replyUdp(pktgen);
            break;

        default:
            trapIllegalValue(pktgen.encap, "Unknown encapsulation: %d", pktgen->encap);
    }
}

// ======================================================================

int
main(int argc, char *argv[argc])
{
    PktGen pktgen = parseCommandLine(argc, argv);

    switch (pktgen.mode) {
        case MODE_SEND:
            runSender(&pktgen);
            break;

        case MODE_REPLY:
            runReplier(&pktgen);
            break;

        default:
            trapIllegalValue(pktgen.mode, "Unknown mode: %d", pktgen.mode);
    }

    displayStatistics(&pktgen);
}

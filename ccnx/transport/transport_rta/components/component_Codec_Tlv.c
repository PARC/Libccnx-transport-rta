/*
 * Copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/socket.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <LongBow/runtime.h>

#include <parc/algol/parc_Memory.h>

#include <ccnx/transport/common/transport_Message.h>

#include <ccnx/transport/transport_rta/core/rta_Framework_Services.h>
#include <ccnx/transport/transport_rta/core/rta_ProtocolStack.h>
#include <ccnx/transport/transport_rta/core/rta_Connection.h>
#include <ccnx/transport/transport_rta/core/rta_Component.h>

#include <ccnx/common/codec/ccnxCodec_TlvPacket.h>
#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_TlvDictionary.h>

#include <ccnx/common/codec/schema_v1/ccnxCodecSchemaV1_PacketEncoder.h>

#include "component_Codec.h"
#include "codec_Signing.h"

// set to 3 or higher for memory dumps of packets
#ifndef DEBUG_OUTPUT
#define DEBUG_OUTPUT 0
#endif

static int  component_Codec_Tlv_Init(RtaProtocolStack *stack);
static int  component_Codec_Tlv_Opener(RtaConnection *conn);
static void component_Codec_Tlv_Upcall_Read(PARCEventQueue *, PARCEventType event, void *conn);
static void component_Codec_Tlv_Downcall_Read(PARCEventQueue *, PARCEventType event, void *conn);
static int  component_Codec_Tlv_Closer(RtaConnection *conn);
static int  component_Codec_Tlv_Release(RtaProtocolStack *stack);
static void component_Codec_Tlv_StateChange(RtaConnection *conn);

RtaComponentOperations codec_tlv_ops = {
    .init          = component_Codec_Tlv_Init,
    .open          = component_Codec_Tlv_Opener,
    .upcallRead    = component_Codec_Tlv_Upcall_Read,
    .upcallEvent   = NULL,
    .downcallRead  = component_Codec_Tlv_Downcall_Read,
    .downcallEvent = NULL,
    .close         = component_Codec_Tlv_Closer,
    .release       = component_Codec_Tlv_Release,
    .stateChange   = component_Codec_Tlv_StateChange
};

typedef struct codec_connection_state {
    PARCSigner *signer;
} CodecConnectionState;

// ==================
// NULL

static int
component_Codec_Tlv_Init(RtaProtocolStack *stack)
{
    // no ProtocolStack wide state
    return 0;
}


static int
component_Codec_Tlv_Opener(RtaConnection *conn)
{
    struct codec_connection_state *codec_state = parcMemory_AllocateAndClear(sizeof(struct codec_connection_state));
    assertNotNull(codec_state, "%s parcMemory_AllocateAndClear(%zu) returned NULL", __func__, sizeof(struct codec_connection_state));

    codec_state->signer = component_Codec_GetSigner(conn);

    rtaConnection_SetPrivateData(conn, CODEC_TLV, codec_state);

    if (DEBUG_OUTPUT) {
        printf("%9" PRIu64 " %s connection %u codec signer %p private %p\n",
               rtaFramework_GetTicks(rtaProtocolStack_GetFramework(rtaConnection_GetStack(conn))),
               __func__,
               rtaConnection_GetConnectionId(conn),
               (void *) codec_state->signer,
               (void *) codec_state);
    }

    return 0;
}

static void
upcallDictionary(TransportMessage *tm, PARCEventQueue *out, RtaComponentStats *stats)
{
    CCNxTlvDictionary *dictionary = transportMessage_GetDictionary(tm);

    PARCBuffer *wireFormat = ccnxWireFormatMessage_GetWireFormatBuffer(dictionary);
    bool success = ccnxCodecTlvPacket_BufferDecode(wireFormat, dictionary);

    if (success) {
        if (rtaComponent_PutMessage(out, tm)) {
            rtaComponentStats_Increment(stats, STATS_UPCALL_OUT);
        }
    } else {
        printf("Decoding error!");
        parcBuffer_Display(wireFormat, 3);
    }
}

/* Read from below and send to above */
static void
component_Codec_Tlv_Upcall_Read(PARCEventQueue *in, PARCEventType event, void *ptr)
{
    RtaProtocolStack *stack = (RtaProtocolStack *) ptr;
    PARCEventQueue *out = rtaProtocolStack_GetPutQueue(stack, CODEC_TLV, RTA_UP);
    TransportMessage *tm;

    while ((tm = rtaComponent_GetMessage(in)) != NULL) {
        RtaConnection  *conn = rtaConnection_GetFromTransport(tm);
        RtaComponentStats *stats = rtaConnection_GetStats(conn, CODEC_TLV);
        rtaComponentStats_Increment(stats, STATS_UPCALL_IN);

        if (transportMessage_IsControl(tm)) {
            if (rtaComponent_PutMessage(out, tm)) {
                rtaComponentStats_Increment(stats, STATS_UPCALL_OUT);
            }
        } else {
            upcallDictionary(tm, out, stats);
        }

        if (DEBUG_OUTPUT) {
            struct timeval delay = transportMessage_GetDelay(tm);
            printf("%9" PRIu64 " %s total upcall reads in %" PRIu64 " out %" PRIu64 " last delay %.6f\n",
                   rtaFramework_GetTicks(rtaProtocolStack_GetFramework(rtaConnection_GetStack(conn))),
                   __func__,
                   rtaComponentStats_Get(stats, STATS_UPCALL_IN),
                   rtaComponentStats_Get(stats, STATS_UPCALL_OUT),
                   delay.tv_sec + delay.tv_usec * 1E-6);
        }
    }
}


static TransportMessage *
component_Codec_Tlv_EncodeDictionary_SchemaV1(TransportMessage *tm, RtaConnection  *conn, CCNxTlvDictionary *packetDictionary)
{
    bool hasWireFormat = (ccnxTlvDictionary_IsValueIoVec(packetDictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_WireFormat) ||
                          ccnxTlvDictionary_IsValueBuffer(packetDictionary, CCNxCodecSchemaV1TlvDictionary_HeadersFastArray_WireFormat));

    if (!hasWireFormat) {
        CodecConnectionState *codec_conn_state = rtaConnection_GetPrivateData(conn, CODEC_TLV);
        assertNotNull(codec_conn_state, "%s got null private data\n", __func__);

        CCNxCodecNetworkBufferIoVec *vec = ccnxCodecSchemaV1PacketEncoder_DictionaryEncode(packetDictionary, codec_conn_state->signer);

        if (vec) {
            // store a reference back into the dictioary
            bool success = ccnxWireFormatMessage_PutIoVec(packetDictionary, vec);
            assertTrue(success, "Failed to save wire format in the dictionary") {
                ccnxCodecNetworkBufferIoVec_Display(vec, 0);
            }

            if (DEBUG_OUTPUT > 2) {
                printf("%s encoded packet:\n", __func__);
                ccnxCodecNetworkBufferIoVec_Display(vec, 0);
            }

            ccnxCodecNetworkBufferIoVec_Release(&vec);

        } else {
            trapUnexpectedState("Error encoding packet") {
                ccnxTlvDictionary_Display(packetDictionary, 0);
            }
        }
    } else {
        if (DEBUG_OUTPUT) {
            printf("%9" PRIu64 " %s packetDictionary %p already has wire format\n",
                   rtaFramework_GetTicks(rtaProtocolStack_GetFramework(rtaConnection_GetStack(conn))),
                   __func__,
                   (void *) packetDictionary);
        }
    }

    if (tm && DEBUG_OUTPUT > 2) {
        CCNxCodecNetworkBufferIoVec *vec = ccnxWireFormatMessage_GetIoVec(packetDictionary);
        printf("%9" PRIu64 " %s packetDictionary %p wire format dump\n",
               rtaFramework_GetTicks(rtaProtocolStack_GetFramework(rtaConnection_GetStack(conn))),
               __func__,
               (void *) packetDictionary);
        ccnxCodecNetworkBufferIoVec_Display(vec, 3);
    }

    return tm;
}


static TransportMessage *
component_Codec_Tlv_EncodeDictionary(TransportMessage *tm, RtaConnection  *conn)
{
    // If the dictionary already contains a wireformat, we use that and skip encoding
    CCNxTlvDictionary *packetDictionary = transportMessage_GetDictionary(tm);

    assertNotNull(packetDictionary, "Got a NULL packet dictionary for dictionary based encoding");
    if (packetDictionary) {
        switch (ccnxTlvDictionary_GetSchemaVersion(packetDictionary)) {

            case CCNxTlvDictionary_SchemaVersion_V1:
                return component_Codec_Tlv_EncodeDictionary_SchemaV1(tm, conn, packetDictionary);
                break;

            default:
                trapIllegalValue(packetDictionary, "Unknown schema version: %d", ccnxTlvDictionary_GetSchemaVersion(packetDictionary));
        }
    }
    return NULL;
}

/* Read from above and send to below */
static void
component_Codec_Tlv_Downcall_Read(PARCEventQueue *in, PARCEventType event, void *ptr)
{
    RtaProtocolStack *stack = (RtaProtocolStack *) ptr;
    PARCEventQueue *out = rtaProtocolStack_GetPutQueue(stack, CODEC_TLV, RTA_DOWN);
    TransportMessage *tm;


    while ((tm = rtaComponent_GetMessage(in)) != NULL) {
        RtaConnection  *conn = rtaConnection_GetFromTransport(tm);
        RtaComponentStats *stats = rtaConnection_GetStats(conn, CODEC_TLV);
        rtaComponentStats_Increment(stats, STATS_DOWNCALL_IN);

        // this will encode everything, including control messages
        TransportMessage *encoded = component_Codec_Tlv_EncodeDictionary(tm, conn);

        if (encoded) {
            if (rtaComponent_PutMessage(out, encoded)) {
                rtaComponentStats_Increment(stats, STATS_DOWNCALL_OUT);
            }
        } else {
            tm = NULL;
        }

        if (DEBUG_OUTPUT && tm) {
            struct timeval delay = transportMessage_GetDelay(tm);
            printf("%9" PRIu64 " %s total downcall reads in %" PRIu64 " out %" PRIu64 " last delay %.6f\n",
                   rtaFramework_GetTicks(rtaProtocolStack_GetFramework(rtaConnection_GetStack(conn))),
                   __func__,
                   rtaComponentStats_Get(stats, STATS_DOWNCALL_IN),
                   rtaComponentStats_Get(stats, STATS_DOWNCALL_OUT),
                   delay.tv_sec + delay.tv_usec * 1E-6);
        }
    }
}

static int
component_Codec_Tlv_Closer(RtaConnection *conn)
{
    struct codec_connection_state *codec_conn_state;

    codec_conn_state = rtaConnection_GetPrivateData(conn, CODEC_TLV);
    assertNotNull(codec_conn_state, "%s got null private data\n", __func__);

    if (DEBUG_OUTPUT) {
        printf("%9" PRIu64 " %s connection %u codec signer %p private %p\n",
               rtaFramework_GetTicks(rtaProtocolStack_GetFramework(rtaConnection_GetStack(conn))),
               __func__,
               rtaConnection_GetConnectionId(conn),
               (void *) codec_conn_state->signer,
               (void *) codec_conn_state);
    }

    parcSigner_Release(&codec_conn_state->signer);

    parcMemory_Deallocate((void **) &codec_conn_state);

    return 0;
}

static int
component_Codec_Tlv_Release(RtaProtocolStack *stack)
{
    // no ProtocolStack wide state
    return 0;
}

static void
component_Codec_Tlv_StateChange(RtaConnection *conn)
{
    struct codec_connection_state *codec_conn_state;

    codec_conn_state = rtaConnection_GetPrivateData(conn, CODEC_TLV);
    assertNotNull(codec_conn_state, "%s got null private data\n", __func__);

    if (DEBUG_OUTPUT) {
        printf("%s connection %p codec signer %p private %p\n",
               __func__,
               (void *) conn,
               (void *) codec_conn_state->signer,
               (void *) codec_conn_state);
    }

    // unimplemented code (case 878)
}

// ==================

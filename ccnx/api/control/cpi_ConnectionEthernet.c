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
 * This was written with PARCJSON, but that class is not really ready for prime time yet,
 * so there are some memory leaks in it that make determining if we have any memory leaks
 * difficult.  So, there may be some memory leaks... case 1029
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>

#include <LongBow/runtime.h>

#include <ccnx/api/control/cpi_ConnectionEthernet.h>
#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_JSON.h>

#include <ccnx/api/control/controlPlaneInterface.h>
extern uint64_t cpi_GetNextSequenceNumber(void);

// JSON keys
static const char *KEY_IFNAME = "IFNAME";
static const char *KEY_ADDR = "PEER_ADDR";
static const char *KEY_ETHERTYPE = "ETHERTYPE";
static const char *KEY_SYMBOLIC = "SYMBOLIC";

static const char *KEY_ADDETHER = "AddConnEther";
static const char *KEY_REMOVEETHER = "RemoveConnEther";

struct cpi_connection_ethernet {
    char *interfaceName;
    char *symbolic;
    CPIAddress *peerLinkAddress;
    uint16_t ethertype;
};

CPIConnectionEthernet *
cpiConnectionEthernet_Create(const char *interfaceName, CPIAddress *peerLinkAddress, uint16_t ethertype, const char *symbolic)
{
    assertNotNull(interfaceName, "Parameter interfaceName must be non-null");
    assertNotNull(peerLinkAddress, "Parameter peerLinkAddress must be non-null");

    CPIConnectionEthernet *etherConn = parcMemory_AllocateAndClear(sizeof(CPIConnectionEthernet));
    if (etherConn) {
        etherConn->interfaceName = parcMemory_StringDuplicate(interfaceName, strlen(interfaceName));
        etherConn->symbolic = parcMemory_StringDuplicate(symbolic, strlen(symbolic));
        etherConn->peerLinkAddress = cpiAddress_Copy(peerLinkAddress);
        etherConn->ethertype = ethertype;
    }

    return etherConn;
}

void
cpiConnectionEthernet_Release(CPIConnectionEthernet **etherConnPtr)
{
    assertNotNull(etherConnPtr, "Parameter etherConnPtr must be non-null double pointer");
    assertNotNull(*etherConnPtr, "Parameter etherConnPtr dereference to non-null pointer");

    CPIConnectionEthernet *etherConn = *etherConnPtr;
    cpiAddress_Destroy(&etherConn->peerLinkAddress);
    parcMemory_Deallocate((void **) &(etherConn->interfaceName));
    parcMemory_Deallocate((void **) &(etherConn->symbolic));
    parcMemory_Deallocate((void **) &etherConn);
    *etherConnPtr = NULL;
}

bool
cpiConnectionEthernet_Equals(const CPIConnectionEthernet *a, const CPIConnectionEthernet *b)
{
    if ((a == NULL && b == NULL) || a == b) {
        // both null or identically equal
        return true;
    }

    if (a == NULL || b == NULL) {
        // only one is null
        return false;
    }

    if (a->ethertype == b->ethertype) {
        if (cpiAddress_Equals(a->peerLinkAddress, b->peerLinkAddress)) {
            if (strcmp(a->interfaceName, b->interfaceName) == 0) {
                if (strcmp(a->symbolic, b->symbolic) == 0) {
                    return true;
                }
            }
        }
    }
    return false;
}


static PARCJSON *
_cpiConnectionEthernet_ToJson(const CPIConnectionEthernet *etherConn)
{
    PARCJSON *json = parcJSON_Create();

    // ------ Interface Name
    parcJSON_AddString(json, KEY_IFNAME, etherConn->interfaceName);

    // ------ Symbolic Name
    parcJSON_AddString(json, KEY_SYMBOLIC, etherConn->symbolic);

    // ------ Link Address
    PARCJSON *peerLinkJson = cpiAddress_ToJson(etherConn->peerLinkAddress);
    parcJSON_AddObject(json, KEY_ADDR, peerLinkJson);
    parcJSON_Release(&peerLinkJson);

    // ------ EtherType
    parcJSON_AddInteger(json, KEY_ETHERTYPE, etherConn->ethertype);

    return json;
}

/*
 * We want to create a JSON object that looks like this
 *  {
 *     "CPI_REQUEST" :
 *        {  "SEQUENCE" : <sequence number>,
 *           <operationName> : { "IFNAME" : "em1", "SYMBOLIC" : "conn0", "PEER_ADDR" : { "ADDRESSTYPE" : "LINK", "DATA" : "AQIDBAUG" }, "ETHERTYPE" : 2049 },
 *        }
 *  }
 */
static CCNxControl *
_cpiConnectionEthernet_CreateControlMessage(const CPIConnectionEthernet *etherConn, const char *operationName)
{
    // Figure out how to refactor this, case 1030

    PARCJSON *cpiRequest = parcJSON_Create();

    // --- add the seqnum

    uint64_t seqnum = cpi_GetNextSequenceNumber();
    parcJSON_AddInteger(cpiRequest, "SEQUENCE", (int) seqnum);

    // -- Add the operation
    PARCJSON *operation = _cpiConnectionEthernet_ToJson(etherConn);
    parcJSON_AddObject(cpiRequest, operationName, operation);
    parcJSON_Release(&operation);

    // -- Do the final encapusulation
    PARCJSON *final = parcJSON_Create();
    parcJSON_AddObject(final, cpiRequest_GetJsonTag(), cpiRequest);
    parcJSON_Release(&cpiRequest);

    // -- Create the CPIControlMessage
    char *finalString = parcJSON_ToCompactString(final);

    parcJSON_Release(&final);

    PARCJSON *oldJson = parcJSON_ParseString(finalString);
    CCNxControl *result = ccnxControl_CreateCPIRequest(oldJson);
    parcJSON_Release(&oldJson);

    parcMemory_Deallocate((void **) &finalString);

    return result;
}

CCNxControl *
cpiConnectionEthernet_CreateAddMessage(const CPIConnectionEthernet *etherConn)
{
    assertNotNull(etherConn, "Parameter etherConn must be non-null");
    CCNxControl *control = _cpiConnectionEthernet_CreateControlMessage(etherConn, KEY_ADDETHER);
    return control;
}

CCNxControl *
cpiConnectionEthernet_CreateRemoveMessage(const CPIConnectionEthernet *etherConn)
{
    assertNotNull(etherConn, "Parameter etherConn must be non-null");
    CCNxControl *control = _cpiConnectionEthernet_CreateControlMessage(etherConn, KEY_REMOVEETHER);
    return control;
}

static bool
_cpiConnectionEthernet_IsMessageType(const CCNxControl *control, const char *operationName)
{
    bool isOperation = false;
    if (ccnxControl_IsCPI(control)) {
        PARCJSON *oldJson = ccnxControl_GetJson(control);
        PARCJSONValue *value = parcJSON_GetValueByName(oldJson, cpiRequest_GetJsonTag());

        if (value != NULL) {
            PARCJSON *innerJson = parcJSONValue_GetJSON(value);
            // the second array element is the key we're looking for
            PARCJSONPair *pair = parcJSON_GetPairByIndex(innerJson, 1);
            if (pair != NULL) {
                const char *opKey = parcBuffer_Overlay(parcJSONPair_GetName(pair), 0);
                if (opKey && strcasecmp(opKey, operationName) == 0) {
                    isOperation = true;
                }
            }
        }
    }

    return isOperation;
}

bool
cpiConnectionEthernet_IsAddMessage(const CCNxControl *control)
{
    assertNotNull(control, "Parameter control must be non-null");
    return _cpiConnectionEthernet_IsMessageType(control, KEY_ADDETHER);
}

bool
cpiConnectionEthernet_IsRemoveMessage(const CCNxControl *control)
{
    assertNotNull(control, "Parameter control must be non-null");
    return _cpiConnectionEthernet_IsMessageType(control, KEY_REMOVEETHER);
}

CPIConnectionEthernet *
cpiConnectionEthernet_FromControl(const CCNxControl *control)
{
    assertNotNull(control, "Parameter control must be non-null");

    CPIConnectionEthernet *etherConn = NULL;

    if (ccnxControl_IsCPI(control)) {
        PARCJSON *oldJson = ccnxControl_GetJson(control);
        PARCJSONValue *value = parcJSON_GetValueByName(oldJson, cpiRequest_GetJsonTag());

        if (value != NULL) {
            assertTrue(parcJSONValue_IsJSON(value),
                       "Wrong JSON type for %s, expected JSON: %s",
                       cpiRequest_GetJsonTag(), parcJSON_ToString(oldJson));
            PARCJSON *requestJson = parcJSONValue_GetJSON(value);
            // the second array element is the key we're looking for
            PARCJSONPair *pair = parcJSON_GetPairByIndex(requestJson, 1);
            const char *opKey = parcBuffer_Overlay(parcJSONPair_GetName(pair), 0);
            if (opKey && ((strcasecmp(opKey, KEY_ADDETHER) == 0) || strcasecmp(opKey, KEY_REMOVEETHER))) {
                PARCJSON *opJson = parcJSONValue_GetJSON(parcJSONPair_GetValue(pair));

                // Ok, it is one of our messages, now assemble the pieces
                value = parcJSON_GetValueByName(opJson, KEY_IFNAME);
                PARCBuffer *sBuf = parcJSONValue_GetString(value);
                const char *ifname = parcBuffer_Overlay(sBuf, 0);
                value = parcJSON_GetValueByName(opJson, KEY_SYMBOLIC);
                sBuf = parcJSONValue_GetString(value);
                const char *symbolic = parcBuffer_Overlay(sBuf, 0);
                value = parcJSON_GetValueByName(opJson, KEY_ETHERTYPE);
                int ethertype = (int)parcJSONValue_GetInteger(value);
                value = parcJSON_GetValueByName(opJson, KEY_ADDR);
                PARCJSON *addrJson = parcJSONValue_GetJSON(value);
                assertNotNull(addrJson, "JSON missing the key %s", KEY_ADDR);

                CPIAddress *peerAddress = cpiAddress_CreateFromJson(addrJson);
                assertNotNull(peerAddress, "Failed to decode the peer address from %s", parcJSON_ToString(addrJson));

                etherConn = cpiConnectionEthernet_Create(ifname, peerAddress, (uint16_t) ethertype, symbolic);

                cpiAddress_Destroy(&peerAddress);
            }
        }
    }

    return etherConn;
}

const char *
cpiConnectionEthernet_GetInterfaceName(const CPIConnectionEthernet *etherConn)
{
    assertNotNull(etherConn, "Parameter etherConn must be non-null");
    return etherConn->interfaceName;
}

const char *
cpiConnectionEthernet_GetSymbolicName(const CPIConnectionEthernet *etherConn)
{
    assertNotNull(etherConn, "Parameter etherConn must be non-null");
    return etherConn->symbolic;
}

CPIAddress *
cpiConnectionEthernet_GetPeerLinkAddress(const CPIConnectionEthernet *etherConn)
{
    assertNotNull(etherConn, "Parameter etherConn must be non-null");
    return etherConn->peerLinkAddress;
}

uint16_t
cpiConnectionEthernet_GetEthertype(const CPIConnectionEthernet *etherConn)
{
    assertNotNull(etherConn, "Parameter etherConn must be non-null");
    return etherConn->ethertype;
}

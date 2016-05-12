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

#include <LongBow/runtime.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Object.h>
#include <parc/algol/parc_JSON.h>

#include <ccnx/api/control/cpi_InterfaceIPTunnel.h>
#include <ccnx/api/control/cpi_InterfaceGeneric.h>

#define SOURCE_INDEX 0
#define DESTINATION_INDEX 1

const static char cpiIFIDX[] = "IFIDX";
const static char cpiSRCADDR[] = "SRC";
const static char cpiDSTADDR[] = "DST";
const static char cpiTUNTYPE[] = "TUNTYPE";
const static char cpiSTATE[] = "STATE";
const static char cpiSYMBOLIC[] = "SYMBOLIC";

struct cpi_interface_iptun {
    CPIInterfaceGeneric *generic;
    CPIInterfaceIPTunnelType tunnelType;
    char *symbolic;
};

struct iptunnel_type_string_s {
    CPIInterfaceIPTunnelType type;
    const char *str;
} iptunnelTypeStrings[] = {
    { .type = IPTUN_UDP, .str = "UDP" },
    { .type = IPTUN_TCP, .str = "TCP" },
    { .type = IPTUN_GRE, .str = "GRE" },
    { .type = 0,         .str = NULL  },
};


static void
_cpiInterfaceIPTunnel_Destroy(CPIInterfaceIPTunnel **iptunPtr)
{
    assertNotNull(iptunPtr, "Parameter must be non-null double pointer");
    assertNotNull(*iptunPtr, "Parameter must dereference to non-null pointer");

    CPIInterfaceIPTunnel *iptun = *iptunPtr;
    cpiInterfaceGeneric_Destroy(&iptun->generic);
    parcMemory_Deallocate((void **) &iptun->symbolic);
}

parcObject_ExtendPARCObject(CPIInterfaceIPTunnel, _cpiInterfaceIPTunnel_Destroy, cpiInterfaceIPTunnel_Copy, NULL, cpiInterfaceIPTunnel_Equals, NULL, NULL, cpiInterfaceIPTunnel_ToJson);

parcObject_ImplementRelease(cpiInterfaceIPTunnel, CPIInterfaceIPTunnel);

parcObject_ImplementAcquire(cpiInterfaceIPTunnel, CPIInterfaceIPTunnel);

const char *
cpiInterfaceIPTunnel_TypeToString(CPIInterfaceIPTunnelType type)
{
    for (int i = 0; iptunnelTypeStrings[i].str != NULL; i++) {
        if (iptunnelTypeStrings[i].type == type) {
            return iptunnelTypeStrings[i].str;
        }
    }
    assertTrue(0, "Unknown type: %d", type);
    abort();
}

CPIInterfaceIPTunnelType
cpiInterfaceIPTunnel_TypeFromString(const char *str)
{
    for (int i = 0; iptunnelTypeStrings[i].str != NULL; i++) {
        if (strcasecmp(iptunnelTypeStrings[i].str, str) == 0) {
            return iptunnelTypeStrings[i].type;
        }
    }
    assertTrue(0, "Unknown stirng: %s", str);
    abort();
}

CPIInterfaceIPTunnel *
cpiInterfaceIPTunnel_Create(unsigned ifidx, CPIAddress *source, CPIAddress *destination, CPIInterfaceIPTunnelType tunnelType, const char *symbolic)
{
    assertNotNull(source, "Parameter source must be non-null");
    assertNotNull(destination, "Parameter destination must be non-null");

    assertTrue(cpiAddress_GetType(source) == cpiAddressType_INET || cpiAddress_GetType(source) == cpiAddressType_INET6,
               "source address unsupported type: %d",
               cpiAddress_GetType(source));

    assertTrue(cpiAddress_GetType(destination) == cpiAddressType_INET || cpiAddress_GetType(destination) == cpiAddressType_INET6,
               "destination address unsupported type: %d",
               cpiAddress_GetType(destination));

    CPIInterfaceIPTunnel *iptun = parcObject_CreateInstance(CPIInterfaceIPTunnel);
    assertNotNull(iptun, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(CPIInterfaceIPTunnel));

    CPIAddressList *addrlist = cpiAddressList_Create();
    cpiAddressList_Append(addrlist, source);
    cpiAddressList_Append(addrlist, destination);

    iptun->generic = cpiInterfaceGeneric_Create(ifidx, addrlist);
    iptun->tunnelType = tunnelType;
    iptun->symbolic = parcMemory_StringDuplicate(symbolic, strlen(symbolic));

    return iptun;
}

CPIInterfaceIPTunnel *
cpiInterfaceIPTunnel_Copy(const CPIInterfaceIPTunnel *original)
{
    assertNotNull(original, "Parameter original must be non-null");
    CPIInterfaceIPTunnel *iptun = parcObject_CreateInstance(CPIInterfaceIPTunnel);
    assertNotNull(iptun, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(CPIInterfaceIPTunnel));
    iptun->generic = cpiInterfaceGeneric_Copy(original->generic);
    iptun->tunnelType = original->tunnelType;
    iptun->symbolic = parcMemory_StringDuplicate(original->symbolic, strlen(original->symbolic));
    return iptun;
}

void
cpiInterfaceIPTunnel_SetState(CPIInterfaceIPTunnel *iptun, CPIInterfaceStateType state)
{
    assertNotNull(iptun, "Parameter must be non-null");
    cpiInterfaceGeneric_SetState(iptun->generic, state);
}

const char *
cpiInterfaceIPTunnel_GetSymbolicName(const CPIInterfaceIPTunnel *iptun)
{
    assertNotNull(iptun, "Parameter must be non-null");
    return iptun->symbolic;
}

unsigned
cpiInterfaceIPTunnel_GetIndex(const CPIInterfaceIPTunnel *iptun)
{
    assertNotNull(iptun, "Parameter must be non-null");
    return cpiInterfaceGeneric_GetIndex(iptun->generic);
}

const CPIAddress *
cpiInterfaceIPTunnel_GetSourceAddress(const CPIInterfaceIPTunnel *iptun)
{
    assertNotNull(iptun, "Parameter must be non-null");
    const CPIAddressList *addrs = cpiInterfaceGeneric_GetAddresses(iptun->generic);
    return cpiAddressList_GetItem(addrs, SOURCE_INDEX);
}

const CPIAddress *
cpiInterfaceIPTunnel_GetDestinationAddress(const CPIInterfaceIPTunnel *iptun)
{
    assertNotNull(iptun, "Parameter must be non-null");
    const CPIAddressList *addrs = cpiInterfaceGeneric_GetAddresses(iptun->generic);
    return cpiAddressList_GetItem(addrs, DESTINATION_INDEX);
}

CPIInterfaceIPTunnelType
cpiInterfaceIPTunnel_GetTunnelType(const CPIInterfaceIPTunnel *iptun)
{
    assertNotNull(iptun, "Parameter must be non-null");
    return iptun->tunnelType;
}

CPIInterfaceStateType
cpiInterfaceIPTunnel_GetState(const CPIInterfaceIPTunnel *iptun)
{
    assertNotNull(iptun, "Parameter must be non-null");
    return cpiInterfaceGeneric_GetState(iptun->generic);
}

bool
cpiInterfaceIPTunnel_Equals(const CPIInterfaceIPTunnel *a, const CPIInterfaceIPTunnel *b)
{
    assertNotNull(a, "Parameter a must be non-null");
    assertNotNull(b, "Parameter b must be non-null");

    if (a->tunnelType == b->tunnelType) {
        if (cpiInterfaceGeneric_Equals(a->generic, b->generic)) {
            if (strcasecmp(a->symbolic, b->symbolic) == 0) {
                return true;
            }
        }
    }
    return false;
}

/**
 * JSON representation
 *
 * <code>
 * { "TUNNEL" :
 * { "IFIDX" : ifidx,
 *  "SYMBOLIC" : "tun3",
 * ["STATE" : "UP" | "DOWN", ]
 * "TYPE": "UDP" | "TCP" | "GRE",
 * "SRC" : {srcaddr},
 * "DST" : {dstaddr}
 * }
 * }
 * </code>
 *
 * @param <#param1#>
 * @return <#return#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
PARCJSON *
cpiInterfaceIPTunnel_ToJson(const CPIInterfaceIPTunnel *iptun)
{
    assertNotNull(iptun, "Parameter must be non-null");

    PARCJSON *inner_json = parcJSON_Create();

    parcJSON_AddInteger(inner_json, cpiIFIDX, cpiInterfaceIPTunnel_GetIndex(iptun));
    parcJSON_AddString(inner_json, cpiSYMBOLIC, iptun->symbolic);

    if (cpiInterfaceIPTunnel_GetState(iptun) != CPI_IFACE_UNKNOWN) {
        parcJSON_AddString(inner_json, cpiSTATE, cpiInterfaceStateType_ToString(cpiInterfaceIPTunnel_GetState(iptun)));
    }
    parcJSON_AddString(inner_json, cpiTUNTYPE, cpiInterfaceIPTunnel_TypeToString(cpiInterfaceIPTunnel_GetTunnelType(iptun)));

    PARCJSON *json = cpiAddress_ToJson(cpiInterfaceIPTunnel_GetSourceAddress(iptun));
    parcJSON_AddObject(inner_json, cpiSRCADDR, json);
    parcJSON_Release(&json);

    json = cpiAddress_ToJson(cpiInterfaceIPTunnel_GetDestinationAddress(iptun));
    parcJSON_AddObject(inner_json, cpiDSTADDR, json);
    parcJSON_Release(&json);

    PARCJSON *outter_json = parcJSON_Create();
    parcJSON_AddObject(outter_json, cpiInterfaceType_ToString(CPI_IFACE_TUNNEL), inner_json);
    parcJSON_Release(&inner_json);

    return outter_json;
}

CPIInterfaceIPTunnel *
cpiInterfaceIPTunnel_CreateFromJson(PARCJSON *json)
{
    assertNotNull(json, "Parameter must be non-null");

    PARCJSONValue *value = parcJSON_GetValueByName(json, cpiInterfaceType_ToString(CPI_IFACE_TUNNEL));
    assertNotNull(value,
                  "JSON key not found %s: %s",
                  cpiInterfaceType_ToString(CPI_IFACE_TUNNEL),
                  parcJSON_ToString(json));
    PARCObject *tunnelJson = parcJSONValue_GetJSON(value);

    value = parcJSON_GetValueByName(tunnelJson, cpiIFIDX);
    assertNotNull(value, "Could not find key %s: %s", cpiIFIDX, parcJSON_ToString(json));
    assertTrue(parcJSONValue_IsNumber(value),
               "%s is not a number: %s",
               cpiIFIDX,
               parcJSON_ToString(json));
    PARCJSONValue *ifidx_value = value;

    value = parcJSON_GetValueByName(tunnelJson, cpiSYMBOLIC);
    assertNotNull(value, "Could not find key %s: %s", cpiSYMBOLIC, parcJSON_ToString(json));
    assertTrue(parcJSONValue_IsString(value),
               "%s is not a string: %s",
               cpiSYMBOLIC,
               parcJSON_ToString(json));
    PARCJSONValue *symbolic_value = value;

    value = parcJSON_GetValueByName(tunnelJson, cpiTUNTYPE);
    assertNotNull(value, "Could not find key %s: %s", cpiTUNTYPE, parcJSON_ToString(json));
    assertTrue(parcJSONValue_IsString(value),
               "%s is not a number: %s",
               cpiTUNTYPE,
               parcJSON_ToString(json));
    PARCJSONValue *tuntype_value = value;

    value = parcJSON_GetValueByName(tunnelJson, cpiSRCADDR);
    assertNotNull(value, "Could not find key %s: %s", cpiSRCADDR, parcJSON_ToString(json));
    assertTrue(parcJSONValue_IsJSON(value),
               "%s is not an array: %s",
               cpiSRCADDR,
               parcJSON_ToString(json));
    PARCJSONValue *srcaddr_value = value;

    value = parcJSON_GetValueByName(tunnelJson, cpiDSTADDR);
    assertNotNull(value, "Could not find key %s: %s", cpiDSTADDR, parcJSON_ToString(json));
    assertTrue(parcJSONValue_IsJSON(value),
               "%s is not an array: %s",
               cpiDSTADDR,
               parcJSON_ToString(json));
    PARCJSONValue *dstaddr_value = value;

    unsigned ifidx = (unsigned) parcJSONValue_GetInteger(ifidx_value);
    PARCBuffer *sBuf = parcJSONValue_GetString(symbolic_value);
    const char *symbolic = parcBuffer_Overlay(sBuf, 0);
    CPIAddress *srcaddr =
        cpiAddress_CreateFromJson(parcJSONValue_GetJSON(srcaddr_value));
    CPIAddress *dstaddr =
        cpiAddress_CreateFromJson(parcJSONValue_GetJSON(dstaddr_value));
    sBuf = parcJSONValue_GetString(tuntype_value);
    CPIInterfaceIPTunnelType tunnelType =
        cpiInterfaceIPTunnel_TypeFromString(parcBuffer_Overlay(sBuf, 0));

    CPIInterfaceIPTunnel *iptun =
        cpiInterfaceIPTunnel_Create(ifidx, srcaddr, dstaddr, tunnelType, symbolic);

    PARCJSONValue *state_value = parcJSON_GetValueByName(tunnelJson, cpiSTATE);
    if (state_value != NULL) {
        sBuf = parcJSONValue_GetString(state_value);
        cpiInterfaceIPTunnel_SetState(iptun, cpiInterfaceStateType_FromString(parcBuffer_Overlay(sBuf, 0)));
    }

    return iptun;
}

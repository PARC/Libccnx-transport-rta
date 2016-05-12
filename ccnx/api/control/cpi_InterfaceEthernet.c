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

#include <ccnx/api/control/cpi_InterfaceEthernet.h>
#include <ccnx/api/control/cpi_InterfaceGeneric.h>
#include <LongBow/runtime.h>
#include <parc/algol/parc_Memory.h>
#include <string.h>

const static char cpiIFIDX[] = "IFIDX";
const static char cpiADDRS[] = "ADDRS";
const static char cpiSTATE[] = "STATE";

static const char *cpiAddEtherConnection = "AddConnEther";

struct cpi_interface_ethernet {
    CPIInterfaceGeneric *generic;
};

CPIInterfaceEthernet *
cpiInterfaceEthernet_Create(unsigned ifidx, CPIAddressList *addresses)
{
    assertNotNull(addresses, "Parameter addresses must be non-null");

    CPIInterfaceEthernet *ethernet = parcMemory_AllocateAndClear(sizeof(CPIInterfaceEthernet));
    assertNotNull(ethernet, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(CPIInterfaceEthernet));
    ethernet->generic = cpiInterfaceGeneric_Create(ifidx, addresses);
    return ethernet;
}

CPIInterfaceEthernet *
cpiInterfaceEthernet_Copy(const CPIInterfaceEthernet *original)
{
    assertNotNull(original, "Parameter original must be non-null");

    CPIInterfaceEthernet *ethernet = parcMemory_AllocateAndClear(sizeof(CPIInterfaceEthernet));
    assertNotNull(ethernet, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(CPIInterfaceEthernet));
    ethernet->generic = cpiInterfaceGeneric_Copy(original->generic);
    return ethernet;
}

void
cpiInterfaceEthernet_Destroy(CPIInterfaceEthernet **ethernetPtr)
{
    assertNotNull(ethernetPtr, "Parameter must be non-null double pointer");
    assertNotNull(*ethernetPtr, "Parameter must dereference to non-null pointer");

    CPIInterfaceEthernet *ethernet = *ethernetPtr;
    cpiInterfaceGeneric_Destroy(&ethernet->generic);
    parcMemory_Deallocate((void **) &ethernet);
    *ethernetPtr = NULL;
}

void
cpiInterfaceEthernet_SetState(CPIInterfaceEthernet *ethernet, CPIInterfaceStateType state)
{
    assertNotNull(ethernet, "Parameter must be non-null pointer");
    cpiInterfaceGeneric_SetState(ethernet->generic, state);
}

unsigned
cpiInterfaceEthernet_GetIndex(const CPIInterfaceEthernet *ethernet)
{
    assertNotNull(ethernet, "Parameter must be non-null pointer");
    return cpiInterfaceGeneric_GetIndex(ethernet->generic);
}

const CPIAddressList *
cpiInterfaceEthernet_GetAddresses(const CPIInterfaceEthernet *ethernet)
{
    assertNotNull(ethernet, "Parameter must be non-null pointer");
    return cpiInterfaceGeneric_GetAddresses(ethernet->generic);
}

CPIInterfaceStateType
cpiInterfaceEthernet_GetState(const CPIInterfaceEthernet *ethernet)
{
    assertNotNull(ethernet, "Parameter must be non-null pointer");
    return cpiInterfaceGeneric_GetState(ethernet->generic);
}

PARCJSON *
cpiInterfaceEthernet_ToJson(const CPIInterfaceEthernet *ethernet)
{
    assertNotNull(ethernet, "Parameter must be non-null");

    PARCJSON *innerJson = parcJSON_Create();

    parcJSON_AddInteger(innerJson, cpiIFIDX, cpiInterfaceEthernet_GetIndex(ethernet));

    if (cpiInterfaceEthernet_GetState(ethernet) != CPI_IFACE_UNKNOWN) {
        parcJSON_AddString(innerJson,
                           cpiSTATE,
                           cpiInterfaceStateType_ToString(cpiInterfaceEthernet_GetState(ethernet)));
    }

    PARCJSONArray *addrsArray = cpiAddressList_ToJson(cpiInterfaceEthernet_GetAddresses(ethernet));
    parcJSON_AddArray(innerJson, cpiADDRS, addrsArray);
    parcJSONArray_Release(&addrsArray);

    PARCJSON *result = parcJSON_Create();
    parcJSON_AddObject(result, cpiInterfaceType_ToString(CPI_IFACE_ETHERNET), innerJson);
    parcJSON_Release(&innerJson);

    return result;
}

CPIInterfaceEthernet *
cpiInterfaceEthernet_CreateFromJson(PARCJSON *json)
{
    assertNotNull(json, "Parameter must be non-null");

    PARCJSONValue *value = parcJSON_GetValueByName(json, cpiInterfaceType_ToString(CPI_IFACE_ETHERNET));
    assertNotNull(value,
                  "JSON key not found %s: %s",
                  cpiInterfaceType_ToString(CPI_IFACE_ETHERNET),
                  parcJSON_ToString(json));
    PARCJSON *etherJson = parcJSONValue_GetJSON(value);

    value = parcJSON_GetValueByName(etherJson, cpiIFIDX);
    assertNotNull(value,
                  "JSON key not found %s: %s",
                  cpiIFIDX,
                  parcJSON_ToString(json));
    assertTrue(parcJSONValue_IsNumber(value),
               "%s is not a number: %s",
               cpiIFIDX,
               parcJSON_ToString(json));
    unsigned ifidx = (unsigned)parcJSONValue_GetInteger(value);
   
    value = parcJSON_GetValueByName(etherJson, cpiADDRS);
    assertNotNull(value,
                  "JSON key not found %s: %s",
                  cpiADDRS,
                  parcJSON_ToString(json));
    assertTrue(parcJSONValue_IsArray(value),
               "%s is not a number: %s",
               cpiADDRS,
               parcJSON_ToString(json));
    PARCJSONArray *addrsJson = parcJSONValue_GetArray(value);

    CPIAddressList *addrs = cpiAddressList_CreateFromJson(addrsJson);
    CPIInterfaceEthernet *ethernet = cpiInterfaceEthernet_Create(ifidx, addrs);

    value = parcJSON_GetValueByName(etherJson, cpiSTATE);
    if (value != NULL) {
        PARCBuffer *sBuf = parcJSONValue_GetString(value);
        char *state = parcBuffer_Overlay(sBuf, 0);
        cpiInterfaceEthernet_SetState(ethernet, cpiInterfaceStateType_FromString(state));
    }

    return ethernet;
}

bool
cpiInterfaceEthernet_Equals(const CPIInterfaceEthernet *a, const CPIInterfaceEthernet *b)
{
    assertNotNull(a, "Parameter a must be non-null");
    assertNotNull(b, "Parameter b must be non-null");

    return cpiInterfaceGeneric_Equals(a->generic, b->generic);
}

const char *
cpiLinks_AddEtherConnectionJasonTag()
{
    return cpiAddEtherConnection;
}

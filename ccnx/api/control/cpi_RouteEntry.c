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
#include <stdlib.h>
#include <sys/time.h>

#include <ccnx/api/control/cpi_RouteEntry.h>
#include <ccnx/api/control/controlPlaneInterface.h>
#include <parc/algol/parc_Memory.h>

#include <limits.h>

#include <LongBow/runtime.h>

static const char *cpiPrefix = "PREFIX";
static const char *cpiInterface = "INTERFACE";
static const char *cpiFlags = "FLAGS";
static const char *cpiLifetime = "LIFETIME";
static const char *cpiNexthop = "NEXTHOP";
static const char *cpiProtocol = "PROTOCOL";
static const char *cpiRouteType = "ROUTETYPE";
static const char *cpiCost = "COST";
static const char *cpiSymbolic = "SYMBOLIC";

struct cpi_route_entry {
    bool hasInterfaceIndex;
    unsigned interfaceIndex;

    CCNxName *prefix;
    char *symbolic;
    CPIAddress *nexthop;
    CPINameRouteProtocolType routingProtocol;
    CPINameRouteType routeType;
    unsigned cost;

    bool hasLifetime;
    struct timeval lifetime;
};

void
cpiRouteEntry_Destroy(CPIRouteEntry **routeEntryPtr)
{
    assertNotNull(routeEntryPtr, "Parameter must be non-null double pointer");
    assertNotNull(*routeEntryPtr, "Parameter must dereference to non-null pointer");
    CPIRouteEntry *route = *routeEntryPtr;

    ccnxName_Release(&route->prefix);
    if (route->nexthop) {
        cpiAddress_Destroy(&route->nexthop);
    }

    if (route->symbolic) {
        parcMemory_Deallocate((void **) &route->symbolic);
    }

    parcMemory_Deallocate((void **) &route);
    *routeEntryPtr = NULL;
}

CPIRouteEntry *
cpiRouteEntry_CreateRouteToSelf(const CCNxName *prefix)
{
    void *optionalLifetime = NULL;
    void *nexthop = NULL;
    unsigned cost = 0;

    CPIRouteEntry *route = cpiRouteEntry_Create(ccnxName_Copy(prefix),
                                                CPI_CURRENT_INTERFACE,
                                                nexthop,
                                                cpiNameRouteProtocolType_LOCAL,
                                                cpiNameRouteType_LONGEST_MATCH,
                                                optionalLifetime,
                                                cost);
    return route;
}

char *
cpiRouteEntry_ToString(CPIRouteEntry *route)
{
    PARCBufferComposer *composer = parcBufferComposer_Create();

    parcBufferComposer_Format(composer, "%6d %9.9s %7.7s %u ",
                              route->interfaceIndex,
                              cpiNameRouteProtocolType_ToString(route->routingProtocol),
                              cpiNameRouteType_ToString(route->routeType),
                              route->cost);

    if (route->symbolic != NULL) {
        parcBufferComposer_PutString(composer, route->symbolic);
    } else {
        parcBufferComposer_PutChar(composer, '-');
    }

    if (route->nexthop != NULL) {
        cpiAddress_BuildString(cpiRouteEntry_GetNexthop(route), composer);
    } else {
        parcBufferComposer_PutChar(composer, '-');
    }

    if (route->hasLifetime) {
#if __APPLE__
        parcBufferComposer_Format(composer, " %ld.%06d ", route->lifetime.tv_sec, route->lifetime.tv_usec);
#else
        parcBufferComposer_Format(composer, " %ld.%06ld ", route->lifetime.tv_sec, route->lifetime.tv_usec);
#endif
    } else {
        parcBufferComposer_Format(composer, " %8.8s ", "infinite");
    }

    char *ccnxName = ccnxName_ToString(cpiRouteEntry_GetPrefix(route));
    parcBufferComposer_PutString(composer, ccnxName);

    parcMemory_Deallocate((void **) &ccnxName);

    PARCBuffer *tempBuffer = parcBufferComposer_ProduceBuffer(composer);
    char *result = parcBuffer_ToString(tempBuffer);
    parcBuffer_Release(&tempBuffer);

    parcBufferComposer_Release(&composer);
    return result;
}

CPIRouteEntry *
cpiRouteEntry_Create(CCNxName *prefix,
                     unsigned interfaceIndex,
                     const CPIAddress *optionalNexthop,
                     CPINameRouteProtocolType routingProtocol,
                     CPINameRouteType routeType,
                     const struct timeval *optionalLifetime,
                     unsigned cost)
{
    assertNotNull(prefix, "Parameter prefix must be non-null");

    CPIRouteEntry *route = parcMemory_AllocateAndClear(sizeof(CPIRouteEntry));
    assertNotNull(route, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(CPIRouteEntry));
    route->prefix = prefix;
    route->symbolic = NULL;
    route->interfaceIndex = interfaceIndex;
    route->hasInterfaceIndex = true;
    route->nexthop = optionalNexthop != NULL ? cpiAddress_Copy(optionalNexthop) : NULL;
    route->routingProtocol = routingProtocol;
    route->routeType = routeType;
    route->cost = cost;

    if (optionalLifetime) {
        route->hasLifetime = true;
        route->lifetime = *optionalLifetime;
    } else {
        route->hasLifetime = false;
        route->lifetime = (struct timeval) { .tv_sec = INT_MAX, .tv_usec = 0 };
    }

    return route;
}

CPIRouteEntry *
cpiRouteEntry_CreateSymbolic(CCNxName *prefix,
                             const char *symbolicName,
                             CPINameRouteProtocolType routingProtocol,
                             CPINameRouteType routeType,
                             const struct timeval *optionalLifetime,
                             unsigned cost)
{
    assertNotNull(prefix, "Parameter prefix must be non-null");

    CPIRouteEntry *route = parcMemory_AllocateAndClear(sizeof(CPIRouteEntry));
    assertNotNull(route, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(CPIRouteEntry));
    route->prefix = prefix;
    route->symbolic = parcMemory_StringDuplicate(symbolicName, strlen(symbolicName));
    route->interfaceIndex = UINT32_MAX;
    route->hasInterfaceIndex = false;
    route->nexthop = NULL;
    route->routingProtocol = routingProtocol;
    route->routeType = routeType;
    route->cost = cost;

    if (optionalLifetime) {
        route->hasLifetime = true;
        route->lifetime = *optionalLifetime;
    } else {
        route->hasLifetime = false;
        route->lifetime = (struct timeval) { .tv_sec = INT_MAX, .tv_usec = 0 };
    }

    return route;
}


CPIRouteEntry *
cpiRouteEntry_Copy(const CPIRouteEntry *original)
{
    assertNotNull(original, "Parameter a must be non-null");
    CPIRouteEntry *copy;

    if (original->hasInterfaceIndex) {
        copy = cpiRouteEntry_Create(
            ccnxName_Copy(original->prefix),
            original->interfaceIndex,
            (original->nexthop ? original->nexthop : NULL),
            original->routingProtocol,
            original->routeType,
            (original->hasLifetime ? &original->lifetime : NULL),
            original->cost);

        if (original->symbolic) {
            copy->symbolic = parcMemory_StringDuplicate(original->symbolic, strlen(original->symbolic));
        }
    } else {
        copy = cpiRouteEntry_CreateSymbolic(
            ccnxName_Copy(original->prefix),
            original->symbolic,
            original->routingProtocol,
            original->routeType,
            (original->hasLifetime ? &original->lifetime : NULL),
            original->cost);
    }
    return copy;
}

void
cpiRouteEntry_SetInterfaceIndex(CPIRouteEntry *route, unsigned interfaceIndex)
{
    assertNotNull(route, "Parameter a must be non-null");
    route->interfaceIndex = interfaceIndex;
    route->hasInterfaceIndex = true;
}


bool
cpiRouteEntry_Equals(const CPIRouteEntry *a, const CPIRouteEntry *b)
{
    assertNotNull(a, "Parameter a must be non-null");
    assertNotNull(b, "Parameter b must be non-null");
    if (a == b) {
        return true;
    }

    if (a->interfaceIndex == b->interfaceIndex) {
        if (a->routeType == b->routeType) {
            if (a->routingProtocol == b->routingProtocol) {
                if (a->cost == b->cost) {
                    if (ccnxName_Equals(a->prefix, b->prefix)) {
                        if (cpiAddress_Equals(a->nexthop, b->nexthop)) {
                            if (a->hasLifetime == b->hasLifetime) {
                                if (timercmp(&a->lifetime, &b->lifetime, ==)) {
                                    if (a->symbolic == b->symbolic || (a->symbolic != NULL && b->symbolic != NULL && strcasecmp(a->symbolic, b->symbolic) == 0)) {
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

const CCNxName *
cpiRouteEntry_GetPrefix(const CPIRouteEntry *route)
{
    assertNotNull(route, "Parameter must be non-null");
    return route->prefix;
}

unsigned
cpiRouteEntry_GetInterfaceIndex(const CPIRouteEntry *route)
{
    assertNotNull(route, "Parameter must be non-null");
    return route->interfaceIndex;
}

const CPIAddress *
cpiRouteEntry_GetNexthop(const CPIRouteEntry *route)
{
    assertNotNull(route, "Parameter must be non-null");
    return route->nexthop;
}

bool
cpiRouteEntry_HasLifetime(const CPIRouteEntry *route)
{
    assertNotNull(route, "Parameter must be non-null");
    return route->hasLifetime;
}

struct timeval
cpiRouteEntry_GetLifetime(const CPIRouteEntry *route)
{
    assertNotNull(route, "Parameter must be non-null");
    return route->lifetime;
}

CPINameRouteProtocolType
cpiRouteEntry_GetRouteProtocolType(const CPIRouteEntry *route)
{
    assertNotNull(route, "Parameter must be non-null");
    return route->routingProtocol;
}

CPINameRouteType
cpiRouteEntry_GetRouteType(const CPIRouteEntry *route)
{
    assertNotNull(route, "Parameter must be non-null");
    return route->routeType;
}

unsigned
cpiRouteEntry_GetCost(const CPIRouteEntry *route)
{
    assertNotNull(route, "Parameter must be non-null");
    return route->cost;
}

const char *
cpiRouteEntry_GetSymbolicName(const CPIRouteEntry *route)
{
    assertNotNull(route, "Parameter must be non-null");
    return route->symbolic;
}

PARCJSON *
cpiRouteEntry_ToJson(const CPIRouteEntry *route)
{
    assertNotNull(route, "Parameter must be non-null");

    PARCJSON *routeJson = parcJSON_Create();
    char *uri = ccnxName_ToString(route->prefix);
    parcJSON_AddString(routeJson, cpiPrefix, uri);
    parcMemory_Deallocate((void **) &uri);

    if (route->symbolic) {
        parcJSON_AddString(routeJson, cpiSymbolic, route->symbolic);
    }

    if (route->hasInterfaceIndex) {
        parcJSON_AddInteger(routeJson, cpiInterface, route->interfaceIndex);
    }

    parcJSON_AddInteger(routeJson, cpiFlags, 0);

    if (route->nexthop) {
        // some registrations can have NULL nexthop, its ok
        PARCJSON *json = cpiAddress_ToJson(route->nexthop);
        parcJSON_AddObject(routeJson, cpiNexthop, json);
        parcJSON_Release(&json);
    }

    parcJSON_AddString(routeJson, cpiProtocol, cpiNameRouteProtocolType_ToString(route->routingProtocol));
    parcJSON_AddString(routeJson, cpiRouteType, cpiNameRouteType_ToString(route->routeType));
    parcJSON_AddInteger(routeJson, cpiCost, route->cost);

    if (route->hasLifetime) {
        PARCJSONArray *lifetimeJson = parcJSONArray_Create();
        PARCJSONValue *value = parcJSONValue_CreateFromInteger(route->lifetime.tv_sec);
        parcJSONArray_AddValue(lifetimeJson, value);
        parcJSONValue_Release(&value);
        value = parcJSONValue_CreateFromInteger(route->lifetime.tv_usec);
        parcJSONArray_AddValue(lifetimeJson, value);
        parcJSONValue_Release(&value);
        parcJSON_AddArray(routeJson, cpiLifetime, lifetimeJson);
        parcJSONArray_Release(&lifetimeJson);
    }

    return routeJson;
}

CPIRouteEntry *
cpiRouteEntry_FromJson(PARCJSON *json)
{
    assertNotNull(json, "Parameter json must be non-null");
    PARCJSON *routeJson = json;

    PARCJSONValue *value = parcJSON_GetValueByName(routeJson, cpiPrefix);
    assertNotNull(value, "Couldn't locate tag %s in: %s", cpiPrefix, parcJSON_ToString(json));
    PARCBuffer *sBuf = parcJSONValue_GetString(value);
    CCNxName *prefix = ccnxName_CreateFromCString(parcBuffer_Overlay(sBuf, 0));

    const char *symbolicName = NULL;
    value = parcJSON_GetValueByName(routeJson, cpiSymbolic);
    if (value) {
        sBuf = parcJSONValue_GetString(value);
        symbolicName = parcBuffer_Overlay(sBuf, 0);
    }

    bool hasInterfaceIndex = false;
    unsigned interfaceIndex = UINT32_MAX;
    value = parcJSON_GetValueByName(routeJson, cpiInterface);
    if (value) {
        hasInterfaceIndex = true;
        interfaceIndex = (unsigned) parcJSONValue_GetInteger(value);
    }

    CPIAddress *nexthop = NULL;
    value = parcJSON_GetValueByName(routeJson, cpiNexthop);
    if (value != NULL) {
        assertTrue(parcJSONValue_IsJSON(value),
                   "Json key %s wrong type in json %s",
                   cpiNexthop,
                   parcJSON_ToString(json));
        PARCJSON *nexthopJson = parcJSONValue_GetJSON(value);
        nexthop = cpiAddress_CreateFromJson(nexthopJson);
    }

    value = parcJSON_GetValueByName(routeJson, cpiProtocol);
    sBuf = parcJSONValue_GetString(value);
    char *valueString = parcBuffer_Overlay(sBuf, 0);
    CPINameRouteProtocolType routingProtocol = cpiNameRouteProtocolType_FromString(valueString);

    value = parcJSON_GetValueByName(routeJson, cpiRouteType);
    sBuf = parcJSONValue_GetString(value);
    valueString = parcBuffer_Overlay(sBuf, 0);
    CPINameRouteType routingType = cpiNameRouteType_FromString(valueString);

    value = parcJSON_GetValueByName(routeJson, cpiCost);
    unsigned cost = (unsigned) parcJSONValue_GetInteger(value);

    value = parcJSON_GetValueByName(routeJson, cpiLifetime);
    struct timeval *lifetime = NULL;

    struct timeval actual_time = { 0, 0 };
    if (value != NULL) {
        assertTrue(parcJSONValue_IsArray(value),
                   "Json key %s wrong typein json %s",
                   cpiNexthop,
                   parcJSON_ToString(json));
        PARCJSONArray *lifetimeJson = parcJSONValue_GetArray(value);

        actual_time.tv_sec = parcJSONValue_GetInteger(parcJSONArray_GetValue(lifetimeJson, 0));
        actual_time.tv_usec = (int) parcJSONValue_GetInteger(parcJSONArray_GetValue(lifetimeJson, 1));

        lifetime = &actual_time;
    }


    // == we're now ready to create the object
    CPIRouteEntry *route;
    if (symbolicName) {
        route = cpiRouteEntry_CreateSymbolic(prefix, symbolicName, routingProtocol, routingType, lifetime, cost);
        if (hasInterfaceIndex) {
            cpiRouteEntry_SetInterfaceIndex(route, interfaceIndex);
        }
    } else {
        route = cpiRouteEntry_Create(prefix, interfaceIndex, nexthop, routingProtocol, routingType, lifetime, cost);
    }

    if (nexthop) {
        cpiAddress_Destroy(&nexthop);
    }

    return route;
}

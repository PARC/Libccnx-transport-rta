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
#include <config.h>
#include <LongBow/runtime.h>

#include <stdio.h>
#include "config_Forwarder_Metis.h"
#include <ccnx/transport/transport_rta/core/components.h>

static const char param_METIS_PORT[] = METIS_PORT_ENV;          // integer, e.g. 9695
static const short default_port = 9695;

/**
 * Generates:
 *
 * { "FWD_METIS" : { "port" : port } }
 */
CCNxStackConfig *
metisForwarder_ProtocolStackConfig(CCNxStackConfig *stackConfig)
{
    PARCJSONValue *value = parcJSONValue_CreateFromNULL();
    CCNxStackConfig *result = ccnxStackConfig_Add(stackConfig, metisForwarder_GetName(), value);
    parcJSONValue_Release(&value);

    return result;
}

/**
 * The metis forwarder port may be set per connection in the stack
 *
 * { "FWD_METIS" : { "port" : port } }
 */
CCNxConnectionConfig *
metisForwarder_ConnectionConfig(CCNxConnectionConfig *connConfig, uint16_t port)
{
    PARCJSON *json = parcJSON_Create();
    parcJSON_AddInteger(json, param_METIS_PORT, port);

    PARCJSONValue *value = parcJSONValue_CreateFromJSON(json);
    parcJSON_Release(&json);
    CCNxConnectionConfig *result = ccnxConnectionConfig_Add(connConfig, metisForwarder_GetName(), value);
    parcJSONValue_Release(&value);

    return result;
}

uint16_t
metisForwarder_GetDefaultPort()
{
    return default_port;
}

const char *
metisForwarder_GetName()
{
    return RtaComponentNames[FWD_METIS];
}

uint16_t
metisForwarder_GetPortFromConfig(PARCJSON *json)
{
    PARCJSONValue *value = parcJSON_GetValueByName(json, metisForwarder_GetName());
    assertNotNull(value, "Got null for %s json", metisForwarder_GetName());
    PARCJSON *metisJson = parcJSONValue_GetJSON(value);

    value = parcJSON_GetValueByName(metisJson, param_METIS_PORT);
    return (uint16_t) parcJSONValue_GetInteger(value);
}

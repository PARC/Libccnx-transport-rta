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
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2014, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
/*
 *  {
 *      "CPI_ACK" : {
 *          "SEQUENCE" : <sequence number>,
 *          "RETURN"   : "ACK" or "NACK",
 *          "REQUEST"  : <original request JSON>
 *          [, "MESSAGE" : <optional message> ]
 *        }
 *     ["AUTHENTICATOR" : <TBD proof based on request/response, e.g. a crypto signature>]
 *  }
 */

#include <config.h>
#include <stdio.h>
#include <strings.h>

#include <LongBow/runtime.h>

#include "controlPlaneInterface.h"
#include "cpi_private.h"
#include "cpi_Acks.h"

struct control_plane_ack {
    ControlPlaneInformation cpi_ack;
    CpiAckType ack_type;
    ControlPlaneInformation cpi_original;
};

static const char *cpiReturn = "RETURN";
static const char *cpiReturnAck = "ACK";
static const char *cpiReturnNack = "NACK";
static const char *cpiOriginal = "REQUEST";
static const char *cpiRequest = "CPI_REQUEST";

PARCJSON *
cpiAcks_CreateAck(const PARCJSON *originalRequest)
{
    uint64_t seqnum = cpi_GetNextSequenceNumber();
    PARCJSON *body = parcJSON_Create();

    parcJSON_AddInteger(body, cpiSeqnum, (int) seqnum);
    parcJSON_AddString(body, cpiReturn, cpiReturnAck);

    PARCJSON *copy = parcJSON_Copy(originalRequest);
    parcJSON_AddObject(body, cpiOriginal, copy);
    parcJSON_Release(&copy);

    PARCJSON *json = parcJSON_Create();

    parcJSON_AddObject(json, cpiAck, body);
    parcJSON_Release(&body);

    return json;
}

PARCJSON *
cpiAcks_CreateNack(const PARCJSON *request)
{
    uint64_t seqnum = cpi_GetNextSequenceNumber();
    PARCJSON *body = parcJSON_Create();
    parcJSON_AddInteger(body, cpiSeqnum, (int) seqnum);
    parcJSON_AddString(body, cpiReturn, cpiReturnNack);

    PARCJSON *copy = parcJSON_Copy(request);
    parcJSON_AddObject(body, cpiOriginal, copy);
    parcJSON_Release(&copy);

    PARCJSON *json = parcJSON_Create();
    parcJSON_AddObject(json, cpiAck, body);
    parcJSON_Release(&body);

    return json;
}

bool
cpiAcks_IsAck(const PARCJSON *json)
{
    PARCJSONValue *ack_value = parcJSON_GetValueByName(json, cpiAck);
    if (ack_value != NULL) {
        PARCJSON *ack_json = parcJSONValue_GetJSON(ack_value);
        PARCJSONValue *return_value = parcJSON_GetValueByName(ack_json, cpiReturn);
        PARCBuffer *sBuf = parcJSONValue_GetString(return_value);
        const char *returnStr = parcBuffer_Overlay(sBuf, 0);
        return strcasecmp(returnStr, cpiReturnAck) == 0;
    }
    return false;
}

uint64_t
cpiAcks_GetAckOriginalSequenceNumber(const PARCJSON *json)
{
    PARCJSONValue *value = parcJSON_GetValueByName(json, cpiAck);
    assertNotNull(value, "got null ack json: %s", parcJSON_ToString(json));

    PARCJSON *tempJson = parcJSONValue_GetJSON(value);

    value = parcJSON_GetValueByName(tempJson, cpiOriginal);
    assertNotNull(value, "got null original json from the ack: %s", parcJSON_ToString(tempJson));

    tempJson = parcJSONValue_GetJSON(value);

    value = parcJSON_GetValueByName(tempJson, cpiRequest);
    assertNotNull(value, "got null request json from the ack: %s", parcJSON_ToString(tempJson));

    tempJson = parcJSONValue_GetJSON(value);

    value = parcJSON_GetValueByName(tempJson, cpiSeqnum);
    assertNotNull(value, "got null seqnum inside the request: %s", parcJSON_ToString(tempJson));

    return parcJSONValue_GetInteger(value);
}

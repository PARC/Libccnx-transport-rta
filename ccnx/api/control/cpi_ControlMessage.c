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
 * @author Marc Mosko, Glenn Scott, Alan Walendowski, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>

#include <LongBow/runtime.h>

#include <ccnx/api/control/cpi_ControlMessage.h>

#include <ccnx/api/control/cpi_ControlFacade.h>

#include <ccnx/api/notify/notify_Status.h>

#include <ccnx/api/control/controlPlaneInterface.h>

PARCJSON *
ccnxControl_GetJson(const CCNxControl *control)
{
    return ccnxControlFacade_GetJson(control);
}

void
ccnxControl_Display(const CCNxControl *control, int indentation)
{
    ccnxControlFacade_Display(control, indentation);
}

void
ccnxControl_Release(CCNxControl **controlP)
{
    ccnxTlvDictionary_Release(controlP);
}

CCNxControl *
ccnxControl_Acquire(const CCNxControl *control)
{
    return ccnxTlvDictionary_Acquire(control);
}

bool
ccnxControl_IsACK(const CCNxControl *control)
{
    if (cpi_GetMessageType(control) == CPI_ACK) {
        PARCJSON *json = ccnxControlFacade_GetJson(control);
        return cpiAcks_IsAck(json);
    }
    return false;
}

bool
ccnxControl_IsNACK(const CCNxControl *control)
{
    if (cpi_GetMessageType(control) == CPI_ACK) {
        PARCJSON *json = ccnxControlFacade_GetJson(control);
        return !cpiAcks_IsAck(json);
    }
    return false;
}

uint64_t
ccnxControl_GetAckOriginalSequenceNumber(const CCNxControl *control)
{
    PARCJSON *json = ccnxControlFacade_GetJson(control);
    return cpiAcks_GetAckOriginalSequenceNumber(json);
}

bool
ccnxControl_IsNotification(const CCNxControl *control)
{
    return ccnxControlFacade_IsNotification(control);
}

NotifyStatus *
ccnxControl_GetNotifyStatus(const CCNxControl *control)
{
    return notifyStatus_ParseJSON(ccnxControl_GetJson(control));
}

CCNxControl *
ccnxControl_CreateCPIRequest(PARCJSON *json)
{
    return ccnxControlFacade_CreateCPI(json);
}

CCNxControl *
ccnxControl_CreateAddRouteRequest(const CPIRouteEntry *route)
{
    PARCJSON *cpiRequest = cpiForwarding_CreateAddRouteRequest(route);
    CCNxControl *result = ccnxControl_CreateCPIRequest(cpiRequest);
    parcJSON_Release(&cpiRequest);
    return result;
}

CCNxControl *
ccnxControl_CreateRemoveRouteRequest(const CPIRouteEntry *route)
{
    PARCJSON *cpiRequest = cpiForwarding_CreateRemoveRouteRequest(route);
    CCNxControl *result = ccnxControl_CreateCPIRequest(cpiRequest);
    parcJSON_Release(&cpiRequest);
    return result;
}

CCNxControl *
ccnxControl_CreateRouteListRequest()
{
    PARCJSON *cpiRequest = cpiForwarding_CreateRouteListRequest();
    CCNxControl *result = ccnxControl_CreateCPIRequest(cpiRequest);
    parcJSON_Release(&cpiRequest);
    return result;
}

CCNxControl *
ccnxControl_CreateConnectionListRequest()
{
    PARCJSON *cpiRequest = cpiLinks_CreateConnectionListRequest();
    CCNxControl *result = ccnxControl_CreateCPIRequest(cpiRequest);
    parcJSON_Release(&cpiRequest);
    return result;
}

CCNxControl *
ccnxControl_CreateInterfaceListRequest()
{
    PARCJSON *cpiRequest = cpiLinks_CreateInterfaceListRequest();
    CCNxControl *result = ccnxControl_CreateCPIRequest(cpiRequest);
    parcJSON_Release(&cpiRequest);
    return result;
}

CCNxControl *
ccnxControl_CreateAddRouteToSelfRequest(const CCNxName *name)
{
    CPIRouteEntry *route = cpiRouteEntry_CreateRouteToSelf(name);
    CCNxControl *result = ccnxControl_CreateAddRouteRequest(route);
    cpiRouteEntry_Destroy(&route);
    return result;
}

CCNxControl *
ccnxControl_CreateRemoveRouteToSelfRequest(const CCNxName *name)
{
    CPIRouteEntry *route = cpiRouteEntry_CreateRouteToSelf(name);
    CCNxControl *result = ccnxControl_CreateRemoveRouteRequest(route);
    cpiRouteEntry_Destroy(&route);
    return result;
}

CCNxControl *
ccnxControl_CreatePauseInputRequest()
{
    PARCJSON *cpiRequest = cpi_CreatePauseInputRequest();
    CCNxControl *result = ccnxControl_CreateCPIRequest(cpiRequest);
    parcJSON_Release(&cpiRequest);
    return result;
}

CCNxControl *
ccnxControl_CreateFlushRequest(void)
{
    PARCJSON *cpiRequest = cpi_CreateFlushRequest();
    CCNxControl *result = ccnxControl_CreateCPIRequest(cpiRequest);
    parcJSON_Release(&cpiRequest);
    return result;
}

bool
ccnxControl_IsCPI(const CCNxControl *controlMsg)
{
    return ccnxControlFacade_IsCPI((CCNxTlvDictionary *) controlMsg);
}

CCNxControl *
ccnxControl_CreateIPTunnelRequest(const CPIInterfaceIPTunnel *tunnel)
{
    PARCJSON *request = cpiLinks_CreateIPTunnel(tunnel);
    CCNxControl *result = ccnxControl_CreateCPIRequest(request);
    parcJSON_Release(&request);
    return result;
}

CCNxControl *
ccnxControl_CreateCancelFlowRequest(const CCNxName *name)
{
    PARCJSON *request = cpiCancelFlow_CreateRequest(name);
    CCNxControl *result = ccnxControl_CreateCPIRequest(request);
    parcJSON_Release(&request);
    return result;
}

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
 * @file ccnx_Control.h
 * @brief This is a stack control message.
 *
 * This may induce other Control messages for the stack, for the Forwarder, or potentially
 * for the network.
 *
 * @author Glenn Scott, Alan Walendowski, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef libccnx_ccnx_Control_h
#define libccnx_ccnx_Control_h

#include <stdint.h>

#include <ccnx/common/internal/ccnx_TlvDictionary.h>

#include <ccnx/api/notify/notify_Status.h>

#include <ccnx/api/control/cpi_RouteEntry.h>
#include <ccnx/api/control/cpi_InterfaceIPTunnel.h>

/**
 * @typedef CCNxControl
 * @brief Control message for CCNx.
 */
typedef CCNxTlvDictionary CCNxControl;

/**
 * Increase the number of references to a `CCNxControl` instance.
 *
 * Note that new `CCNxControl` is not created,
 * only that the given `CCNxControl` reference count is incremented.
 * Discard the reference by invoking {@link ccnxControl_Release()}.
 *
 * @param [in] control A pointer to the original instance.
 * @return The value of the input parameter @p control.
 *
 * Example:
 * @code
 * {
 *     ...
 *
 *     CCNxControl *control = ccnxControl_Acquire(instance);
 *
 *     ccnxControl_Release(&control);
 *
 * }
 * @endcode
 *
 * @see {@link ccnxControl_Release}
 */
CCNxControl *ccnxControl_Acquire(const CCNxControl *control);

/**
 * Print a human readable representation of the given `CCNxControl` instance.
 *
 * @param [in] indentation The level of indentation to use to pretty-print the output.
 * @param [in] control A pointer to the instance to display.
 *
 * Example:
 * @code
 * {
 *     CCNxControl *control = < ... >
 *
 *     ccnxControl_Display(control, 4);
 *
 *     ccnxControl_Release(&control);
 * }
 * @endcode
 *
 */
void ccnxControl_Display(const CCNxControl *control, int indentation);

/**
 * Release a previously acquired reference to the specified instance,
 * decrementing the reference count for the instance.
 *
 * The pointer to the instance is set to NULL as a side-effect of this function.
 *
 * If the invocation causes the last reference to the instance to be released,
 * the instance is deallocated and the instance's implementation will perform
 * additional cleanup and release other privately held references.
 *
 * @param [in,out] controlP A pointer to a pointer to the instance to release.
 *
 * Example:
 * @code
 * {
 *     CCNxControl *control = < ... >
 *
 *     ccnxControl_Release(&control);
 * }
 * @endcode
 *
 * @see {@link ccnxControl_Acquire}
 */
void ccnxControl_Release(CCNxControl **controlP);

/**
 * Return the original sequence number to which an ACK corresponds.
 *
 * Control plane messages contain sequence numbers. When an ACK is received, this function
 * returns the sequence number of the control plane message being ACKed.
 *
 * @param [in] control A pointer to a `CCNxControl` instance.
 *
 * @return The sequence number of the control plane message being ACKed.
 *
 * Example:
 * @code
 * {
 *     uint64_t originalSequenceNumber = ccnxControl_GetAckOriginalSequenceNumber(control);
 * }
 * @endcode
 */
uint64_t ccnxControl_GetAckOriginalSequenceNumber(const CCNxControl *control);

/**
 * Return true if the specified `CCNxControl` instance is a Notification.
 *
 * @param [in] control A pointer to a `CCNxControl` instance.
 *
 * @return `true` if the specified `CCNxControl` instance is a Notification message.
 * @return `false` if the specified `CCNxControl` instance is not a Notification message.
 *
 * Example:
 * @code
 * {
 *     bool isNotification = ccnxControl_IsNotification(control);
 * }
 * @endcode
 *
 * @see {@link ccnxControl_IsACK}
 */
bool ccnxControl_IsNotification(const CCNxControl *control);

/**
 * Return `true` if the specified `CCNxControl` instance is an ACK message carrying an ACK (not a NACK)
 *
 * An acknolwedgement message can be either a positive (ACK) or negative (NACK) acknowlegement.
 * In both cases, it carries the original sequence number of the message being ACKed or NACKed.
 *
 * @param [in] control A pointer to a `CCNxControl` instance.
 *
 * @return `true` if the specified `CCNxControl` instance is an Ack message.
 * @return `false` if the specified `CCNxControl` instance is not an Ack message.
 *
 * Example:
 * @code
 * {
 *     bool isAck = ccnxControl_IsACK(control);
 * }
 * @endcode
 *
 * @see {@link ccnxControl_IsNotification}
 */
bool ccnxControl_IsACK(const CCNxControl *control);

/**
 * Return `true` if the specified `CCNxControl` instance is an ACK message carrying a NACK (not a ACK)
 *
 * An acknolwedgement message can be either a positive (ACK) or negative (NACK) acknowlegement.
 * In both cases, it carries the original sequence number of the message being ACKed or NACKed.
 *
 * @param [in] control A pointer to a `CCNxControl` instance.
 *
 * @return `true` if the specified `CCNxControl` is an NACK.
 * @return `false` if the specified `CCNxControl` instance is not an NAck message.
 *
 * Example:
 * @code
 * {
 *     bool isAck = ccnxControl_IsACK(control);
 * }
 * @endcode
 *
 * @see {@link ccnxControl_IsNotification}
 */
bool ccnxControl_IsNACK(const CCNxControl *control);

/**
 * Get the {@link NotifyStatus} from a `CCNxControl` instance, if it exists.
 *
 * This function creates a new instance of `NotifyStatus`, initialized from the specified
 * `CCNxControl`, which must eventually be released by calling {@link notifyStatus_Release}().
 * If the specified `CCNxControl` instance does not contain a `NotifyStatus`, this function will return NULL.
 *
 * @param [in] control A pointer to the instance of `CCNxControl` from which to retrieve the `NotifyStatus`.
 *
 * @return An instance of `NotifyStatus`, if it exists.
 * @return NULL If the `CCNxControl` instance did not contain a `NotifyStatus`.
 *
 * Example:
 * @code
 * {
 *     NotifyStatus status = ccnxControl_GetNotifyStatus(control);
 *
 *     notifyStatus_Release(&status);
 * }
 * @endcode
 *
 * @see {@link notifyStatus_Release}
 * @see {@link NotifyStatus}
 */
NotifyStatus *ccnxControl_GetNotifyStatus(const CCNxControl *control);

/**
 * Create a new `CCNxControl` instance containing a request to add a route to the control plane.
 *
 * The new `CCNxControl` instance must eventually be released by calling {@link ccnxControl_Release()}.
 *
 * @param [in] route The {@link CPIRouteEntry} to add.
 *
 * @return A new `CCNxControl` instance encapsulating a request to add the specified route.
 *
 * Example:
 * @code
 * {
 *     CPIRouteEntry *cpiRouteEntry = cpiRouteEntry_Create(...);
 *
 *     CCNxControl *control = ccnxControl_CreateAddRouteRequest(cpiRouteEntry);
 *
 *     cpiRouteEntry_Destroy(&cpiRouteEntry);
 *     ccnxControl_Release(&control);
 * }
 * @endcode
 *
 * @see {@link ccnxControl_CreateRemoveRouteRequest}
 * @see {@link ccnxControl_Release}
 */
CCNxControl *ccnxControl_CreateAddRouteRequest(const CPIRouteEntry *route);

/**
 * Create a new `CCNxControl` instance containing a request to remove a route from the control plane.
 *
 * The new `CCNxControl` instance must eventually be released by calling {@link ccnxControl_Release}.
 *
 * @param [in] route The {@link CPIRouteEntry} to remove.
 *
 * @return A new `CCNxControl` instance encapsulating a request to remove the specified route.
 *
 * Example:
 * @code
 * {
 * {
 *     CPIRouteEntry *cpiRouteEntry = cpiRouteEntry_Create(...);
 *
 *     CCNxControl *control = ccnxControl_CreateRemoveRouteRequest(cpiRouteEntry);
 *
 *     cpiRouteEntry_Destroy(&cpiRouteEntry);
 *     ccnxControl_Release(&control);
 * }
 * @endcode
 *
 * @see {@link ccnxControl_CreateAddRouteRequest}
 * @see {@link ccnxControl_Release}
 */
CCNxControl *ccnxControl_CreateRemoveRouteRequest(const CPIRouteEntry *route);

/**
 * Create a new `CCNxControl` instance containing a request to add a route for CCN messages matching the given {@link CCNxName}
 * back to the caller's network interface.
 *
 * The created `CCNxControl` message describes to the forwarder that messages matching the specified `CCNxName` should be
 * routed back to the caller. This is how to initiate listening for a name.
 * The new `CCNxControl` instance must eventually be released by calling {@link ccnxControl_Release}.
 *
 * @param [in] name A pointer to a `CCNxName` instance containing the name to match against.
 *
 * @return A new `CCNxControl` instance encapsulating a request to add a route for the given `CCNxName` back to the caller's
 *         network interface.
 *
 * Example:
 * @code
 * {
 *     CCNxName *name = ccnxName_CreateFromCString("lci:/parc/csl/media/thingie");
 *
 *     CCNxControl *control = ccnxControl_CreateAddRouteToSelfRequest(name);
 *     ...
 *
 *     ccnxName_Release(&name);
 *     ccnxControl_Release(&control);
 * }
 * @endcode
 *
 * @see {@link ccnxControl_CreateRemoveRouteToSelfRequest}
 * @see {@link ccnxControl_CreateAddRouteRequest}
 * @see {@link ccnxControl_Release}
 */
CCNxControl *ccnxControl_CreateAddRouteToSelfRequest(const CCNxName *name);

/**
 * Create a new `CCNxControl` instance containing a request to remove a route to the caller for messages matching the specified
 * {@link CCNxName}.
 *
 *
 * The new `CCNxControl` instance must eventually be released by calling {@link ccnxControl_Release}.
 *
 * @param [in] name A pointer to a `CCNxName` instance containing the name to match against.
 *
 * @return A new `CCNxControl` instance encapsulating a request to remove the specified route.
 *
 * Example:
 * @code
 * {
 *     CCNxName *name = ccnxName_CreateFromCString("lci:/parc/csl/media/thingie");
 *
 *     CCNxControl *control = ccnxControl_CreateRemoveRouteToSelfRequest(name);
 *     ...
 *
 *     ccnxName_Release(&name);
 *     ccnxControl_Release(&control);
 * }
 * @endcode
 *
 * @see {@link ccnxControl_CreateAddRouteToSelfRequest}
 * @see {@link ccnxControl_Release}
 */
CCNxControl *ccnxControl_CreateRemoveRouteToSelfRequest(const CCNxName *name);

/**
 * Create a new `CCNxControl` instance containing the specified CPI command, and including the
 * flag indicating that it is a CPI message.
 *
 * The new `CCNxControl` instance must eventually be released by calling {@link ccnxControl_Release}.
 *
 * @param [in] json A pointer to a {@link PARCJSON} instance containing CPI command to wrap.
 *
 * @return A new `CCNxControl` instance containing the specified CPI command.
 *
 * Example:
 * @code
 * {
 *     CCNxName *name = ccnxName_CreateFromCString("lci:/boose/roo/pie");
 *     PARCJSON *cpiRequest = cpiCancelFlow_CreateRequest(name);
 *     CCNxControl *control = ccnxControl_CreateCPIRequest(cpiRequest);
 *
 *     ...
 *
 *     parcJSON_Release(&cpiRequest);
 *     ccnxControl_Release(&control);
 *     ccnxName_Release(&name);
 * }
 * @endcode
 *
 * @see {@link ccnxControl_Release}
 */
CCNxControl *ccnxControl_CreateCPIRequest(PARCJSON *json);

/**
 * Create a new `CCNxControl` instance containing a "List Routes" request.
 *
 * The new `CCNxControl` instance must eventually be released by calling {@link ccnxControl_Release}.
 *
 * @return A new `CCNxControl` instance containing the request.
 *
 * Example:
 * @code
 * {
 *     CCNxControl *control = ccnxControl_CreateRouteListRequest();
 *     PARCJSON *json = ccnxControl_GetJson(control);
 *
 *     ...
 *
 *     ccnxControl_Release(&control);
 *  }
 * @endcode
 *
 * @see {@link ccnxControl_Release}
 */
CCNxControl *ccnxControl_CreateRouteListRequest(void);

/**
 * Create a new `CCNxControl` instance containing a "List Connections" request.
 *
 * The new `CCNxControl` instance must eventually be released by calling {@link ccnxControl_Release}.
 *
 * @return A new `CCNxControl` instance containing the request.
 *
 * Example:
 * @code
 * {
 *     CCNxControl *control = ccnxControl_CreateRouteListRequest();
 *
 *     ...
 *
 *     ccnxControl_Release(&control);
 * }
 * @endcode
 *
 * @see {@link ccnxControl_Release}
 */
CCNxControl *ccnxControl_CreateConnectionListRequest(void);

/**
 * Create a new `CCNxControl` instance containing a "List Interfaces" request.
 *
 * The new `CCNxControl` instance must eventually be released by calling {@link ccnxControl_Release}.
 *
 * @return A new `CCNxControl` instance containing the request.
 *
 * Example:
 * @code
 * {
 *     CCNxControl *control = ccnxControl_CreateInterfaceListRequest();
 *
 *     ...
 *
 *     ccnxControl_Release(&control);
 * }
 * @endcode
 *
 * @see {@link ccnxControl_Release}
 */
CCNxControl *ccnxControl_CreateInterfaceListRequest(void);

/**
 * Create a new `CCNxControl` instance containing a "Pause Input" request.
 *
 * The new `CCNxControl` instance must eventually be released by calling {@link ccnxControl_Release}.
 *
 * @return A new `CCNxControl` instance containing the request.
 *
 * Example:
 * @code
 * {
 *   CCNxControl *control = ccnxControl_CreatePauseInputRequest();
 *
 *   ...
 *
 *   ccnxControl_Release(&control);
 * }
 * @endcode
 *
 * @see {@link ccnxControl_Release}
 * @see {@link cpi_CreatePauseInputRequest}
 */
CCNxControl *ccnxControl_CreatePauseInputRequest(void);


/**
 * Creates a request to flush the output.  The ForwarderConnector will ACK the request.
 *
 * When the user recieves an ACK with the corresponding sequence number as this request, the
 * user knows that all ouptut prior to that request has been processed.
 *
 * @retval non-null An allocated CCnxControl message
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxControl *ccnxControl_CreateFlushRequest(void);

/**
 * Create a new `CCNxControl` instance containing a "Cancel Flow" request.
 *
 * The new `CCNxControl` instance must eventually be released by calling {@link ccnxControl_Release}.
 * @param [in] name A pointer to a `CCNxName`.
 *
 * @return A new `CCNxControl` instance containing the request.
 *
 * Example:
 * @code
 * {
 *     CCNxName *name = ccnxName_CreateFromCString("lci:/boose/roo/pie");
 *     CCNxControl *control = ccnxControl_CreateCancelFlowRequest(name);
 *
 *     ...
 *
 *     ccnxControl_Release(&control);
 *     ccnxName_Release(&name);
 * }
 * @endcode
 *
 * @see {@link ccnxControl_Release}
 */
CCNxControl *ccnxControl_CreateCancelFlowRequest(const CCNxName *name);

/**
 * Create a new `CCNxControl` instance containing a "Create IP Tunnel" request.
 *
 * The new `CCNxControl` instance must eventually be released by calling {@link ccnxControl_Release}.
 *
 * @param [in] tunnel An instance of `CPIInterfaceIPTunnel` to be included.
 * @return A new `CCNxControl` instance containing the request.
 *
 * Example:
 * @code
 * {
 *     struct sockaddr_in sockaddr_any;
 *     memset(&sockaddr_any, 0, sizeof(sockaddr_any));
 *     sockaddr_any.sin_family = PF_INET;
 *     sockaddr_any.sin_addr.s_addr = INADDR_ANY;
 *
 *     CPIAddress *source = cpiAddress_CreateFromInet(&sockaddr_any);
 *
 *     struct sockaddr_in sockaddr_dst;
 *     memset(&sockaddr_dst, 0, sizeof(sockaddr_dst));
 *     sockaddr_dst.sin_family = PF_INET;
 *     sockaddr_dst.sin_port = htons(9999);
 *     inet_pton(AF_INET, "127.0.0.1", &(sockaddr_dst.sin_addr));
 *
 *     CPIAddress *destination = cpiAddress_CreateFromInet(&sockaddr_dst);
 *
 *     CPIInterfaceIPTunnel *tunnel = cpiInterfaceIPTunnel_Create(0, source, destination, IPTUN_TCP);
 *     CCNxControl *control = ccnxControl_CreateIPTunnelRequest(tunnel);
 *
 *     ...
 *
 *     ccnxControl_Release(&control);
 *     cpiInterfaceIPTunnel_Destroy(&tunnel);
 * }
 * @endcode
 *
 * @see {@link ccnxControl_Release}
 */
CCNxControl *ccnxControl_CreateIPTunnelRequest(const CPIInterfaceIPTunnel *tunnel);

/**
 * Return true if the specified `CCNxControl` instance is an a CPI request.
 *
 * @param [in] controlMsg A pointer to a `CCNxControl` instance.
 *
 * @return `true` if the specified `CCNxControl` instance is a CPI request.
 * @return `false` if the specified `CCNxControl` instance is not a CPI request.
 *
 * Example:
 * @code
 * {
 *     bool isCPI = ccnxControl_IsCPI(control);
 * }
 * @endcode
 *
 * @see {@link ccnxControl_IsNotification}
 */
bool ccnxControl_IsCPI(const CCNxControl *controlMsg);

/**
 * Return the underlying CPI request from the specified `CCNxControl`.
 *
 * @return A pointer to the underlying CPI request object.
 *
 * Example:
 * @code
 * {
 *     CCNxControl *control = ccnxControl_CreateRouteListRequest();
 *     PARCJSON *json = ccnxControl_GetJson(control);
 *
 *     ...
 *
 *     ccnxControl_Release(&control);
 * }
 * @endcode
 *
 * @see ccnxControl_Release
 */
PARCJSON *ccnxControl_GetJson(const CCNxControl *control);
#endif // libccnx_ccnx_Control_h

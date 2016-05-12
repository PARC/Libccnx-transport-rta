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
/** @file rta_ProtocolStack.h
 *  @brief A set of connectors and components
 *
 * In a Ready To Assemble transport, individual pieces are called connectors
 * and components.  A connector attaches to the API library at the top and
 * to the forwarder at the bottom.  In between the connectors are components.
 *
 * One set of connectors and components is called a protocol stack.
 *
 * A ProtocolStack defines a set of Components linked by bidirectional
 * queues.  A ProtocolStack is defined by the KeyValue set passed to
 * the Transport.  The hash of the KeyValue set selects the protocol stack.
 * If the Transport sees a new hash, it creates a new protocol stack
 * via ProtocolStack_Create().
 *
 * Each API connection calls _Open, which will return a new RtaConnection
 * pointer.  The Transport gives the API an "api_fd", which the Transport
 * translates to the RtaConnection.
 *
 * A protocol stack is implemented as a set of queue pairs between components.
 * There is a fixed sized array called queue_pairs[MAX_STACK_DEPTH].  The
 * queue_pairs[i].pair[RTA_DOWN] end attaches to the upper component.  RTA_DOWN
 * indicates the direction of travel for a write.  queue_pairs[i].pair[RTA_UP]
 * attaches to the lower component.
 *
 * A component only knows its identity (see components.h).  For example, the
 * TLV codec is called CODEC_TLV, and that is the only identity it know.  It does
 * not know the identity of the pieces above or below it.
 *
 * Therefore, when a component calls protocolStack_GetPutQ(stack, CODEC_TLV, RTA_DOWN),
 * it is asking for the queue to write to in the DOWN direction.  This means that
 * we should keep an index by the component name, not by the queue_pairs[] array.
 * Thus, we keep a component_queues[] array that is indexed by the component name.
 *
 * Let's say our stack is API_CONNECTOR, FC_NULL, VERIFY_NULL, CODEC_TLV, FWD_LOCAL.
 * The picture is like this:
 *
 * @code
 *         |
 *         *     <- api_connector managed queue
 *    API_CONNECTOR
 *         *     <- queue_pair[0].pair[DOWN]  <- component_queue[API_CONNECTOR].pair[DOWN]
 *         |
 *         *     <- queue_pair[0].pair[UP]    <- component_queue[FC_NULL].pair[UP]
 *      FC_NULL
 *         *     <- queue_pair[1].pair[DOWN]  <- component_queue[FC_NULL].pair[DOWN]
 *         |
 *         *     <- queue_pair[1].pair[UP]    <- component_queue[VERIFY_NULL].pair[UP]
 *     VERIFY_NULL
 *         *     <- queue_pair[2].pair[DOWN]  <- component_queue[VERIFY_NULL].pair[DOWN]
 *         |
 *         *     <- queue_pair[2].pair[UP]    <- component_queue[CODEC_TLV].pair[UP]
 *     CODEC_TLV
 *         *     <- queue_pair[3].pair[DOWN]  <- component_queue[CODEC_TLV].pair[DOWN]
 *         |
 *         *     <- queue_pair[3].pair[UP]    <- component_queue[FWD_LOCAL].pair[UP]
 *     FWD_LOCAL
 *         *     <- fwd_local managed connection
 *         |
 * @endcode
 *
 * Each component also has a pair of callbacks, one for reading messages flowing down
 * the stack and one for reading messages flowing up the stack.  These are called
 * "downcall_read" for reading messages flowing down and "upcall_read" for messages
 * flowing up.
 *
 * Recall that the direction attributes UP and DOWN in the queues are in terms
 * of WRITES, therefore the directions are opposite for reads.  A component's
 * downcall_read will read from component_queue[X].pair[UP].
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
#ifndef Libccnx_rta_ProtocolStack_h
#define Libccnx_rta_ProtocolStack_h

#include <parc/algol/parc_ArrayList.h>

#include <parc/algol/parc_EventQueue.h>

#include <ccnx/transport/transport_rta/core/rta_ComponentStats.h>
#include <ccnx/transport/transport_rta/core/rta_Framework.h>
#include <ccnx/transport/transport_rta/core/components.h>
#include <ccnx/transport/transport_rta/core/rta_ComponentQueue.h>
#include <ccnx/transport/transport_rta/commands/rta_Command.h>

struct rta_connection;
struct component_queue;

struct protocol_stack;
typedef struct protocol_stack RtaProtocolStack;

/**
 * Used to assign unique connection id to sockets.  This is just
 * for internal tracking, its not a descriptor.
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] stack <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 *
 * @see <#references#>
 */
unsigned rtaProtocolStack_GetNextConnectionId(RtaProtocolStack *stack);

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] framework <#description#>
 * @param [in] params <#description#>
 * @param [in] stack_id <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 *
 * @see <#references#>
 */
RtaProtocolStack *rtaProtocolStack_Create(RtaFramework *framework, PARCJSON *params, int stack_id);

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] stack <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 *
 * @see <#references#>
 */
int rtaProtocolStack_Configure(RtaProtocolStack *stack);

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] stack <#description#>
 * @param [in] component <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 *
 * @see <#references#>
 */
void *rtaProtocolStack_GetPrivateData(RtaProtocolStack *stack, RtaComponents component);
/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] stack <#description#>
 * @param [in] component <#description#>
 * @param [in] private <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 *
 * @see <#references#>
 */
void rtaProtocolStack_SetPrivateData(RtaProtocolStack *stack, RtaComponents component, void *private);

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] stack <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 *
 * @see <#references#>
 */
RtaFramework *rtaProtocolStack_GetFramework(RtaProtocolStack *stack);

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 *
 * @see <#references#>
 */
int rtaProtocolStack_GetStackId(RtaProtocolStack *stack);

/**
 * Opens a connection inside the protocol stack: it calls open() on each component.
 *
 * Returns 0 on success, -1 on error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
int rtaProtocolStack_Open(RtaProtocolStack *, struct rta_connection *connection);

/**
 *
 * 0 success, -1 error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
int rtaProtocolStack_Close(RtaProtocolStack *, struct rta_connection *conn);

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 *
 * @see <#references#>
 */
void rtaProtocolStack_Destroy(RtaProtocolStack **stack);

/**
 *  Return the queue used for output for a component in a given direction
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 *
 * @see <#references#>
 */
PARCEventQueue *rtaProtocolStack_GetPutQueue(RtaProtocolStack *stack,
                                             RtaComponents component,
                                             RtaDirection direction);

/**
 * <#One Line Description#>
 *
 * Domain is the top-level key, e.g. SYSTEM or USER
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 *
 * @see <#references#>
 */
PARCJSON *rtaProtocolStack_GetParam(RtaProtocolStack *stack, const char *domain, const char *key);

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 *
 * @see <#references#>
 */
RtaComponentStats *rtaProtocolStack_GetStats(const RtaProtocolStack *stack, RtaComponents type);

/**
 * <#OneLineDescription#>
 *
 *   <#Discussion#>
 *
 * @param stack
 * @param file
 * @return <#return#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
PARCArrayList *rtaProtocolStack_GetStatistics(const RtaProtocolStack *stack, FILE *file);

/**
 * Look up the symbolic name of the queue.  Do not free the return.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
const char *rtaProtocolStack_GetQueueName(RtaProtocolStack *stack, PARCEventQueue *queue);

/**
 * A state event occured on the given connection, let all the components know.
 *
 * A state changed occured (UP, DOWN, PAUSE, or flow control), notify all the components
 *
 * @param [in] connection The RtaConnection.
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void rtaProtocolStack_ConnectionStateChange(RtaProtocolStack *stack, void *connection);
#endif

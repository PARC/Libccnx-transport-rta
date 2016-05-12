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
 * @file rta_CommandCreateProtocolStack.h
 * @brief Represents a command to create a protocol stack
 *
 * Used to construct an RtaCommand object that is passed to rtaTransport_PassCommand() or _rtaTransport_SendCommandToFramework()
 * to send a command from the API's thread of execution to the Transport's thread of execution.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#ifndef Libccnx_rta_CommandCreateProtocolStack_h
#define Libccnx_rta_CommandCreateProtocolStack_h

struct rta_command_createprotocolstack;
typedef struct rta_command_createprotocolstack RtaCommandCreateProtocolStack;


#include <ccnx/transport/common/ccnx_StackConfig.h>

/**
 * Creates a CreateProtocolStack command object
 *
 * Creates a CreateProtocolStack command object used to signal the RTA framework to
 * create a new Protocol Stack with the specified stackId and configuration.  The caller is
 * responsible for ensuring that the stackId is unique among existing stacks (the framework might
 * assert an error for duplicates).  Note that the check for a unique stack ID is only done
 * once the RtaCommandCreateProtocolStack is passed to the RtaFramework, not on creation
 * of this object.
 *
 * @param [in] stackId The new (unique) ID for the stack to create
 * @param [in] config the JSON representation of the stack configuration
 *
 * @return non-null An allocated object
 * @return null An error
 *
 * Example:
 * @code
 * void
 * foo(RTATransport *transport)
 * {
 *     int stackId = nextStackIdNumber();
 *
 *     CCNxStackConfig *config = ccnxStackConfig_Create();
 *     RtaCommandCreateProtocolStack *createStack = rtaCommandCreateProtocolStack_Create(stackId, config);
 *     RtaCommand *command = rtaCommand_CreateCreateProtocolStack(createStack);
 *     _rtaTransport_SendCommandToFramework(transport, command);
 *     rtaCommand_Release(&command);
 *     rtaCommandCreateProtocolStack_Release(&createStack);
 *
 *     // ... do work ...
 *
 *     RtaCommandDestroyProtocolStack *destroyStack = rtaCommandDestroyProtocolStack_Create(stackId);
 *     command = rtaCommand_CreateDestroyProtocolStack(destroyStack);
 *     _rtaTransport_SendCommandToFramework(transport, command);
 *     rtaCommand_Release(&command);
 *     rtaCommandDestroyProtocolStack(&destroyStack);
 *     ccnxStackConfig_Release(&config);
 * }
 * @endcode
 */
RtaCommandCreateProtocolStack *rtaCommandCreateProtocolStack_Create(int stackId, CCNxStackConfig *config);

/**
 * Increase the number of references to a `RtaCommandCreateProtocolStack`.
 *
 * Note that new `RtaCommandCreateProtocolStack` is not created,
 * only that the given `RtaCommandCreateProtocolStack` reference count is incremented.
 * Discard the reference by invoking `rtaCommandCreateProtocolStack_Release`.
 *
 * @param [in] createStack The RtaCommandCreateProtocolStack to reference.
 *
 * @return non-null A reference to `createStack`.
 * @return null An error
 *
 * Example:
 * @code
 * {
 *     CCNxStackConfig *config = ccnxStackConfig_Create();
 *     RtaCommandCreateProtocolStack *createStack = rtaCommandCreateProtocolStack_Create(stackId, config);
 *     RtaCommandCreateProtocolStack *second = rtaCommandCreateProtocolStack_Acquire(createStack);
 *
 *     // release order does not matter
 *     rtaCommandCreateProtocolStack_Release(&createStack);
 *     rtaCommandCreateProtocolStack_Release(&second);
 *     ccnxStackConfig_Release(&config);
 * }
 * @endcode
 */
RtaCommandCreateProtocolStack *rtaCommandCreateProtocolStack_Acquire(const RtaCommandCreateProtocolStack *createStack);

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
 * @param [in,out] closePtr A pointer to the object to release, will return NULL'd.
 *
 * Example:
 * @code
 * {
 *     CCNxStackConfig *config = ccnxStackConfig_Create();
 *     RtaCommandCreateProtocolStack *createStack = rtaCommandCreateProtocolStack_Create(stackId, config);
 *     RtaCommand *command = rtaCommand_CreateCreateProtocolStack(createStack);
 *     _rtaTransport_SendCommandToFramework(transport, command);
 *     rtaCommand_Release(&command);
 *     rtaCommandCreateProtocolStack_Release(&createStack);
 *     ccnxStackConfig_Release(&config);
 * }
 * @endcode
 */
void rtaCommandCreateProtocolStack_Release(RtaCommandCreateProtocolStack **createStackPtr);

/**
 * Returns the Stack ID of the create stack command
 *
 * Returns the Stack ID parameter.
 *
 * @param [in] createStack An allocated RtaCommandCreateProtocolStack
 *
 * @return integer The value passed to rtaCommandCreateProtocolStack_Create().
 *
 * Example:
 * @code
 * {
 *     int stackId = 7;
 *     CCNxStackConfig *config = ccnxStackConfig_Create();
 *     RtaCommandCreateProtocolStack *createStack = rtaCommandCreateProtocolStack_Create(stackId, config);
 *     int testValue = rtaCommandCreateProtocolStack_GetStackId(createStack);
 *     assertTrue(testValue == stackId, "Wrong value got %d expected %d", testValue, stackId);
 *     rtaCommandCreateProtocolStack_Release(&createStack);
 *     ccnxStackConfig_Release(&config);
 * }
 * @endcode
 */
int rtaCommandCreateProtocolStack_GetStackId(const RtaCommandCreateProtocolStack *createStack);

/**
 * Get the CCNxStackConfig used by the given `RtaCommandCreateProtocolStack` instance.
 *
 * @param [in] createStack A pointer to a valid `RtaCommandCreateProtocolStack` instance.
 *
 * @return A pointer to the CCNxStackConfig used by the given `RtaCommandCreateProtocolStack` instance.
 *
 * Example:
 * @code
 * {
 *     int stackId = 7;
 *
 *     CCNxStackConfig *config = ccnxStackConfig_Create();
 *     RtaCommandCreateProtocolStack *createStack = rtaCommandCreateProtocolStack_Create(stackId, config);
 *
 *     CCNxStackConfig *config = rtaCommandCreateProtocolStack_GetStackConfig(createStack);
 *
 *     rtaCommandCreateProtocolStack_Release(&createStack);
 *
 *     ccnxStackConfig_Release(&config);
 * }
 * @endcode
 */
CCNxStackConfig *rtaCommandCreateProtocolStack_GetStackConfig(const RtaCommandCreateProtocolStack *createStack);

/**
 * Returns the PARCJSON stack configuration of the create stack command
 *
 * Returns the JSON representation of the stack configuration.
 *
 * @param [in] createStack An allocated RtaCommandCreateProtocolStack
 *
 * @return The value passed to rtaCommandCreateProtocolStack_Create().
 *
 * Example:
 * @code
 * {
 *     int stackId = 7;
 *
 *     CCNxStackConfig *config = ccnxStackConfig_Create();
 *     RtaCommandCreateProtocolStack *createStack = rtaCommandCreateProtocolStack_Create(stackId, config);
 *
 *     PARCJSON *testValue = rtaCommandCreateProtocolStack_GetConfig(createStack);
 *     assertTrue(ccnxJson_Equals(config, testValue), "Wrong value");
 *     rtaCommandCreateProtocolStack_Release(&createStack);
 *
 *     ccnxStackConfig_Release(&config);
 * }
 * @endcode
 */
PARCJSON *rtaCommandCreateProtocolStack_GetConfig(const RtaCommandCreateProtocolStack *createStack);

/**
 * Derive an explanation for why a RtaCommandCreateProtocolStack instance is invalid.
 *
 * Returns either a nul-terminated C string containing a human-readable explanation,
 * or NULL which indicates the instance is valid.
 *
 * @param [in] instance A pointer to a `RtaCommandCreateProtocolStack` instance.
 *
 * @return NULL The instance is valid.
 * @return non-NULL A nul-terminated C string containing an explanation.
 *
 * Example:
 * @code
 * {
 *     RtaCommandCreateProtocolStack *instance = rtaCommandCreateProtocolStack_Create(...);
 *
 *     if (rtaCommandCreateProtocolStack_IsValid(instance)) {
 *         printf("Instance is valid.\n");
 *     }
 * }
 * @endcode
 */
const char *rtaCommandCreateProtocolStack_AssessValidity(const RtaCommandCreateProtocolStack *instance);

/**
 * Determine if an instance of `RtaCommandCreateProtocolStack` is valid.
 *
 * Valid means the internal state of the type is consistent with its required current or future behaviour.
 * This may include the validation of internal instances of types.
 *
 * @param [in] instance A pointer to a `RtaCommandCreateProtocolStack` instance.
 *
 * @return true The instance is valid.
 * @return false The instance is not valid.
 *
 * Example:
 * @code
 * {
 *     RtaCommandCreateProtocolStack *instance = rtaCommandCreateProtocolStack_Create(...);
 *
 *     if (rtaCommandCreateProtocolStack_IsValid(instance)) {
 *         printf("Instance is valid.\n");
 *     }
 * }
 * @endcode
 */
bool rtaCommandCreateProtocolStack_IsValid(const RtaCommandCreateProtocolStack *instance);

/**
 * Assert that the given `RtaCommandCreateProtocolStack` instance is valid.
 *
 * @param [in] instance A pointer to a valid RtaCommandCreateProtocolStack instance.
 *
 * Example:
 * @code
 * {
 *     RtaCommandCreateProtocolStack *a = rtaCommandCreateProtocolStack_Create();
 *
 *     rtaCommandCreateProtocolStack_AssertValid(a);
 *
 *     printf("Instance is valid.\n");
 *
 *     rtaCommandCreateProtocolStack_Release(&b);
 * }
 * @endcode
 */
void rtaCommandCreateProtocolStack_AssertValid(const RtaCommandCreateProtocolStack *instance);
#endif // Libccnx_rta_CommandCreateProtocolStack_h

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
 * @file rta_Command.h
 * @brief Wraps individual commands and is written to/from a Ring Buffer
 *
 * The RtaCommand is the common wrapper for all the specific command types.  It also supports functions to
 * write it to a PARCRingBuffer and read from a one.
 *
 * The ShutdownFramework command is a little different than all the other commands.  There are no parameters
 * to this command, so there is no separate type for it.  You can create an RtaCommand of this flavor and
 * check for it (rtaCommand_IsShutdownFramework), but there is no Get function.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#ifndef Libccnx_rta_Commands_h
#define Libccnx_rta_Commands_h

struct rta_command;
typedef struct rta_command RtaCommand;

#include <ccnx/transport/transport_rta/commands/rta_CommandCloseConnection.h>
#include <ccnx/transport/transport_rta/commands/rta_CommandCreateProtocolStack.h>
#include <ccnx/transport/transport_rta/commands/rta_CommandDestroyProtocolStack.h>
#include <ccnx/transport/transport_rta/commands/rta_CommandOpenConnection.h>
#include <ccnx/transport/transport_rta/commands/rta_CommandTransmitStatistics.h>

#include <parc/concurrent/parc_RingBuffer_1x1.h>


/**
 * Writes a command to a Ring Buffer
 *
 * Creates a reference to the command and puts the reference on the ring buffer.
 * The caller still owns their own reference to the command.
 *
 * This command does not involve a PARCNotifier.  If using a notifier in conjunction
 * with the ring buffer, the caller is reponsible for posting the notification after
 * all ther writes are done.
 *
 * The function will not block.  If the ring buffer is full, it will return false.
 *
 * @param [in] command The command to put (by reference) on the ring buffer.
 * @param [in] commandRingBuffer The ring buffer to use
 *
 * @return true A reference was put on the ring buffer
 * @return false Failed to put reference, likely because the ring buffer was full.
 *
 * Example:
 * @code
 * {
 *    RtaCommand *command = rtaCommand_CreateShutdownFramework();
 *
 *    bool success = rtaCommand_Write(command, ring);
 *    if (!success) {
 *       // return error to user that we're backlogged
 *    }
 *
 *    rtaCommand_Release(&command);
 * }
 * @endcode
 */
bool rtaCommand_Write(const RtaCommand *command, PARCRingBuffer1x1 *commandRingBuffer);

/**
 * Reads a command from a ring buffer
 *
 * If the buffer is empty, will return NULL.
 *
 * @param [in] commandRingBuffer The buffer to read
 *
 * @return non-null A valid command object
 * @return null Could not read a whole command object
 *
 * Example:
 * @code
 * {
 *    PARCRingBuffer1x1 *ring = parcRingBuffer1x1_Create(4, NULL);
 *    RtaCommand *command = rtaCommand_CreateShutdownFramework();
 *
 *    bool success = rtaCommand_Write(command, ring);
 *    assertTrue(success, "Failed to put command in to ring buffer");
 *
 *    // We should now have two references
 *    assertTrue(parcObject_GetReferenceCount(command) == 2, "Wrong refernce count, got %zu expected %zu", parcObject_GetReferenceCount(command), 2);
 *
 *    RtaCommand *test = rtaCommand_Read(ring);
 *    assertTrue(test == command, "Wrong pointers, got %p expected %p", (void *) test, (void *) command);
 *
 *    rtaCommand_Release(&command);
 *    rtaCommand_Release(&test);
 *    parcRingBuffer1x1_Release(&ring);
 * }
 * @endcode
 */
RtaCommand *rtaCommand_Read(PARCRingBuffer1x1 *commandRingBuffer);

/**
 * Increase the number of references to a `RtaCommand`.
 *
 * Note that new `RtaCommand` is not created,
 * only that the given `RtaCommand` reference count is incremented.
 * Discard the reference by invoking `rtaCommand_Release`.
 *
 * @param [in] command The RtaCommand to reference.
 *
 * @return non-null A reference to `command`.
 * @return null An error
 *
 * Example:
 * @code
 * {
 *    RtaCommand *command = rtaCommand_CreateShutdownFramework();
 *    RtaCommand *second = rtaCommand_Acquire(command);
 *
 *    // release order does not matter
 *    rtaCommand_Release(&command);
 *    rtaCommand_Release(&second);
 * }
 * @endcode
 */
RtaCommand *rtaCommand_Acquire(const RtaCommand *command);

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
 * @param [in,out] commandPtr A pointer to the object to release, will return NULL'd.
 *
 * Example:
 * @code
 * {
 *    RtaCommand *command = rtaCommand_CreateShutdownFramework();
 *    RtaCommand *second = rtaCommand_Acquire(command);
 *
 *    // release order does not matter
 *    rtaCommand_Release(&command);
 *    rtaCommand_Release(&second);
 * }
 * @endcode
 */
void rtaCommand_Release(RtaCommand **commandPtr);

/**
 * Print a human readable representation of the given `RtaCommand`.
 *
 * @param [in] command A pointer to the instance to display.
 * @param [in] indentation The level of indentation to use to pretty-print the output.
 *
 * Example:
 * @code
 * {
 *    RtaCommand *command = rtaCommand_CreateShutdownFramework();
 *    rtaCommand_Display(command, 0);
 *    rtaCommand_Release(&command);
 * }
 * @endcode
 *
 */
void rtaCommand_Display(const RtaCommand *command, int indentation);

// ======================
// CLOSE CONNECTION


/**
 * Tests if the RtaCommand is of type CloseConnection
 *
 * Tests if the RtaCommand is of type CloseConnection.  This will also assert the
 * RtaCommand invariants, so the RtaCommand object must be a properly constructed object.
 *
 * @param [in] command An allocated RtaCommand ojbect
 *
 * @return true The object is of type CloseConnection
 * @return false The object is of some other type
 *
 * Example:
 * @code
 * {
 *    RtaCommandCloseConnection *closeConnection = rtaCommandCloseConnection_Create(77);
 *    RtaCommand *command = rtaCommand_CreateCloseConnection(closeConnection);
 *    assertTrue(rtaCommand_IsCloseConnection(command), "Command is not CloseConnection");
 *    rtaCommand_Release(&command);
 *    rtaCommandCloseConnection_Release(&closeConnection);
 * }
 * @endcode
 */
bool rtaCommand_IsCloseConnection(const RtaCommand *command);

/**
 * Allocates and creates an RtaCommand object from a RtaCommandCloseConnection
 *
 * Allocates and creates an RtaCommand object from a RtaCommandCloseConnection
 * by acquiring a reference to it and storing it in the RtaCommand.  The caller
 * may release their reference to `close` at any time.
 *
 * @param [in] close The specific command to make acquire a reference from.
 *
 * @return non-null A properly allocated and configured RtaCommand.
 * @return null An error.
 *
 * Example:
 * @code
 * {
 *    RtaCommandCloseConnection *closeConnection = rtaCommandCloseConnection_Create(77);
 *    RtaCommand *command = rtaCommand_CreateCloseConnection(closeConnection);
 *
 *    // release order does not matter
 *    rtaCommand_Release(&command);
 *    rtaCommandCloseConnection_Release(&closeConnection);
 * }
 * @endcode
 */
RtaCommand *rtaCommand_CreateCloseConnection(const RtaCommandCloseConnection *close);

/**
 * Returns the internal RtaCommandCloseConnection object
 *
 * Returns the internal RtaCommandCloseConnection object, the user should not release it.
 * The the RtaCommand is not of type CloseConnection, it will assert in its validation.
 *
 * @param [in] command The RtaCommand to query for the object.
 *
 * @return The RtaCommandCloseConnection object that constructed the RtaCommand.
 *
 * Example:
 * @code
 *    RtaCommandCloseConnection *closeConnection = rtaCommandCloseConnection_Create(77);
 *    RtaCommand *command = rtaCommand_CreateCloseConnection(closeConnection);
 *
 *    const RtaCommandCloseConnection *testValue = rtaCommand_GetCloseConnection(command);
 *    assertTrue(testValue == closeConnection, "Wrong pointer returned");
 *
 *    rtaCommand_Release(&command);
 *    rtaCommandCloseConnection_Release(&closeConnection);
 * @endcode
 */
const RtaCommandCloseConnection *rtaCommand_GetCloseConnection(const RtaCommand *command);

// ======================
// OPEN CONNECTION

/**
 * Tests if the RtaCommand is of type OpenConnection
 *
 * Tests if the RtaCommand is of type OpenConnection.  This will also assert the
 * RtaCommand invariants, so the RtaCommand object must be a properly constructed object.
 *
 * @param [in] command An allocated RtaCommand ojbect
 *
 * @return true The object is of type OpenConnection
 * @return false The object is of some other type
 *
 * Example:
 * @code
 * {
 *    RtaCommandOpenConnection *openConnection = rtaCommandOpenConnection_Create(111, 2341, 2450987, NULL);
 *    RtaCommand *command = rtaCommand_CreateOpenConnection(openConnection);
 *    assertTrue(rtaCommand_IsOpenConnection(command), "Command is not OpenConnection");
 *    rtaCommand_Release(&command);
 *    rtaCommandOpenConnection_Release(&openConnection);
 * }
 * @endcode
 */
bool rtaCommand_IsOpenConnection(const RtaCommand *command);

/**
 * Allocates and creates an RtaCommand object from a RtaCommandOpenConnection
 *
 * Allocates and creates an RtaCommand object from a RtaCommandOpenConnection
 * by acquiring a reference to it and storing it in the RtaCommand.  The caller
 * may release their reference to `open` at any time.
 *
 * @param [in] open The specific command to make acquire a reference from.
 *
 * @return non-null A properly allocated and configured RtaCommand.
 * @return null An error.
 *
 * Example:
 * @code
 * {
 *    RtaCommandOpenConnection *openConnection = rtaCommandOpenConnection_Create(111, 2341, 2450987, NULL);
 *    RtaCommand *command = rtaCommand_CreateOpenConnection(openConnection);
 *
 *    // release order does not matter
 *    rtaCommand_Release(&command);
 *    rtaCommandOpenConnection_Release(&openConnection);
 * }
 * @endcode
 */
RtaCommand *rtaCommand_CreateOpenConnection(const RtaCommandOpenConnection *open);

/**
 * Returns the internal RtaCommandOpenConnection object
 *
 * Returns the internal RtaCommandOpenConnection object, the user should not release it.
 * The the RtaCommand is not of type CloseConnection, it will assert in its validation.
 *
 * @param [in] command The RtaCommand to query for the object.
 *
 * @return The RtaCommandOpenConnection object that constructed the RtaCommand.
 *
 * Example:
 * @code
 *    RtaCommandOpenConnection *openConnection = rtaCommandOpenConnection_Create(111, 2341, 2450987, NULL);
 *    RtaCommand *command = rtaCommand_CreateOpenConnection(openConnection);
 *
 *    const RtaCommandOpenConnection *testValue = rtaCommand_GetOpenConnection(command);
 *    assertTrue(testValue == openConnection, "Wrong pointer returned");
 *
 *    rtaCommand_Release(&command);
 *    rtaCommandOpenConnection_Release(&openConnection);
 * @endcode
 */
const RtaCommandOpenConnection *rtaCommand_GetOpenConnection(const RtaCommand *command);

// ======================
// CREATE STACK

/**
 * Tests if the RtaCommand is of type CreateProtocolStack
 *
 * Tests if the RtaCommand is of type CreateProtocolStack.  This will also assert the
 * RtaCommand invariants, so the RtaCommand object must be a properly constructed object.
 *
 * @param [in] command An allocated RtaCommand ojbect
 *
 * @return true The object is of type CreateProtocolStack
 * @return false The object is of some other type
 *
 * Example:
 * @code
 * {
 *    CCNxStackConfig *config = ccnxStackConfig_Create();
 *    RtaCommandCreateProtocolStack *createStack = rtaCommandCreateProtocolStack_Create(111, config);
 *    RtaCommand *command = rtaCommand_CreateCreateProtocolStack(createStack);
 *    assertTrue(rtaCommand_IsCreateProtocolStack(command), "Command is not CreateProtocolStack");
 *    rtaCommand_Release(&command);
 *    rtaCommandCreateProtocolStack_Release(&createStack);
 *    ccnxStackConfig_Release(&config);
 * }
 * @endcode
 */
bool rtaCommand_IsCreateProtocolStack(const RtaCommand *command);

/**
 * Allocates and creates an RtaCommand object from a RtaCommandCreateProtocolStack
 *
 * Allocates and creates an RtaCommand object from a RtaCommandCreateProtocolStack
 * by acquiring a reference to it and storing it in the RtaCommand.  The caller
 * may release their reference to `createStack` at any time.
 *
 * @param [in] createStack The specific command to make acquire a reference from.
 *
 * @return non-null A properly allocated and configured RtaCommand.
 * @return null An error.
 *
 * Example:
 * @code
 * {
 *    CCNxStackConfig *config = ccnxStackConfig_Create();
 *    RtaCommandCreateProtocolStack *createStack = rtaCommandCreateProtocolStack_Create(111, config);
 *    RtaCommand *command = rtaCommand_CreateCreateProtocolStack(createStack);
 *
 *    // release order does not matter
 *    rtaCommand_Release(&command);
 *    rtaCommandCreateProtocolStack_Release(&createStack);
 *    ccnxStackConfig_Release(&config);
 * }
 * @endcode
 */
RtaCommand *rtaCommand_CreateCreateProtocolStack(const RtaCommandCreateProtocolStack *createStack);

/**
 * Returns the internal RtaCommandCreateProtocolStack object
 *
 * Returns the internal RtaCommandCreateProtocolStack object, the user should not release it.
 * The the RtaCommand is not of type CloseConnection, it will assert in its validation.
 *
 * @param [in] command The RtaCommand to query for the object.
 *
 * @return The RtaCommandCreateProtocolStack object that constructed the RtaCommand.
 *
 * Example:
 * @code
 * {
 *    CCNxStackConfig *config = ccnxStackConfig_Create();
 *    RtaCommandCreateProtocolStack *createStack = rtaCommandCreateProtocolStack_Create(111, config);
 *    RtaCommand *command = rtaCommand_CreateCreateProtocolStack(createStack);
 *
 *    const RtaCommandOpenConnection *testValue = rtaCommand_GetOpenConnection(command);
 *    assertTrue(testValue == createStack, "Wrong pointer returned");
 *
 *    rtaCommand_Release(&command);
 *    rtaCommandCreateProtocolStack_Release(&createStack);
 *    ccnxStackConfig_Release(&config);
 * }
 * @endcode
 */
const RtaCommandCreateProtocolStack *rtaCommand_GetCreateProtocolStack(const RtaCommand *command);

// ======================
// DESTROY STACK

/**
 * Tests if the RtaCommand is of type DestroyProtocolStack
 *
 * Tests if the RtaCommand is of type DestroyProtocolStack.  This will also assert the
 * RtaCommand invariants, so the RtaCommand object must be a properly constructed object.
 *
 * @param [in] command An allocated RtaCommand ojbect
 *
 * @return true The object is of type DestroyProtocolStack
 * @return false The object is of some other type
 *
 * Example:
 * @code
 * {
 *    RtaCommandDestroyProtocolStack *destroyStack = rtaCommandDestroyProtocolStack_Create(77);
 *    RtaCommand *command = rtaCommand_CreateDestroyProtocolStack(destroyStack);
 *    assertTrue(rtaCommand_IsDestroyProtocolStack(command), "Command is not DestroyProtocolStack");
 *    rtaCommand_Release(&command);
 *    rtaCommandDestroyProtocolStack_Release(&destroyStack);
 * }
 * @endcode
 */
bool rtaCommand_IsDestroyProtocolStack(const RtaCommand *command);

/**
 * Allocates and creates an RtaCommand object from a RtaCommandDestroyProtocolStack
 *
 * Allocates and creates an RtaCommand object from a RtaCommandDestroyProtocolStack
 * by acquiring a reference to it and storing it in the RtaCommand.  The caller
 * may release their reference to `destroyStack` at any time.
 *
 * @param [in] destroyStack The specific command to make acquire a reference from.
 *
 * @return non-null A properly allocated and configured RtaCommand.
 * @return null An error.
 *
 * Example:
 * @code
 * {
 *    RtaCommandDestroyProtocolStack *destroyStack = rtaCommandDestroyProtocolStack_Create(77);
 *    RtaCommand *command = rtaCommand_CreateDestroyProtocolStack(destroyStack);
 *
 *    // release order does not matter
 *    rtaCommand_Release(&command);
 *    rtaCommandDestroyProtocolStack_Release(&destroyStack);
 * }
 * @endcode
 */
RtaCommand *rtaCommand_CreateDestroyProtocolStack(const RtaCommandDestroyProtocolStack *destroyStack);

/**
 * Returns the internal RtaCommandDestroyProtocolStack object
 *
 * Returns the internal RtaCommandDestroyProtocolStack object, the user should not release it.
 * The the RtaCommand is not of type CloseConnection, it will assert in its validation.
 *
 * @param [in] command The RtaCommand to query for the object.
 *
 * @return The RtaCommandDestroyProtocolStack object that constructed the RtaCommand.
 *
 * Example:
 * @code
 *    RtaCommandDestroyProtocolStack *destroyStack = rtaCommandDestroyProtocolStack_Create(77);
 *    RtaCommand *command = rtaCommand_CreateDestroyProtocolStack(destroyStack);
 *
 *    const RtaCommandDestroyProtocolStack *testValue = rtaCommand_GetOpenConnection(command);
 *    assertTrue(testValue == destroyStack, "Wrong pointer returned");
 *
 *    rtaCommand_Release(&command);
 *    rtaCommandDestroyProtocolStack_Release(&destroyStack);
 * @endcode
 */
const RtaCommandDestroyProtocolStack *rtaCommand_GetDestroyProtocolStack(const RtaCommand *command);

// ======================
// SHUTDOWN FRAMEWORK

/**
 * Tests if the RtaCommand is of type ShutdownFramework
 *
 * Tests if the RtaCommand is of type ShutdownFramework.  This will also assert the
 * RtaCommand invariants, so the RtaCommand object must be a properly constructed object.
 *
 * The ShutdownFramework command has no parameters, so there is no rtaCommand_GetShutdownFramework() function.
 *
 * @param [in] command An allocated RtaCommand ojbect
 *
 * @return true The object is of type ShutdownFramework
 * @return false The object is of some other type
 *
 * Example:
 * @code
 * {
 *    RtaCommand *command = rtaCommand_CreateShutdownFramework();
 *    assertTrue(rtaCommand_IsShutdownFramework(command), "Command is not shutdown framework");
 *    rtaCommand_Release(&command);
 * }
 * @endcode
 */
bool rtaCommand_IsShutdownFramework(const RtaCommand *command);

/**
 * Allocates and creates an RtaCommand object of type ShutdownFramework.
 *
 * Allocates and creates an RtaCommand object of type ShutdownFramework.
 * There are no parameters to ShutdownFramework, so there is no underlying type.
 *
 * @return non-null A properly allocated and configured RtaCommand.
 * @return null An error.
 *
 * Example:
 * @code
 * {
 *    RtaCommand *command = rtaCommand_CreateShutdownFramework();
 *    rtaCommand_Release(&command);
 * }
 * @endcode
 */
RtaCommand *rtaCommand_CreateShutdownFramework(void);

// ======================
// TRANSMIT STATS

/**
 * Tests if the RtaCommand is of type TransmitStatistics
 *
 * Tests if the RtaCommand is of type TransmitStatistics.  This will also assert the
 * RtaCommand invariants, so the RtaCommand object must be a properly constructed object.
 *
 * @param [in] command An allocated RtaCommand ojbect
 *
 * @return true The object is of type TransmitStatistics
 * @return false The object is of some other type
 *
 * Example:
 * @code
 * {
 *    RtaCommandTransmitStatistics *transmitStats = rtaCommandTransmitStatistics_Create((struct timeval) { 1, 2 }, "filename");
 *    RtaCommand *command = rtaCommand_CreateTransmitStatistics(transmitStats);
 *    assertTrue(rtaCommand_IsTransmitStatistics(command), "Command is not TransmitStatistics");
 *    rtaCommand_Release(&command);
 *    rtaCommandTransmitStatistics_Release(&transmitStats);
 * }
 * @endcode
 */
bool rtaCommand_IsTransmitStatistics(const RtaCommand *command);

/**
 * Allocates and creates an RtaCommand object from a RtaCommandTransmitStatistics
 *
 * Allocates and creates an RtaCommand object from a RtaCommandTransmitStatistics
 * by acquiring a reference to it and storing it in the RtaCommand.  The caller
 * may release their reference to `transmitStats` at any time.
 *
 * @param [in] transmitStats The specific command to make acquire a reference from.
 *
 * @return non-null A properly allocated and configured RtaCommand.
 * @return null An error.
 *
 * Example:
 * @code
 * {
 *    RtaCommandTransmitStatistics *transmitStats = rtaCommandTransmitStatistics_Create((struct timeval) { 1, 2 }, "filename");
 *    RtaCommand *command = rtaCommand_CreateTransmitStatistics(transmitStats);
 *
 *    // release order does not matter
 *    rtaCommand_Release(&command);
 *    rtaCommandTransmitStatistics_Release(&transmitStats);
 * }
 * @endcode
 */
RtaCommand *rtaCommand_CreateTransmitStatistics(const RtaCommandTransmitStatistics *transmitStats);

/**
 * Returns the internal RtaCommandTransmitStatistics object
 *
 * Returns the internal RtaCommandTransmitStatistics object, the user should not release it.
 * The the RtaCommand is not of type CloseConnection, it will assert in its validation.
 *
 * @param [in] command The RtaCommand to query for the object.
 *
 * @return The RtaCommandTransmitStatistics object that constructed the RtaCommand.
 *
 * Example:
 * @code
 *    RtaCommandTransmitStatistics *transmitStats = rtaCommandTransmitStatistics_Create((struct timeval) { 1, 2 }, "filename");
 *    RtaCommand *command = rtaCommand_CreateTransmitStatistics(transmitStats);
 *
 *    const RtaCommandDestroyProtocolStack *testValue = rtaCommand_GetOpenConnection(command);
 *    assertTrue(testValue == destroyStack, "Wrong pointer returned");
 *
 *    rtaCommand_Release(&command);
 *    rtaCommandTransmitStatistics_Release(&transmitStats);
 * @endcode
 */
const RtaCommandTransmitStatistics *rtaCommand_GetTransmitStatistics(const RtaCommand *command);
#endif // Libccnx_rta_Commands_h

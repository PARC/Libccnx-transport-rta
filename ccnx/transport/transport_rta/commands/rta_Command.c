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
 * Implements the single wrapper for commands sent over the command channel
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>

#include <LongBow/runtime.h>

#include <stdio.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Object.h>
#include <parc/algol/parc_DisplayIndented.h>

#include <ccnx/transport/transport_rta/commands/rta_Command.h>

/*
 * Internal definition of command types
 */
typedef enum {
    RtaCommandType_Unknown = 0,
    RtaCommandType_CreateProtocolStack,
    RtaCommandType_OpenConnection,
    RtaCommandType_CloseConnection,
    RtaCommandType_DestroyProtocolStack,
    RtaCommandType_ShutdownFramework,
    RtaCommandType_TransmitStatistics,
    RtaCommandType_Last
} _RtaCommandType;

struct rta_command {
    _RtaCommandType type;

    union {
        RtaCommandCloseConnection *closeConnection;
        RtaCommandOpenConnection *openConnection;
        RtaCommandCreateProtocolStack *createStack;
        RtaCommandDestroyProtocolStack *destroyStack;
        RtaCommandTransmitStatistics *transmitStats;

        // shutdown framework has no value it will be NULL
        // Statistics has no value
        void *noValue;
    } value;
};

static struct commandtype_to_string {
    _RtaCommandType type;
    const char *string;
} _RtaCommandTypeToString[] = {
    { .type = RtaCommandType_Unknown,              .string = "Unknown"              },
    { .type = RtaCommandType_CreateProtocolStack,  .string = "CreateProtocolStack"  },
    { .type = RtaCommandType_OpenConnection,       .string = "OpenConnection"       },
    { .type = RtaCommandType_CloseConnection,      .string = "CloseConnection"      },
    { .type = RtaCommandType_DestroyProtocolStack, .string = "DestroyProtocolStack" },
    { .type = RtaCommandType_ShutdownFramework,    .string = "ShutdownFramework"    },
    { .type = RtaCommandType_TransmitStatistics,   .string = "TransmitStatistics"   },
    { .type = RtaCommandType_Last,                 .string = NULL                   },
};

// ===============================
// Internal API

static void
_rtaCommand_Destroy(RtaCommand **commandPtr)
{
    RtaCommand *command = *commandPtr;
    switch (command->type) {
        case RtaCommandType_ShutdownFramework:
            // no inner-release needed
            break;

        case RtaCommandType_CreateProtocolStack:
            rtaCommandCreateProtocolStack_Release(&command->value.createStack);
            break;

        case RtaCommandType_OpenConnection:
            rtaCommandOpenConnection_Release(&command->value.openConnection);
            break;

        case RtaCommandType_CloseConnection:
            rtaCommandCloseConnection_Release(&command->value.closeConnection);
            break;

        case RtaCommandType_DestroyProtocolStack:
            rtaCommandDestroyProtocolStack_Release(&command->value.destroyStack);
            break;

        case RtaCommandType_TransmitStatistics:
            rtaCommandTransmitStatistics_Release(&command->value.transmitStats);
            break;

        default:
            trapIllegalValue(command->type, "Illegal command type %d", command->type);
            break;
    }
}

#ifdef Transport_DISABLE_VALIDATION
#  define _rtaCommand_OptionalAssertValid(_instance_)
#else
#  define _rtaCommand_OptionalAssertValid(_instance_) _rtaCommand_AssertValid(_instance_)
#endif

static void
_rtaCommand_AssertValid(const RtaCommand *command)
{
    assertNotNull(command, "RtaCommand must be non-null");
    assertTrue(RtaCommandType_Unknown < command->type && command->type < RtaCommandType_Last,
               "Invalid RtaCommand type, must be %d < type %d < %d",
               RtaCommandType_Unknown,
               command->type,
               RtaCommandType_Last);

    switch (command->type) {
        case RtaCommandType_ShutdownFramework:
            assertNull(command->value.noValue, "RtaCommand value must be null for ShutdownFramework or Statistics");
            break;

        case RtaCommandType_CreateProtocolStack:
            assertNotNull(command->value.createStack, "RtaCommand createStack member must be non-null");
            break;

        case RtaCommandType_OpenConnection:
            assertNotNull(command->value.openConnection, "RtaCommand openConnection member must be non-null");
            break;

        case RtaCommandType_CloseConnection:
            assertNotNull(command->value.closeConnection, "RtaCommand closeConnection member must be non-null");
            break;

        case RtaCommandType_DestroyProtocolStack:
            assertNotNull(command->value.destroyStack, "RtaCommand destroyStack member must be non-null");
            break;

        case RtaCommandType_TransmitStatistics:
            assertNotNull(command->value.transmitStats, "RtaCommand transmitStats member must be non-null");
            break;

        default:
            trapIllegalValue(command->type, "Illegal command type %d", command->type);
            break;
    }
}

parcObject_ExtendPARCObject(RtaCommand, _rtaCommand_Destroy, NULL, NULL, NULL, NULL, NULL, NULL);

parcObject_ImplementAcquire(rtaCommand, RtaCommand);

parcObject_ImplementRelease(rtaCommand, RtaCommand);

static RtaCommand *
_rtaCommand_Allocate(_RtaCommandType type)
{
    RtaCommand *command = parcObject_CreateInstance(RtaCommand);
    command->type = type;
    return command;
}

static const char *
_rtaCommand_TypeToString(const RtaCommand *command)
{
    for (int i = 0; _RtaCommandTypeToString[i].string != NULL; i++) {
        if (_RtaCommandTypeToString[i].type == command->type) {
            return _RtaCommandTypeToString[i].string;
        }
    }
    trapUnexpectedState("Command is not a valid type: %d", command->type);
}

// ===============================
// Public API

void
rtaCommand_Display(const RtaCommand *command, int indentation)
{
    _rtaCommand_OptionalAssertValid(command);

    parcDisplayIndented_PrintLine(indentation, "RtaCommand type %s (%d) value pointer %p\n",
                                  _rtaCommand_TypeToString(command), command->type, command->value);
}

/*
 * Gets a reference to itself and puts it in the ring buffer
 */
bool
rtaCommand_Write(const RtaCommand *command, PARCRingBuffer1x1 *commandRingBuffer)
{
    _rtaCommand_OptionalAssertValid(command);

    RtaCommand *reference = rtaCommand_Acquire(command);

    bool addedToRingBuffer = parcRingBuffer1x1_Put(commandRingBuffer, reference);

    if (!addedToRingBuffer) {
        // it was not stored in the ring, so we need to be responsible and release it
        rtaCommand_Release(&reference);
    }

    return addedToRingBuffer;
}

RtaCommand *
rtaCommand_Read(PARCRingBuffer1x1 *commandRingBuffer)
{
    RtaCommand *referenceFromRing = NULL;

    bool fetchedReference = parcRingBuffer1x1_Get(commandRingBuffer, (void **) &referenceFromRing);
    if (fetchedReference) {
        return referenceFromRing;
    }
    return NULL;
}

// ======================
// CLOSE CONNECTION

bool
rtaCommand_IsCloseConnection(const RtaCommand *command)
{
    _rtaCommand_OptionalAssertValid(command);
    return (command->type == RtaCommandType_CloseConnection);
}

RtaCommand *
rtaCommand_CreateCloseConnection(const RtaCommandCloseConnection *close)
{
    RtaCommand *command = _rtaCommand_Allocate(RtaCommandType_CloseConnection);
    command->value.closeConnection = rtaCommandCloseConnection_Acquire(close);
    return command;
}

const RtaCommandCloseConnection *
rtaCommand_GetCloseConnection(const RtaCommand *command)
{
    assertTrue(rtaCommand_IsCloseConnection(command), "Command is not CloseConnection");
    return command->value.closeConnection;
}

// ======================
// OPEN CONNECTION

bool
rtaCommand_IsOpenConnection(const RtaCommand *command)
{
    _rtaCommand_OptionalAssertValid(command);
    return (command->type == RtaCommandType_OpenConnection);
}

RtaCommand *
rtaCommand_CreateOpenConnection(const RtaCommandOpenConnection *open)
{
    RtaCommand *command = _rtaCommand_Allocate(RtaCommandType_OpenConnection);
    command->value.openConnection = rtaCommandOpenConnection_Acquire(open);
    return command;
}

const RtaCommandOpenConnection *
rtaCommand_GetOpenConnection(const RtaCommand *command)
{
    assertTrue(rtaCommand_IsOpenConnection(command), "Command is not OpenConnection");
    return command->value.openConnection;
}

// ======================
// CREATE STACK

bool
rtaCommand_IsCreateProtocolStack(const RtaCommand *command)
{
    _rtaCommand_OptionalAssertValid(command);
    return (command->type == RtaCommandType_CreateProtocolStack);
}

RtaCommand *
rtaCommand_CreateCreateProtocolStack(const RtaCommandCreateProtocolStack *createStack)
{
    RtaCommand *command = _rtaCommand_Allocate(RtaCommandType_CreateProtocolStack);
    command->value.createStack = rtaCommandCreateProtocolStack_Acquire(createStack);
    return command;
}

const RtaCommandCreateProtocolStack *
rtaCommand_GetCreateProtocolStack(const RtaCommand *command)
{
    assertTrue(rtaCommand_IsCreateProtocolStack(command), "Command is not CreateProtocolStack");
    return command->value.createStack;
}

bool
rtaCommand_IsDestroyProtocolStack(const RtaCommand *command)
{
    _rtaCommand_OptionalAssertValid(command);
    return (command->type == RtaCommandType_DestroyProtocolStack);
}

RtaCommand *
rtaCommand_CreateDestroyProtocolStack(const RtaCommandDestroyProtocolStack *destroyStack)
{
    RtaCommand *command = _rtaCommand_Allocate(RtaCommandType_DestroyProtocolStack);
    command->value.destroyStack = rtaCommandDestroyProtocolStack_Acquire(destroyStack);
    return command;
}

const RtaCommandDestroyProtocolStack *
rtaCommand_GetDestroyProtocolStack(const RtaCommand *command)
{
    assertTrue(rtaCommand_IsDestroyProtocolStack(command), "Command is not DestroyProtocolStack");
    return command->value.destroyStack;
}

bool
rtaCommand_IsShutdownFramework(const RtaCommand *command)
{
    _rtaCommand_OptionalAssertValid(command);
    return (command->type == RtaCommandType_ShutdownFramework);
}

RtaCommand *
rtaCommand_CreateShutdownFramework(void)
{
    RtaCommand *command = _rtaCommand_Allocate(RtaCommandType_ShutdownFramework);
    command->value.noValue = NULL;
    return command;
}

// no getter

bool
rtaCommand_IsTransmitStatistics(const RtaCommand *command)
{
    _rtaCommand_OptionalAssertValid(command);
    return (command->type == RtaCommandType_TransmitStatistics);
}

RtaCommand *
rtaCommand_CreateTransmitStatistics(const RtaCommandTransmitStatistics *transmitStats)
{
    RtaCommand *command = _rtaCommand_Allocate(RtaCommandType_TransmitStatistics);
    command->value.transmitStats = rtaCommandTransmitStatistics_Acquire(transmitStats);
    return command;
}

const RtaCommandTransmitStatistics *
rtaCommand_GetTransmitStatistics(const RtaCommand *command)
{
    assertTrue(rtaCommand_IsTransmitStatistics(command), "Command is not TransmitStatistics");
    return command->value.transmitStats;
}

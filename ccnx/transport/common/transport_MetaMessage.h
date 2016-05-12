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
 * @file ccnx_MetaMessage.h
 * @brief A CCNx message suitable for sending through the CCNx Portal API.
 *
 * A CCNxMetaMessage encapsulates a CCN Interest, ContentObject, or Control message, and can
 * be read from and written to the CCNx Portal API.
 *
 * @author Glenn Scott, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#ifndef libccnx_ccnx_MetaMessage_h
#define libccnx_ccnx_MetaMessage_h

#include <ccnx/common/ccnx_Interest.h>
#include <ccnx/common/ccnx_InterestReturn.h>
#include <ccnx/common/ccnx_ContentObject.h>
#include <ccnx/common/ccnx_Manifest.h>
#include <ccnx/common/ccnx_WireFormatMessage.h>

#include <ccnx/common/internal/ccnx_TlvDictionary.h>

#include <ccnx/api/control/cpi_ControlMessage.h>

/**
 * @typedef CCNxMetaMessage
 * @brief A CCNxMetaMessage encapsulates a CCN Interest, ContentObject, or Control message.
 */
typedef CCNxTlvDictionary CCNxMetaMessage;


/**
 * Create a `CCNxMetaMessage` instance containing the given {@link CCNxInterest}.
 *
 * A new reference to the `CCNxInterest` is created.
 *
 * @param [in] interest A pointer to a valid `CCNxInterest` instance.
 *
 * @return NULL The `CCNxInterest` is not valid, or memory could not be allocated.
 * @return non-NULL A pointer to a `CCNxMetaMessage` instance.
 *
 * Example:
 * @code
 * {
 *         CCNxMetaMessage *message = ccnxMetaMessage_CreateFromInterest(interest);
 * }
 * @endcode
 */
CCNxMetaMessage *ccnxMetaMessage_CreateFromInterest(const CCNxInterest *interest);

/**
 * Create a `CCNxMetaMessage` instance containing the given {@link CCNxContentObject}.
 *
 * A new reference to the `CCNxContentObject` is created.
 *
 * @param [in] contentObject A pointer to a valid `CCNxContentObject` instance.
 *
 * @return NULL The `CCNxContentObject` is not valid, or memory could not be allocated.
 * @return non-NULL A pointer to a `CCNxMetaMessage` instance.
 *
 * Example:
 * @code
 * {
 *         CCNxMetaMessage *message = ccnxMetaMessage_CreateFromContentObject(contentObject);
 * }
 * @endcode
 */
CCNxMetaMessage *ccnxMetaMessage_CreateFromContentObject(const CCNxContentObject *contentObject);

/**
 * Create a `CCNxMetaMessage` instance containing the given {@link CCNxControl}.
 *
 * A new reference to the `CCNxControl` is created.
 *
 * @param [in] control A pointer to a valid `CCNxControl` instance.
 *
 * @return NULL The `CCNxControl` is not valid, or memory could not be allocated.
 * @return non-NULL A pointer to a `CCNxMetaMessage` instance.
 *
 * Example:
 * @code
 * {
 *         CCNxMetaMessage *message = ccnxMetaMessage_CreateFromControl(control);
 * }
 * @endcode
 */
CCNxMetaMessage *ccnxMetaMessage_CreateFromControl(const CCNxControl *control);

/**
 * Create a `CCNxMetaMessage` instance containing the given {@link CCNxManifest}.
 *
 * A new reference to the `CCNxManifest` is created.
 *
 * @param [in] control A pointer to a valid `CCNxManifest` instance.
 *
 * @return NULL The `CCNxManifest` is not valid, or memory could not be allocated.
 * @return non-NULL A pointer to a `CCNxMetaMessage` instance.
 *
 * Example:
 * @code
 * {
 *         CCNxMetaMessage *message = ccnxMetaMessage_CreateFromManifest(manifest);
 * }
 * @endcode
 */
CCNxMetaMessage *ccnxMetaMessage_CreateFromManifest(const CCNxManifest *manifest);

/**
 * Print a human readable representation of the given `CCNxMetaMessage` instance.
 *
 * @param [in] message A pointer to the `CCNxMetaMessage` to display.
 * @param [in] indentation The level of indentation to use to pretty-print the output.
 *
 * Example:
 * @code
 * {
 *     CCNxMetaMessage *message = ccnxMetaMessage_CreateFromInterest(...);
 *
 *     ccnxMetaMessage_Display(message, 0);
 *
 *     ccnxMetaMessage_Release(&message);
 * }
 * @endcode
 *
 */
void ccnxMetaMessage_Display(const CCNxMetaMessage *message, int indentation);

/**
 * Increase the number of references to a `CCNxMetaMessage`.
 *
 * Note that new `CCNxMetaMessage` is not created,
 * only that the given `CCNxMetaMessage` reference count is incremented.
 * Discard the reference by invoking {@link ccnxMetaMessage_Release}().
 *
 * @param [in] instance A pointer to a `CCNxMetaMessage` instance.
 * @return The input @p instance.
 *
 * Example:
 * @code
 * {
 *     CCNxMetaMessage *message = ccnxPortal_Receive(portal, CCNxStackTimeout_Never);
 *     CCNxMetaMessage *messageReference = ccnxMetaMessage_Acquire(message);
 *
 *     ccnxMetaMessage_Release(&message);
 *     ccnxMetaMessage_Release(&messageReference);
 * }
 * @endcode
 * @see `ccnxMetaMessage_Release`
 */
CCNxMetaMessage *ccnxMetaMessage_Acquire(const CCNxMetaMessage *instance);

/**
 * Determine whether a specified `CCNxMetaMessage` instance encapsulates an {@link CCNxInterest}.
 *
 * Returns `true` if the underlying message is a `CCNxInterest`.
 *
 * @param [in] message A pointer to a `CCNxMetaMessage` instance.
 *
 * @return `true` If the underlying message is a `CCNxInterest`.
 * @return `false` If the underlying message is not a `CCNxInterest`.
 *
 * Example:
 * @code
 * {
 *     CCNxMetaMessage *message = ccnxPortal_Receive(portal, CCNxStackTimeout_Never);
 *
 *     if (ccnxMetaMessage_IsInterest(message)) {
 *         CCNxInterest *interest = ccnxMetaMessage_GetInterest(message);
 *         ...
 *         ccnxInterest_Release(&interest);
 *     }
 *
 *     ccnxMetaMessage_Release(&message);
 * }
 * @endcode
 * @see CCNxInterest
 */
bool ccnxMetaMessage_IsInterest(const CCNxMetaMessage *message);

/**
 * Determine whether a specified `CCNxMetaMessage` instance encapsulates an {@link CCNxInterestReturn}.
 *
 * Returns `true` if the underlying message is a `CCNxInterestReturn`.
 *
 * @param [in] message A pointer to a `CCNxMetaMessage` instance.
 *
 * @return `true` If the underlying message is a `CCNxInterestReturn`.
 * @return `false` If the underlying message is not a `CCNxInterestReturn`.
 *
 * Example:
 * @code
 * {
 *     CCNxMetaMessage *message = ccnxPortal_Receive(portal, CCNxStackTimeout_Never);
 *
 *     if (ccnxMetaMessage_IsInterestReturn(message)) {
 *         CCNxInterestReturn *interestReturn = ccnxMetaMessage_GetInterestReturn(message);
 *
 *         ...
 *         ccnxInterestReturn_Release(&interestReturn);
 *     }
 *
 *     ccnxMetaMessage_Release(&message);
 * }
 * @endcode
 * @see CCNxInterestReturn
 */
bool ccnxMetaMessage_IsInterestReturn(const CCNxMetaMessage *message);

/**
 * Determine whether a specified `CCNxMetaMessage` instance encapsulates a {@link CCNxContentObject}.
 *
 * Returns true if the underlying message is a `CCNxContentObject`.
 *
 * @param [in] message A pointer to a `CCNxMetaMessage` instance.
 *
 * @return `true` If the underlying message is a `CCNxContentObject`.
 * @return `false` If the underlying message is not a `CCNxContentObject`.
 *
 * Example:
 * @code
 * {
 *     CCNxMetaMessage *message = ccnxPortal_Receive(portal, CCNxStackTimeout_Never);
 *
 *     if (ccnxMetaMessage_IsContentObject(message)) {
 *         CCNxContentObject *contentObject = ccnxMetaMessage_GetContentObject(message);
 *         ...
 *         ccnxContentObject_Release(&contentObject);
 *     }
 *
 *     ccnxMetaMessage_Release(&message);
 * }
 * @endcode
 * @see `CCNxContentObject`
 */
bool ccnxMetaMessage_IsContentObject(const CCNxMetaMessage *message);

/**
 * Determine whether a specified `CCNxMetaMessage` instance encapsulates a {@link CCNxControl}.
 *
 * Returns true if the underlying message is a `CCNxControl`.
 *
 * @param [in] message A pointer to a `CCNxMetaMessage` instance.
 *
 * @return `true` If the underlying message is a `CCNxControl`.
 * @return `false` If the underlying message is not a `CCNxControl`.
 *
 * Example:
 * @code
 * {
 *     CCNxMetaMessage *message = ccnxPortal_Receive(portal, CCNxStackTimeout_Never);
 *
 *     if (ccnxMetaMessage_IsControl(message)) {
 *         CCNxControl *control = ccnxMetaMessage_GetControl(message);
 *         ...
 *         ccnxControl_Release(&control);
 *     }
 *
 *     ccnxMetaMessage_Release(&message);
 * }
 * @endcode
 * @see `CCNxControl`
 */
bool ccnxMetaMessage_IsControl(const CCNxMetaMessage *message);

/**
 * Determine whether a specified `CCNxMetaMessage` instance encapsulates a {@link CCNxManifest}.
 *
 * Returns true if the underlying message is a `CCNxManifest`.
 *
 * @param [in] message A pointer to a `CCNxMetaMessage` instance.
 *
 * @return `true` If the underlying message is a `CCNxManifest`.
 * @return `false` If the underlying message is not a `CCNxManifest`.
 *
 * Example:
 * @code
 * {
 *     CCNxMetaMessage *message = ccnxPortal_Receive(portal, CCNxStackTimeout_Never);
 *
 *     if (ccnxMetaMessage_IsManifest(message)) {
 *         CCNxManifest *control = ccnxMetaMessage_GetManifest(message);
 *         ...
 *         ccnxManifest_Release(&manifest);
 *     }
 *
 *     ccnxMetaMessage_Release(&message);
 * }
 * @endcode
 * @see `CCNxManifest`
 */
bool ccnxMetaMessage_IsManifest(const CCNxMetaMessage *message);

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
 * @param [in,out] messagePtr A pointer to a pointer to the instance to release.
 *
 * Example:
 * @code
 * {
 *     CCNxMetaMessage *message = ccnxPortal_Receive(portal, CCNxStackTimeout_Never);
 *     CCNxMetaMessage *messageReference = ccnxMetaMessage_Acquire(message);
 *
 *     ccnxMetaMessage_Release(&message);
 *     ccnxMetaMessage_Release(&messageReference);
 * }
 * @endcode
 * @see {@link ccnxMetaMessage_Acquire}
 */
void ccnxMetaMessage_Release(CCNxMetaMessage **messagePtr);

/**
 * Return a new {@link CCNxContentObject} instance created from the `CCNxMetaMessage`.
 *
 * The newly created `CCNxContentObject` instance must eventually be released by calling {@link ccnxContentObject_Release}().
 *
 * @param [in] message A pointer to a `CCNxMetaMessage` instance.
 *
 * @return A new `CCNxContentObject` instance, which must eventually be released by calling `ccnxContentObject_Release()`.
 *
 * Example:
 * @code
 * {
 *     CCNxMetaMessage *message = ccnxPortal_Receive(portal, CCNxStackTimeout_Never);
 *
 *     if (ccnxMetaMessage_IsContentObject(message)) {
 *         CCNxContentObject *contentObject = ccnxMetaMessage_GetContentObject(message);
 *         ...
 *         ccnxContentObject_Release(&contentObject);
 *     }
 *
 *     ccnxMetaMessage_Release(&message);
 * }
 * @endcode
 *
 * @see `ccnxContentObject_Release`
 * @see {@link ccnxMetaMessage_IsContentObject}
 */
CCNxContentObject *ccnxMetaMessage_GetContentObject(const CCNxMetaMessage *message);

/**
 * Return a new {@link CCNxInterest} instance created from the `CCNxMetaMessage`.
 *
 * The newly created `CCNxInterest}\` instance must eventually be released by calling {@link ccnxInterest_Release}().
 *
 * @param [in] message A pointer to a `CCNxMetaMessage` instance.
 *
 * @return A new `CCNxInterest` instance, which must eventually be released by calling `ccnxInterest_Release()`.
 *
 * Example:
 * @code
 * {
 *     CCNxMetaMessage *message = ccnxPortal_Receive(portal, CCNxStackTimeout_Never);
 *
 *     if (ccnxMetaMessage_IsInterest(message)) {
 *         CCNxInterest *interest = ccnxMetaMessage_GetInterest(message);
 *         ...
 *         ccnxInterest_Release(&interest);
 *     }
 *
 *     ccnxMetaMessage_Release(&message);
 * }
 * @endcode
 *
 * @see `ccnxInterest_Release`
 * @see {@link ccnxMetaMessage_IsInterest}
 */
CCNxInterest *ccnxMetaMessage_GetInterest(const CCNxMetaMessage *message);

/**
 * Return a new {@link CCNxInterestReturn} instance created from the `CCNxMetaMessage`.
 *
 * The newly created `CCNxInterestReturn}\` instance must eventually be released by calling {@link ccnxInterest_Release}().
 *
 * @param [in] message A pointer to a `CCNxMetaMessage` instance.
 *
 * @return A new `CCNxInterest` instance, which must eventually be released by calling `ccnxInterestReturn_Release()`.
 *
 * Example:
 * @code
 * {
 *     CCNxMetaMessage *message = ccnxPortal_Receive(portal, CCNxStackTimeout_Never);
 *
 *     if (ccnxMetaMessage_IsInterestReturn(message)) {
 *         CCNxInterestReturn *interestReturn = ccnxMetaMessage_GetInterestReturn(message);
 *
 *         ...
 *         ccnxInterestReturn_Release(&interestReturn);
 *     }
 *
 *     ccnxMetaMessage_Release(&message);
 * }
 * @endcode
 *
 * @see `ccnxInterest_Release`
 * @see {@link ccnxMetaMessage_IsInterest}
 */
CCNxInterest *ccnxMetaMessage_GetInterestReturn(const CCNxMetaMessage *message);

/**
 * Return a new {@link CCNxControl} instance created from the `CCNxMetaMessage`.
 *
 * The newly created `CCNxControl` instance must eventually be released by calling {@link ccnxControl_Release}().
 *
 * @param [in] message A pointer to a `CCNxMetaMessage` instance.
 *
 * @return A new `CCNxControl` instance, which must eventually be released by calling `ccnxControl_Release()`.
 *
 * Example:
 * @code
 * {
 *     CCNxMetaMessage *message = ccnxPortal_Receive(portal, CCNxStackTimeout_Never);
 *
 *     if (ccnxMetaMessage_IsControl(message)) {
 *         CCNxControl *control = ccnxMetaMessage_GetControl(message);
 *         ...
 *         ccnxControl_Release(&control);
 *     }
 *
 *     ccnxMetaMessage_Release(&message);
 * }
 * @endcode
 *
 * @see `ccnxControl_Release`
 * @see {@link ccnxMetaMessage_IsControl}
 */
CCNxControl *ccnxMetaMessage_GetControl(const CCNxMetaMessage *message);

/**
 * Return a new {@link CCNxManifest} instance created from the `CCNxMetaMessage`.
 *
 * The newly created `CCNxManifest` instance must eventually be released by calling {@link ccnxManifest_Release}().
 *
 * @param [in] message A pointer to a `CCNxMetaMessage` instance.
 *
 * @return A new `CCNxManifest` instance, which must eventually be released by calling `ccnxManifest_Release()`.
 *
 * Example:
 * @code
 * {
 *     CCNxMetaMessage *message = ccnxPortal_Receive(portal, CCNxStackTimeout_Never);
 *
 *     if (ccnxMetaMessage_IsManifest(message)) {
 *         CCNxManifest *manifest = ccnxMetaMessage_GetManifest(message);
 *         ...
 *         ccnxManifest_Release(&manifest);
 *     }
 *
 *     ccnxMetaMessage_Release(&message);
 * }
 * @endcode
 *
 * @see {@link ccnxManifest_Release}
 * @see {@link ccnxMetaMessage_IsManifest}
 */
CCNxManifest *ccnxMetaMessage_GetManifest(const CCNxMetaMessage *message);

/**
 * Return a new {@link CCNxMetaMessage} instance created from a wire format message
 *
 * The newly created `CCNxMetaMessage` instance must eventually be released by calling {@link CCNxMetaMessage_Release}().
 *
 * @param [in] message A pointer to a `PARCBuffer` instance containing a wire format message.
 *
 * @return A new `CCNxMetaMessage` instance, which must eventually be released by calling `ccnxMetaMessage_Release()`.
 *
 * Example:
 * @code
 * {
 * }
 * @endcode
 *
 */
CCNxMetaMessage *ccnxMetaMessage_CreateFromWireFormatBuffer(PARCBuffer *rawMessage);

/**
 * Return a new {@link PARCBuffer} instance containing an encodeded wire format message created
 * from the source `CCNxMetaMessage`.
 *
 * The newly created `PARCBuffer` instance must eventually be released by calling {@link parcBuffer_Release}().
 *
 * @param [in] message A pointer to a `CCNxMetaMessage` instance.
 * @param [in] signer A pointer to a `PARCSigner` instance.
 *
 * @return A new `PARCBuffer` instance, which must eventually be released by calling `parcBuffer_Release()`.
 *
 * Example:
 * @code
 * {
 * }
 * @endcode
 *
 */
PARCBuffer *ccnxMetaMessage_CreateWireFormatBuffer(CCNxMetaMessage *message, PARCSigner *signer);

#endif // libccnx_ccnx_MetaMessage_h

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
 * @file cpi_InterfaceIPTunnelList.h
 * @brief <#Brief Description#>
 *
 * <#Detailed Description#>
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2014, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#ifndef libccnx_cpi_InterfaceIPTunnelList_h
#define libccnx_cpi_InterfaceIPTunnelList_h

struct cpi_interface_iptunnel_list;
typedef struct cpi_interface_iptunnel_list CPIInterfaceIPTunnelList;

#include <ccnx/api/control/cpi_InterfaceIPTunnel.h>

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * {
 *     <#example#>
 * }
 * @endcode
 *
 * @see <#references#>
 */
CPIInterfaceIPTunnelList *cpiInterfaceIPTunnelList_Create(void);

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * {
 *     <#example#>
 * }
 * @endcode
 *
 * @see <#references#>
 */
void cpiInterfaceIPTunnelList_Destroy(CPIInterfaceIPTunnelList **listPtr);

/**
 * Adds a iptunnel entry to the list.
 *
 *   Appends <code>entry</code> to the list.  Takes ownership of the entry
 *
 * @param <#param1#>
 * @return <#return#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void cpiInterfaceIPTunnelList_Append(CPIInterfaceIPTunnelList *list, CPIInterfaceIPTunnel *entry);

/**
 * Determine if two CPIInterfaceIPTunnelList instances are equal.
 *
 * Two CPIInterfaceIPTunnelList instances are equal if, and only if,
 * the size of the lists are equal and every element is equal and in the same order.
 *
 * The following equivalence relations on non-null `CPIInterfaceIPTunnelList` instances are maintained:
 *
 *  * It is reflexive: for any non-null reference value x, `CPIInterfaceIPTunnelList_Equals(x, x)`
 *      must return true.
 *
 *  * It is symmetric: for any non-null reference values x and y,
 *    `cpiInterfaceIPTunnelList_Equals(x, y)` must return true if and only if
 *        `cpiInterfaceIPTunnelList_Equals(y, x)` returns true.
 *
 *  * It is transitive: for any non-null reference values x, y, and z, if
 *        `cpiInterfaceIPTunnelList_Equals(x, y)` returns true and
 *        `cpiInterfaceIPTunnelList_Equals(y, z)` returns true,
 *        then  `cpiInterfaceIPTunnelList_Equals(x, z)` must return true.
 *
 *  * It is consistent: for any non-null reference values x and y, multiple
 *      invocations of `cpiInterfaceIPTunnelList_Equals(x, y)` consistently return true or
 *      consistently return false.
 *
 *  * For any non-null reference value x, `cpiInterfaceIPTunnelList_Equals(x, NULL)` must
 *      return false.
 *
 * @param a A pointer to a `CPIInterfaceIPTunnelList` instance.
 * @param b A pointer to a `CPIInterfaceIPTunnelList` instance.
 * @return true if the two `CPIInterfaceIPTunnelList` instances are equal.
 *
 * Example:
 * @code
 * {
 *    CPIInterfaceIPTunnelList *a = cpiInterfaceIPTunnelList_Create();
 *    CPIInterfaceIPTunnelList *b = cpiInterfaceIPTunnelList_Create();
 *
 *    if (cpiInterfaceIPTunnelList_Equals(a, b)) {
 *        // true
 *    } else {
 *        // false
 *    }
 * }
 * @endcode
 */
bool cpiInterfaceIPTunnelList_Equals(const CPIInterfaceIPTunnelList *a, const CPIInterfaceIPTunnelList *b);

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
size_t cpiInterfaceIPTunnelList_Length(const CPIInterfaceIPTunnelList *list);

/**
 * Returns a reference counted copy of the iptunnel entry.
 *
 *   Caller must destroy the returned value.
 *   Will assert if you go beyond the end of the list.
 *
 * @param <#param1#>
 * @return <#return#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CPIInterfaceIPTunnel *cpiInterfaceIPTunnelList_Get(CPIInterfaceIPTunnelList *list, size_t index);

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
PARCJSON *cpiInterfaceIPTunnelList_ToJson(const CPIInterfaceIPTunnelList *list);

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
CPIInterfaceIPTunnelList *cpiInterfaceIPTunnelList_FromJson(PARCJSON *json);
#endif // libccnx_cpi_InterfaceIPTunnelList_h

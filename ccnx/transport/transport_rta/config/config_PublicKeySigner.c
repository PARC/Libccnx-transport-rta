/*
 * Copyright (c) 2014, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
//
//  config_PublicKeySigner.c
//  Libccnx
//
//  Created by Mosko, Marc <Marc.Mosko@parc.com> on 10/23/13.
//
//
#include <config.h>
#include <LongBow/runtime.h>

#include <stdio.h>

#include <parc/security/parc_Identity.h>
#include "config_PublicKeySigner.h"

#include <ccnx/transport/transport_rta/core/components.h>

static const char name[] = "publicKeySigner";

static const char param_KEYSTORE_NAME[] = "KEYSTORE_NAME";
static const char param_KEYSTORE_PASSWD[] = "KEYSTORE_PASSWD";
static const char param_SIGNER[] = "SIGNER";

/**
 * Generates:
 *
 * { "SIGNER" : "publicKeySigner",
 * "publicKeySigner" : { "filename" : filename, "password" : password }
 * }
 */
CCNxConnectionConfig *
configPublicKeySigner_SetIdentity(CCNxConnectionConfig *connConfig, const PARCIdentity *identity)
{
    return publicKeySigner_ConnectionConfig(connConfig,
                                                       parcIdentity_GetFileName(identity),
                                                       parcIdentity_GetPassWord(identity));
}

CCNxConnectionConfig *
publicKeySigner_ConnectionConfig(CCNxConnectionConfig *connConfig, const char *filename, const char *password)
{
    assertNotNull(connConfig, "Parameter connConfig must be non-null");
    assertNotNull(filename, "Parameter filename must be non-null");
    assertNotNull(password, "Parameter password must be non-null");

    PARCJSONValue *signerNameJson = parcJSONValue_CreateFromCString((char *) publicKeySigner_GetName());
    ccnxConnectionConfig_Add(connConfig, param_SIGNER, signerNameJson);
    parcJSONValue_Release(&signerNameJson);

    PARCJSON *keystoreJson = parcJSON_Create();
    parcJSON_AddString(keystoreJson, param_KEYSTORE_NAME, filename);
    parcJSON_AddString(keystoreJson, param_KEYSTORE_PASSWD, password);

    PARCJSONValue *value = parcJSONValue_CreateFromJSON(keystoreJson);
    parcJSON_Release(&keystoreJson);
    CCNxConnectionConfig *result = ccnxConnectionConfig_Add(connConfig, publicKeySigner_GetName(), value);
    parcJSONValue_Release(&value);
    return result;
}

const char *
publicKeySigner_GetName()
{
    return name;
}

/**
 * If successful, return true and fill in the parameters in the output argument
 */
bool
publicKeySigner_GetConnectionParams(PARCJSON *connectionJson, struct publickeysigner_params *output)
{
    assertNotNull(connectionJson, "Parameter connectionJson must be non-null");
    assertNotNull(output, "Parameter output must be non-null");

    PARCJSONValue *value = parcJSON_GetValueByName(connectionJson, publicKeySigner_GetName());
    assertNotNull(value, "JSON key %s missing", publicKeySigner_GetName());
    PARCJSON *keystoreJson = parcJSONValue_GetJSON(value);

    value = parcJSON_GetValueByName(keystoreJson, param_KEYSTORE_NAME);
    assertNotNull(value, "JSON key %s missing inside %s", param_KEYSTORE_NAME, publicKeySigner_GetName());
    PARCBuffer *sBuf = parcJSONValue_GetString(value);
    output->filename = parcBuffer_Overlay(sBuf, 0);

    value = parcJSON_GetValueByName(keystoreJson, param_KEYSTORE_PASSWD);
    assertNotNull(value, "JSON key %s missing inside %s", param_KEYSTORE_PASSWD, publicKeySigner_GetName());
    sBuf = parcJSONValue_GetString(value);
    output->password = parcBuffer_Overlay(sBuf, 0);


    return true;
}

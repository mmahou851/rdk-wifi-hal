/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2018 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/types.h>
#include <asm/byteorder.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include "wifi_hal_rdk.h"
#include "wifi_hal_priv.h"

#define TLV_HDR_SIZE (2 * sizeof(unsigned short))

wifi_tlv_t *get_tlv(unsigned char *buff, unsigned short attrib, unsigned short len)
{
    unsigned int total_len = 0;
    wifi_tlv_t *tlv = (wifi_tlv_t *)buff;

    wifi_hal_dbg_print("%s:%d Enter: attrib=0x%04x len=%u\n", __func__, __LINE__, attrib, len);

    while (total_len < len) {
        unsigned int remaining = len - total_len;

        /* Ensure the 4-byte TLV header fits before dereferencing */
        if (remaining < TLV_HDR_SIZE) {
            wifi_hal_dbg_print("%s:%d Exit 2: header exceeds remaining=%u\n", __func__, __LINE__, remaining);
            break;
        }

        /* Ensure the full TLV (header + value) fits within the buffer */
        if (tlv->length > remaining - TLV_HDR_SIZE) {
            wifi_hal_dbg_print("%s:%d Exit 2: tlv->length=%u exceeds remaining=%u\n", __func__, __LINE__, tlv->length, remaining);
            break;
        }

        if (tlv->type == attrib) {
            wifi_hal_dbg_print("%s:%d Exit 1: found attrib=0x%04x at offset=%u\n", __func__, __LINE__, attrib, total_len);
            return tlv;
        }

        total_len += TLV_HDR_SIZE + tlv->length;
        tlv = (wifi_tlv_t *)((unsigned char *)tlv + TLV_HDR_SIZE + tlv->length);
    }

    wifi_hal_dbg_print("%s:%d Exit end: attrib=0x%04x not found\n", __func__, __LINE__, attrib);
    return NULL;
}


wifi_tlv_t *set_tlv(unsigned char *buff, unsigned short attrib, unsigned short len, unsigned char *val)
{
    wifi_tlv_t *tlv = (wifi_tlv_t *)buff;

    tlv->type = attrib;
    tlv->length = len;
    memcpy(tlv->value, val, len);

    return (wifi_tlv_t *)(buff + 2*sizeof(unsigned short) + len);
}

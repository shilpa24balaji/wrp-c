/**
 *  Copyright 2010-2016 Comcast Cable Communications Management, LLC
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <CUnit/Basic.h>
#include <stdbool.h>
#include <cimplog.h>

#include "../src/wrp-c.h"


static void test_encode_decode();


struct test_vectors {
    /* Struct Version */
    const wrp_msg_t in;

    /* String Version */
    const char *string;
    const ssize_t string_size;

    /* MsgPack Version */
    const ssize_t msgpack_size;
    const uint8_t msgpack[1024];
};

headers_t headers = { 2, {"Header 1", "Header 2"}};
headers_t single_headers = { 1, {"Single Header 1"} };

const struct money_trace_span spans[] = {
    {
        .name = "hop-1",
        .start = 123000044,
        .duration = 11
    },
};


const struct test_vectors test[] = {
    /*--------------------------------------------------------------------*/
    {/* Index 0 */
        .in.msg_type = WRP_MSG_TYPE__AUTH,
        .in.u.auth.status = 123,

        .string_size = 0,
        .string =
        "wrp_auth_msg {\n"
        "    .status = 123\n"
        "}\n",

        .msgpack_size = 19, /* Really 19, but test is broken now. */
        .msgpack = {
            0x82,   /* 2 name value pairs */

            /* msg_type -> 2 */
            0xa8,  /* "msg_type" */
            'm', 's', 'g', '_', 't', 'y', 'p', 'e',
            0x02,  /* 2 */

            /* status -> 2 */
            0xa6,  /* "status" */
            's', 't', 'a', 't', 'u', 's',
            0x7b,  /* 123 */
        },
    },

    /*--------------------------------------------------------------------*/
    {/* Index 1 */
        .in.msg_type = WRP_MSG_TYPE__REQ,
        .in.u.req.transaction_uuid = "c07ee5e1-70be-444c-a156-097c767ad8aa",
        .in.u.req.source = "source-address",
        .in.u.req.dest = "dest-address",
        .in.u.req.headers = NULL,
        .in.u.req.content_type = "application/json",
        .in.u.req.include_spans = false,
        .in.u.req.spans.spans = NULL,
        .in.u.req.spans.count = 0,
        .in.u.req.payload = "123",
        .in.u.req.payload_size = 3,

        .string_size = 0,
        .string =
        "wrp_req_msg {\n"
        "    .transaction_uuid = c07ee5e1-70be-444c-a156-097c767ad8aa\n"
        "    .source           = source-address\n"
        "    .dest             = dest-address\n"
        "    .headers          = ''\n"
        "    .content_type     = application/json\n"
        "    .include_spans    = false\n"
        "    .spans            = ''\n"
        "    .payload_size     = 3\n"
        "}\n",

        .msgpack_size = 149,
        .msgpack = {
            0x86,  /* 6 name value pairs */

            /* msg_type -> 3 */
            0xa8,  /* "msg_type" */
            'm', 's', 'g', '_', 't', 'y', 'p', 'e',
            0x03,  /* 3 */

            /* source -> source-address */
            0xa6,   /* source */
            's', 'o', 'u', 'r', 'c', 'e',
            0xae,   /* source-address */
            's', 'o', 'u', 'r', 'c', 'e', '-', 'a', 'd', 'd', 'r', 'e', 's', 's',

            /* dest -> dest-address */
            0xa4,   /* dest */
            'd', 'e', 's', 't',
            0xac,   /* dest-address */
            'd', 'e', 's', 't', '-', 'a', 'd', 'd', 'r', 'e', 's', 's',

            /* transaction_uuid -> c07ee5e1-70be-444c-a156-097c767ad8aa */
            0xb0,   /* transaction_uuid */
            't', 'r', 'a', 'n', 's', 'a', 'c', 't', 'i', 'o', 'n', '_', 'u', 'u', 'i', 'd',
            0xd9, 0x24, /* c07ee5e1-70be-444c-a156-097c767ad8aa */
            'c', '0', '7', 'e', 'e', '5', 'e', '1', '-',
            '7', '0', 'b', 'e', '-',
            '4', '4', '4', 'c', '-',
            'a', '1', '5', '6', '-',
            '0', '9', '7', 'c', '7', '6', '7', 'a', 'd', '8', 'a', 'a',

            /* content_type -> application/json */
            0xac,   /* content_type */
            'c', 'o', 'n', 't', 'e', 'n', 't', '_', 't', 'y', 'p', 'e',
            0xb0,   /* application/json */
            'a', 'p', 'p', 'l', 'i', 'c', 'a', 't', 'i', 'o', 'n', '/', 'j', 's', 'o', 'n',

            /* payload -> data */
            0xa7,   /* payload */
            'p', 'a', 'y', 'l', 'o', 'a', 'd',
            0xc4,   /* WTS: I don't think this is right. */
            0x03, 0x31, 0x32, 0x33,
        },
    },

    /*--------------------------------------------------------------------*/
    {/* Index 2 */
        .in.msg_type = WRP_MSG_TYPE__REQ,
        .in.u.req.transaction_uuid = "c07ee5e1-70be-444c-a156-097c767ad8aa",
        .in.u.req.source = "source-address",
        .in.u.req.dest = "dest-address",
        .in.u.req.headers = &headers,
        .in.u.req.content_type = "application/json",
        .in.u.req.include_spans = true,
        .in.u.req.spans.spans = NULL,
        .in.u.req.spans.count = 0,
        .in.u.req.payload = "123",
        .in.u.req.payload_size = 3,

        .string_size = 0,
        .string =
        "wrp_req_msg {\n"
        "    .transaction_uuid = c07ee5e1-70be-444c-a156-097c767ad8aa\n"
        "    .source           = source-address\n"
        "    .dest             = dest-address\n"
        "    .headers          = 'Header 1, Header 2'\n"
        "    .content_type     = application/json\n"
        "    .include_spans    = true\n"
        "    .spans            = ''\n"
        "    .payload_size     = 3\n"
        "}\n",

        .msgpack_size = 191,
        .msgpack = {
            0x88,  /* 8 name value pairs */

            /* msg_type -> 3 */
            0xa8,  /* "msg_type" */
            'm', 's', 'g', '_', 't', 'y', 'p', 'e',
            0x03,  /* 3 */

            /* source -> source-address */
            0xa6,   /* source */
            's', 'o', 'u', 'r', 'c', 'e',
            0xae,   /* source-address */
            's', 'o', 'u', 'r', 'c', 'e', '-', 'a', 'd', 'd', 'r', 'e', 's', 's',

            /* dest -> dest-address */
            0xa4,   /* dest */
            'd', 'e', 's', 't',
            0xac,   /* dest-address */
            'd', 'e', 's', 't', '-', 'a', 'd', 'd', 'r', 'e', 's', 's',

            0xa7,   /* headers -> Array[2] */
            'h', 'e', 'a', 'd', 'e', 'r', 's',
            0x92,
            0xa8,
            'H', 'e', 'a', 'd', 'e', 'r', ' ', '1',
            0xa8,
            'H', 'e', 'a', 'd', 'e', 'r', ' ', '2',

            /* transaction_uuid -> c07ee5e1-70be-444c-a156-097c767ad8aa */
            0xb0,   /* transaction_uuid */
            't', 'r', 'a', 'n', 's', 'a', 'c', 't', 'i', 'o', 'n', '_', 'u', 'u', 'i', 'd',
            0xd9, 0x24, /* c07ee5e1-70be-444c-a156-097c767ad8aa */
            'c', '0', '7', 'e', 'e', '5', 'e', '1', '-',
            '7', '0', 'b', 'e', '-',
            '4', '4', '4', 'c', '-',
            'a', '1', '5', '6', '-',
            '0', '9', '7', 'c', '7', '6', '7', 'a', 'd', '8', 'a', 'a',

            /* content_type -> application/json */
            0xac,   /* content_type */
            'c', 'o', 'n', 't', 'e', 'n', 't', '_', 't', 'y', 'p', 'e',
            0xb0,   /* application/json */
            'a', 'p', 'p', 'l', 'i', 'c', 'a', 't', 'i', 'o', 'n', '/', 'j', 's', 'o', 'n',

            0xad,   /* include_spans -> true */
            'i', 'n', 'c', 'l', 'u', 'd', 'e', '_', 's', 'p', 'a', 'n', 's',
            0xc3, /* true */

            /* payload -> data */
            0xa7,   /* payload */
            'p', 'a', 'y', 'l', 'o', 'a', 'd',
            0xc4, 0x03, /* Binary message, length 3 */
            0x31, 0x32, 0x33,
        },

    },


    /*--------------------------------------------------------------------*/
    {/* Index 3 */
        .in.msg_type = WRP_MSG_TYPE__REQ,
        .in.u.req.transaction_uuid = "c07ee5e1-70be-444c-a156-097c767ad8aa",
        .in.u.req.source = "source-address",
        .in.u.req.dest = "dest-address",
        .in.u.req.headers = &headers,
        .in.u.req.content_type = "application/json",
        .in.u.req.include_spans = false,
        .in.u.req.spans.spans = NULL,
        .in.u.req.spans.count = 0,
        .in.u.req.payload = "123",
        .in.u.req.payload_size = 3,

        .string_size = 0,
        .string =
        "wrp_req_msg {\n"
        "    .transaction_uuid = c07ee5e1-70be-444c-a156-097c767ad8aa\n"
        "    .source           = source-address\n"
        "    .dest             = dest-address\n"
        "    .headers          = 'Header 1, Header 2'\n"
        "    .content_type     = application/json\n"
        "    .include_spans    = false\n"
        "    .spans            = ''\n"
        "    .payload_size     = 3\n"
        "}\n",

        .msgpack_size = 176,
        .msgpack = {
            0x87,  /* 7 name value pairs */

            /* msg_type -> 3 */
            0xa8,  /* "msg_type" */
            'm', 's', 'g', '_', 't', 'y', 'p', 'e',
            0x03,  /* 3 */

            /* source -> source-address */
            0xa6,   /* source */
            's', 'o', 'u', 'r', 'c', 'e',
            0xae,   /* source-address */
            's', 'o', 'u', 'r', 'c', 'e', '-', 'a', 'd', 'd', 'r', 'e', 's', 's',

            /* dest -> dest-address */
            0xa4,   /* dest */
            'd', 'e', 's', 't',
            0xac,   /* dest-address */
            'd', 'e', 's', 't', '-', 'a', 'd', 'd', 'r', 'e', 's', 's',

            0xa7,   /* headers -> Array[2] */
            'h', 'e', 'a', 'd', 'e', 'r', 's',
            0x92,
            0xa8,
            'H', 'e', 'a', 'd', 'e', 'r', ' ', '1',
            0xa8,
            'H', 'e', 'a', 'd', 'e', 'r', ' ', '2',

            /* transaction_uuid -> c07ee5e1-70be-444c-a156-097c767ad8aa */
            0xb0,   /* transaction_uuid */
            't', 'r', 'a', 'n', 's', 'a', 'c', 't', 'i', 'o', 'n', '_', 'u', 'u', 'i', 'd',
            0xd9, 0x24, /* c07ee5e1-70be-444c-a156-097c767ad8aa */
            'c', '0', '7', 'e', 'e', '5', 'e', '1', '-',
            '7', '0', 'b', 'e', '-',
            '4', '4', '4', 'c', '-',
            'a', '1', '5', '6', '-',
            '0', '9', '7', 'c', '7', '6', '7', 'a', 'd', '8', 'a', 'a',

            /* content_type -> application/json */
            0xac,   /* content_type */
            'c', 'o', 'n', 't', 'e', 'n', 't', '_', 't', 'y', 'p', 'e',
            0xb0,   /* application/json */
            'a', 'p', 'p', 'l', 'i', 'c', 'a', 't', 'i', 'o', 'n', '/', 'j', 's', 'o', 'n',

            /* payload -> data */
            0xa7,   /* payload */
            'p', 'a', 'y', 'l', 'o', 'a', 'd',
            0xc4, 0x03, /* Binary message, length 3 */
            0x31, 0x32, 0x33,
        },

    },

    /*--------------------------------------------------------------------*/
    {/* Index 4 */
        .in.msg_type = WRP_MSG_TYPE__REQ,
        .in.u.req.transaction_uuid = "c07ee5e1-70be-444c-a156-097c767ad8aa",
        .in.u.req.source = "source-address",
        .in.u.req.dest = "dest-address",
        .in.u.req.headers = &headers,
        .in.u.req.content_type = "application/json",
        .in.u.req.include_spans = false,
        .in.u.req.spans.spans = ( struct money_trace_span* ) spans,
        .in.u.req.spans.count = sizeof( spans ) / sizeof( struct money_trace_span ),
        .in.u.req.payload = "123",
        .in.u.req.payload_size = 3,

        .string_size = 0,
        .string =
        "wrp_req_msg {\n"
        "    .transaction_uuid = c07ee5e1-70be-444c-a156-097c767ad8aa\n"
        "    .source           = source-address\n"
        "    .dest             = dest-address\n"
        "    .headers          = 'Header 1, Header 2'\n"
        "    .content_type     = application/json\n"
        "    .include_spans    = false\n"
        "    .spans            = \n"
        "        hop-1: 123000044 - 11\n"
        "    .payload_size     = 3\n"
        "}\n",

        .msgpack_size = 196,
        .msgpack = {
            0x88,  /* 8 name value pairs */

            /* msg_type -> 3 */
            0xa8,  /* "msg_type" */
            'm', 's', 'g', '_', 't', 'y', 'p', 'e',
            0x03,  /* 3 */

            /* source -> source-address */
            0xa6,   /* source */
            's', 'o', 'u', 'r', 'c', 'e',
            0xae,   /* source-address */
            's', 'o', 'u', 'r', 'c', 'e', '-', 'a', 'd', 'd', 'r', 'e', 's', 's',

            /* dest -> dest-address */
            0xa4,   /* dest */
            'd', 'e', 's', 't',
            0xac,   /* dest-address */
            'd', 'e', 's', 't', '-', 'a', 'd', 'd', 'r', 'e', 's', 's',

            0xa7,   /* headers -> Array[2] */
            'h', 'e', 'a', 'd', 'e', 'r', 's',
            0x92,
            0xa8,
            'H', 'e', 'a', 'd', 'e', 'r', ' ', '1',
            0xa8,
            'H', 'e', 'a', 'd', 'e', 'r', ' ', '2',

            /* transaction_uuid -> c07ee5e1-70be-444c-a156-097c767ad8aa */
            0xb0,   /* transaction_uuid */
            't', 'r', 'a', 'n', 's', 'a', 'c', 't', 'i', 'o', 'n', '_', 'u', 'u', 'i', 'd',
            0xd9, 0x24, /* c07ee5e1-70be-444c-a156-097c767ad8aa */
            'c', '0', '7', 'e', 'e', '5', 'e', '1', '-',
            '7', '0', 'b', 'e', '-',
            '4', '4', '4', 'c', '-',
            'a', '1', '5', '6', '-',
            '0', '9', '7', 'c', '7', '6', '7', 'a', 'd', '8', 'a', 'a',

            /* content_type -> application/json */
            0xac,   /* content_type */
            'c', 'o', 'n', 't', 'e', 'n', 't', '_', 't', 'y', 'p', 'e',
            0xb0,   /* application/json */
            'a', 'p', 'p', 'l', 'i', 'c', 'a', 't', 'i', 'o', 'n', '/', 'j', 's', 'o', 'n',

            0xa5,   /* spans -> Array[2] */
            's', 'p', 'a', 'n', 's',
            0x91,
            0x93,
            0xa5,   /* Name */
            'h', 'o', 'p', '-', '1',
            0xce,   /* 123000044 */
            0x07, 0x54, 0xd4, 0xec,
            0x0b,   /* 11 */

            /* payload -> data */
            0xa7,   /* payload */
            'p', 'a', 'y', 'l', 'o', 'a', 'd',
            0xc4, 0x03, /* Binary message, length 3 */
            0x31, 0x32, 0x33,
        },
    },

    /*--------------------------------------------------------------------*/
    {/* Index 5 */
        .in.msg_type = WRP_MSG_TYPE__EVENT,
        .in.u.event.source = "source-address",
        .in.u.event.dest = "dest-address",
        .in.u.event.content_type = "application/json",
        .in.u.event.headers = NULL,
        .in.u.event.payload = "123",
        .in.u.event.payload_size = 3,

        .string_size = 0,
        .string =
        "wrp_event_msg {\n"
        "    .source           = source-address\n"
        "    .dest             = dest-address\n"
        "    .headers          = ''\n"
        "    .content_type     = application/json\n"
        "    .payload_size     = 3\n"
        "}\n",

        .msgpack_size = 94,
        .msgpack = {
            0x85,  /* 5 name value pairs */

            /* msg_type -> 4 */
            0xa8,  /* "msg_type" */
            'm', 's', 'g', '_', 't', 'y', 'p', 'e',
            0x04,  /* 4 */

            /* source -> source-address */
            0xa6,   /* source */
            's', 'o', 'u', 'r', 'c', 'e',
            0xae,   /* source-address */
            's', 'o', 'u', 'r', 'c', 'e', '-', 'a', 'd', 'd', 'r', 'e', 's', 's',

            /* dest -> dest-address */
            0xa4,   /* dest */
            'd', 'e', 's', 't',
            0xac,   /* dest-address */
            'd', 'e', 's', 't', '-', 'a', 'd', 'd', 'r', 'e', 's', 's',
            
            /* content_type -> application/json */
            0xac,   /* content_type */
            'c', 'o', 'n', 't', 'e', 'n', 't', '_', 't', 'y', 'p', 'e',
            0xb0,   /* application/json */
            'a', 'p', 'p', 'l', 'i', 'c', 'a', 't', 'i', 'o', 'n', '/', 'j', 's', 'o', 'n',

            /* payload -> data */
            0xa7,   /* payload */
            'p', 'a', 'y', 'l', 'o', 'a', 'd',
            0xc4, 0x03, /* Binary message, length 3 */
            0x31, 0x32, 0x33,
        },
    },

    /*--------------------------------------------------------------------*/
    {/* Index 6 */
        .in.msg_type = WRP_MSG_TYPE__EVENT,
        .in.u.event.source = "source-address",
        .in.u.event.dest = "dest-address",
        .in.u.event.content_type = "application/json",
        .in.u.event.headers = &headers,
        .in.u.event.payload = "123",
        .in.u.event.payload_size = 3,

        .string_size = 0,
        .string =
        "wrp_event_msg {\n"
        "    .source           = source-address\n"
        "    .dest             = dest-address\n"
        "    .headers          = 'Header 1, Header 2'\n"
        "    .content_type     = application/json\n"
        "    .payload_size     = 3\n"
        "}\n",

        .msgpack_size = 121,
        .msgpack = {
            0x86,  /* 6 name value pairs */

            /* msg_type -> 4 */
            0xa8,  /* "msg_type" */
            'm', 's', 'g', '_', 't', 'y', 'p', 'e',
            0x04,  /* 4 */

            /* source -> source-address */
            0xa6,   /* source */
            's', 'o', 'u', 'r', 'c', 'e',
            0xae,   /* source-address */
            's', 'o', 'u', 'r', 'c', 'e', '-', 'a', 'd', 'd', 'r', 'e', 's', 's',

            /* dest -> dest-address */
            0xa4,   /* dest */
            'd', 'e', 's', 't',
            0xac,   /* dest-address */
            'd', 'e', 's', 't', '-', 'a', 'd', 'd', 'r', 'e', 's', 's',

            0xa7,   /* headers -> Array[2] */
            'h', 'e', 'a', 'd', 'e', 'r', 's',
            0x92,
            0xa8,
            'H', 'e', 'a', 'd', 'e', 'r', ' ', '1',
            0xa8,
            'H', 'e', 'a', 'd', 'e', 'r', ' ', '2',

            /* content_type -> application/json */
            0xac,   /* content_type */
            'c', 'o', 'n', 't', 'e', 'n', 't', '_', 't', 'y', 'p', 'e',
            0xb0,   /* application/json */
            'a', 'p', 'p', 'l', 'i', 'c', 'a', 't', 'i', 'o', 'n', '/', 'j', 's', 'o', 'n',

            /* payload -> data */
            0xa7,   /* payload */
            'p', 'a', 'y', 'l', 'o', 'a', 'd',
            0xc4, 0x03, /* Binary message, length 3 */
            0x31, 0x32, 0x33,
        },
    },

    /*--------------------------------------------------------------------*/
    {/* Index 7 */
        .in.msg_type = WRP_MSG_TYPE__SVC_ALIVE,

        .string_size = 0,
        .string =
        "wrp_keep_alive_msg {\n"
        "}\n",

        .msgpack_size = 11,
        .msgpack = {
            0x81,   /* 1 name value pair */

            /* msg_type -> 10 */
            0xa8,  /* "msg_type" */
            'm', 's', 'g', '_', 't', 'y', 'p', 'e',
            0x0a,  /* 10 */
        },
    },
};

void validate_to_strings( const char *expected, ssize_t expected_len,
                          const char *actual, size_t actual_len )
{
    if( 0 == expected_len ) {
        expected_len = strlen( expected );
    }

    if( ( NULL != actual ) && ( 0 != strcmp( actual, expected ) ) ) {
        cimplog_info("WRP-C: ", "\n\nGot: |%s| Expected: |%s|\n\n", actual, expected );
    }

    CU_ASSERT_STRING_EQUAL( actual, expected );
    CU_ASSERT_EQUAL( ( ssize_t ) actual_len, expected_len );
}

void _internal_tva_xxd( const void *buffer, const size_t length, size_t line )
{
    const char hex[17] = "0123456789abcdef";
    const char *data = ( char * ) buffer;
    const char *end = &data[length];
    char output[70];

    if( ( NULL == buffer ) || ( 0 == length ) ) {
        return;
    }

    while( data < end ) {
        int shiftCount;
        size_t i;
        char *text_ptr = &output[51];
        char *ptr = output;

        /* Output the '00000000:' portion */
        for( shiftCount = 28; shiftCount >= 0; shiftCount -= 4 ) {
            *ptr++ = hex[( line >> shiftCount ) & 0x0F];
        }

        *ptr++ = ':';
        *ptr++ = ' ';

        for( i = 0; i < 16; i++ ) {
            if( data < end ) {
                *ptr++ = hex[( 0x0f & ( *data >> 4 ) )];
                *ptr++ = hex[( 0x0f & ( *data ) )];

                if( ( ' ' <= *data ) && ( *data <= '~' ) ) {
                    *text_ptr++ = *data;
                } else {
                    *text_ptr++ = '.';
                }

                data++;
            } else {
                *ptr++ = ' ';
                *ptr++ = ' ';
                *text_ptr++ = ' ';
            }

            if( 0x01 == ( 0x01 & i ) ) {
                *ptr++ = ' ';
            }
        }

        line += 16;
        *ptr = ' ';
        *text_ptr = '\0';
        puts( output );
    }
}


void validate_to_bytes( const uint8_t *expected, ssize_t expected_size,
                        const uint8_t *actual, ssize_t actual_size )
{
    int tmp;

    if( expected_size != actual_size ) {
        cimplog_info("WRP-C: ", "\n\nsize: %zd, expected: %zd\n", actual_size, expected_size );
    }

    CU_ASSERT( expected_size == actual_size );

    if( 0 < expected_size ) {
        tmp = memcmp( expected, actual, expected_size );

        if( ( 0 != tmp ) || ( expected_size != actual_size ) ) {
            ssize_t i;
            i = 0;

            while( ( i < actual_size ) && ( actual[i] == expected[i] ) ) {
                i++;
            }

            cimplog_error("\n\nmemcmp() RV: ", "%d, Mismatch Offset: 0x%zx, expected_size: %zd, actual_size: %zd\n", tmp, i, expected_size, actual_size );
            cimplog_info("\nWRP-C: ", "Expected:\n" );
            _internal_tva_xxd( expected, expected_size, 0 );
            cimplog_info("\nWRP-C: ", "Actual:\n" );
            _internal_tva_xxd( actual, actual_size, 0 );
        }

        CU_ASSERT( 0 == tmp );
    }
}

void validate_from_bytes( wrp_msg_t *msg, const char *expected )
{
    char *actual;
    actual = wrp_struct_to_string( msg );

    if( NULL == expected ) {
        CU_ASSERT( expected == actual );
    } else {
        CU_ASSERT( NULL != actual );

        if( NULL != actual ) {
            if( 0 != strcmp( actual, expected ) ) {
                cimplog_info("\n\nWRP-C: ", "Got: ", "|%s| Expected: |%s|\n\n", actual, expected );
            }

            CU_ASSERT_STRING_EQUAL( actual, expected );
            free( actual );
        }
    }
}

void test_all()
{
    size_t i;
    ssize_t size;
    void *bytes;
    wrp_msg_t *message;

    for( i = 0; i < sizeof( test ) / sizeof( struct test_vectors ); i++ ) {
        char *string;
        /* Testing wrp_struct_to_string(). */
        string = wrp_struct_to_string( &test[i].in );
        validate_to_strings( test[i].string, test[i].string_size, string, strlen( string ) );

        if( NULL != string ) {
            free( string );
        }

        /* Testing wrp_struct_to() --> string. */
        size = wrp_struct_to( &test[i].in, WRP_STRING, ( void** ) &string );
        validate_to_strings( test[i].string, test[i].string_size, string, size );

        if( 0 < size ) {
            free( string );
        }

        /* Testing wrp_struct_to() --> bytes. */
        size = wrp_struct_to( &test[i].in, WRP_BYTES, &bytes );
        validate_to_bytes( test[i].msgpack, test[i].msgpack_size, bytes, size );

        if( 0 < size ) {
            free( bytes );
        }

        /* Testing wrp_to_struct() --> from bytes. */
        if( 1 == i ) {
            wrp_msg_t *msg;
            msg = NULL;
            size = wrp_to_struct( test[i].msgpack, test[i].msgpack_size, WRP_BYTES, &msg );
            validate_from_bytes( msg, test[i].string );

            if( NULL != msg ) {
                wrp_free_struct( msg );
            }
        }
    }

    cimplog_info("WRP-C: ", "Testing NULL msg handling\n" );
    size = wrp_struct_to( NULL, WRP_BYTES, bytes );
    CU_ASSERT( size < 0 );
    cimplog_info("WRP-C: ", "Testing Invalid conversion type handling\n" );
    size = wrp_struct_to( &test[2].in, 911, &bytes );
    CU_ASSERT( size < 0 );
    cimplog_info("WRP-C: ", "Testing Invalid message type handling\n" );
    size = wrp_struct_to( (wrp_msg_t*) "*** Invalid WRP message\n", WRP_BYTES, &bytes );
    CU_ASSERT( size < 0 );
    CU_ASSERT(NULL == bytes);
    cimplog_info("WRP-C: ", "Testing NULL data handling\n" );
    size = wrp_struct_to( &test[2].in, WRP_BYTES, NULL );
    CU_ASSERT( size < 0 );
    cimplog_info("WRP-C: ", "Testing NULL wrp_to_struct handling\n" );
    wrp_to_struct( NULL, 0, WRP_BYTES, NULL );
    CU_ASSERT( size < 0 );
    cimplog_info("WRP-C: ", "Testing wrp_to_struct null data handling\n" );
    wrp_to_struct( test[3].msgpack, test[3].msgpack_size, WRP_BYTES, NULL );
    CU_ASSERT( size < 0 );
    cimplog_info("WRP-C: ", "Testing wrp_to_struct invalid type handling\n" );
    wrp_to_struct( test[3].msgpack, test[3].msgpack_size, 911, &message );
    CU_ASSERT( size < 0 );
    test_encode_decode();
}



void test_encode_decode()
{
    ssize_t size, base64_size, rv;
    void *bytes;
    wrp_msg_t *message;
    cimplog_debug( "\n WRP-C: ", "Inside test_encode_decode()....\n" );
    const wrp_msg_t msg = { .msg_type = WRP_MSG_TYPE__REQ,
                            .u.req.transaction_uuid = "c07ee5e1-70be-444c-a156-097c767ad8aa",
                            .u.req.content_type = "application/json",
                            .u.req.source = "source-address",
                            .u.req.dest = "dest-address",
                            .u.req.headers = &headers,
                            .u.req.include_spans = false,
                            .u.req.spans.spans = NULL,
                            .u.req.spans.count = 0,
                            .u.req.payload = "123",
                            .u.req.payload_size = 3
                          };
    const wrp_msg_t event_m = { .msg_type = WRP_MSG_TYPE__EVENT,
                                .u.event.source = "source-address",
                                .u.event.dest = "dest-address",
                                .u.event.content_type = "application/json",
                                .u.event.headers = &headers,
                                .u.event.payload = "0123456789",
                                .u.event.payload_size = 10
                              };
    const wrp_msg_t msg2 = { .msg_type = WRP_MSG_TYPE__REQ,
                             .u.req.transaction_uuid = "c07ee5e1-70be-444c-a156-097c767ad8aa",
                             .u.req.source = "source-address",
                             .u.req.content_type = NULL,
                             .u.req.dest = "dest-address",
                             .u.req.headers = &single_headers,
                             .u.req.include_spans = false,
                             .u.req.spans.spans = NULL,
                             .u.req.spans.count = 0,
                             .u.req.payload = "123",
                             .u.req.payload_size = 3
                           };
    const wrp_msg_t reg = { .msg_type = WRP_MSG_TYPE__SVC_REGISTRATION,
                            .u.reg.service_name = "IoT",
                            .u.reg.url = "tcp://127.0.0.0:6888"
                          };
    // msgpack encode
    size = wrp_struct_to( &msg, WRP_BYTES, &bytes );
    /* print the encoded message */
    _internal_tva_xxd( bytes, size, 0 );
    // msgpck decode
    rv = wrp_to_struct( bytes, size, WRP_BYTES, &message );
    free( bytes );
    CU_ASSERT_EQUAL( rv, size );
    CU_ASSERT_EQUAL( message->msg_type, msg.msg_type );
    CU_ASSERT_STRING_EQUAL( message->u.req.source, msg.u.req.source );
    CU_ASSERT_STRING_EQUAL( message->u.req.dest, msg.u.req.dest );
    CU_ASSERT_STRING_EQUAL( message->u.req.content_type, msg.u.req.content_type );
    CU_ASSERT_STRING_EQUAL( message->u.req.transaction_uuid, msg.u.req.transaction_uuid );
    CU_ASSERT_STRING_EQUAL( message->u.req.payload, msg.u.req.payload );

    if( NULL != msg.u.req.headers ) {
        size_t n = 0;
        cimplog_debug("WRP-C: ", "headers count returned is %d\n", ( int ) message->u.req.headers->count );

        if( NULL != msg.u.req.headers ) {
            while( n < msg.u.req.headers->count ) {
                CU_ASSERT_STRING_EQUAL( msg.u.req.headers->headers[n],
                                        message->u.req.headers->headers[n] );
                n++;
            }
        } else {
            CU_ASSERT( false );
        }
    }

    cimplog_debug("WRP-C: ", "decoded msgType:%d\n", message->msg_type );
    cimplog_debug("WRP-C: ", "decoded source:%s\n", message->u.req.source );
    cimplog_debug("WRP-C: ", "decoded dest:%s\n", message->u.req.dest );
    cimplog_debug("WRP-C: ", "decoded content_type:%s\n", message->u.req.content_type );
    cimplog_debug("WRP-C: ", "decoded transaction_uuid:%s\n", message->u.req.transaction_uuid );
    cimplog_debug("WRP-C: ", "decoded payload:%s\n", ( char* )message->u.req.payload );
    wrp_free_struct( message );
    cimplog_debug("WRP-C: ", "Encode-Decode for BASE64\n" );
    // msgpack encode
    base64_size = wrp_struct_to( &msg, WRP_BASE64, &bytes );
    /* print the encoded message */
    _internal_tva_xxd( bytes, base64_size, 0 );
    // msgpck decode
    rv = wrp_to_struct( bytes, base64_size, WRP_BASE64, &message );
    free( bytes );
    CU_ASSERT_EQUAL( rv, size );
    CU_ASSERT_EQUAL( message->msg_type, msg.msg_type );
    CU_ASSERT_STRING_EQUAL( message->u.req.source, msg.u.req.source );
    CU_ASSERT_STRING_EQUAL( message->u.req.dest, msg.u.req.dest );
    CU_ASSERT_STRING_EQUAL( message->u.req.content_type, msg.u.req.content_type );
    CU_ASSERT_STRING_EQUAL( message->u.req.transaction_uuid, msg.u.req.transaction_uuid );
    CU_ASSERT_STRING_EQUAL( message->u.req.payload, msg.u.req.payload );
    cimplog_debug("WRP-C: ", "decoded msgType:%d\n", message->msg_type );
    cimplog_debug("WRP-C: ", "decoded source:%s\n", message->u.req.source );
    cimplog_debug("WRP-C: ", "decoded dest:%s\n", message->u.req.dest );
    cimplog_debug("WRP-C: ", "decoded transaction_uuid:%s\n", message->u.req.transaction_uuid );
    cimplog_debug("WRP-C: ", "decoded content_type:%s\n", message->u.req.content_type );
    cimplog_debug("WRP-C: ", "decoded payload:%s\n", ( char* )message->u.req.payload );
    wrp_free_struct( message );
    // msgpack encode
    const wrp_msg_t *event_msg = &test[5].in;
    size = wrp_struct_to( event_msg, WRP_BYTES, &bytes );
    free( bytes );
    base64_size = wrp_struct_to( event_msg, WRP_BASE64, &bytes );
    /* print the encoded message */
    _internal_tva_xxd( bytes, base64_size, 0 );
    // msgpck decode
    rv = wrp_to_struct( bytes, base64_size, WRP_BASE64, &message );
    free( bytes );

    if( 0 < rv ) {
        CU_ASSERT_EQUAL( rv, size );
        CU_ASSERT_EQUAL( message->u.event.payload_size, event_msg->u.event.payload_size );
        CU_ASSERT_EQUAL( message->msg_type, event_msg->msg_type );
        CU_ASSERT_STRING_EQUAL( message->u.event.source, event_msg->u.event.source );
        CU_ASSERT_STRING_EQUAL( message->u.event.dest, event_msg->u.event.dest );
        CU_ASSERT_STRING_EQUAL( message->u.event.content_type, event_msg->u.event.content_type );
        CU_ASSERT_STRING_EQUAL( message->u.event.payload, event_msg->u.event.payload );

        if( NULL != event_msg->u.event.headers ) {
            size_t n = 0;

            while( n < event_msg->u.event.headers->count ) {
                CU_ASSERT_STRING_EQUAL( message->u.event.headers->headers[n],
                                        event_msg->u.event.headers->headers[n] );
                n++;
            }
        }
    }

    wrp_free_struct( message );
    cimplog_debug("WRP-C: ", "decoded msgType:%d\n", event_msg->msg_type );
    cimplog_debug("WRP-C: ", "decoded source:%s\n", event_msg->u.event.source );
    cimplog_debug("WRP-C: ", "decoded dest:%s\n", event_msg->u.event.dest );
    cimplog_debug("WRP-C: ", "decoded content_type:%s\n", event_msg->u.event.content_type );
    cimplog_debug("WRP-C: ", "decoded payload:%s\n", ( char* )event_msg->u.event.payload );
    cimplog_debug("WRP-C: ", "message->u.event.payload_size %zu\n", message->u.event.payload_size );
    // msgpack encode
    event_msg = &event_m;
    size = wrp_struct_to( event_msg, WRP_BYTES, &bytes );
    free( bytes );
    base64_size = wrp_struct_to( event_msg, WRP_BASE64, &bytes );
    /* print the encoded message */
    _internal_tva_xxd( bytes, base64_size, 0 );
    // msgpck decode
    rv = wrp_to_struct( bytes, base64_size, WRP_BASE64, &message );
    free( bytes );

    if( 0 < rv ) {
        CU_ASSERT_EQUAL( rv, size );
        CU_ASSERT_EQUAL( message->msg_type, event_msg->msg_type );
        CU_ASSERT_STRING_EQUAL( message->u.event.source, event_msg->u.event.source );
        CU_ASSERT_STRING_EQUAL( message->u.event.dest, event_msg->u.event.dest );
        CU_ASSERT_STRING_EQUAL( message->u.event.content_type, event_msg->u.event.content_type );
        CU_ASSERT_STRING_EQUAL( message->u.event.payload, event_msg->u.event.payload );

        if( NULL != event_msg->u.event.headers ) {
            size_t n = 0;

            while( n < event_msg->u.event.headers->count ) {
                CU_ASSERT_STRING_EQUAL( message->u.event.headers->headers[n],
                                        event_msg->u.event.headers->headers[n] );
                n++;
            }
        }
    }

    wrp_free_struct( message );
    // msgpack encode
    size = wrp_struct_to( &msg2, WRP_BYTES, &bytes );
    /* print the encoded message */
    _internal_tva_xxd( bytes, size, 0 );
    // msgpck decode
    rv = wrp_to_struct( bytes, size, WRP_BYTES, &message );
    free( bytes );
    CU_ASSERT_EQUAL( rv, size );
    CU_ASSERT_EQUAL( message->msg_type, msg2.msg_type );
    CU_ASSERT_STRING_EQUAL( message->u.req.source, msg2.u.req.source );
    CU_ASSERT_STRING_EQUAL( message->u.req.dest, msg2.u.req.dest );
    CU_ASSERT_STRING_EQUAL( message->u.req.transaction_uuid, msg2.u.req.transaction_uuid );
    CU_ASSERT_STRING_EQUAL( message->u.req.payload, msg2.u.req.payload );

    if( NULL != message->u.req.headers ) {
        size_t n = 0;

        while( n < message->u.req.headers->count ) {
            CU_ASSERT_STRING_EQUAL( msg2.u.req.headers->headers[n],
                                    message->u.req.headers->headers[n] );
            n++;
        }
    }

    wrp_free_struct( message );
    cimplog_debug("WRP-C: ", "msgtype 9--Registration message\n" );
    // msgpack encode for msgtype 9--Registration message
    size = wrp_struct_to( &reg, WRP_BYTES, &bytes );
    /* print the encoded message */
    _internal_tva_xxd( bytes, size, 0 );
    // msgpck decode
    rv = wrp_to_struct( bytes, size, WRP_BYTES, &message );
    free( bytes );
    CU_ASSERT_EQUAL( rv, size );
    CU_ASSERT_EQUAL( message->msg_type, reg.msg_type );
    CU_ASSERT_STRING_EQUAL( message->u.reg.service_name, reg.u.reg.service_name );
    CU_ASSERT_STRING_EQUAL( message->u.reg.url, reg.u.reg.url );
    cimplog_debug("WRP-C: ", "decoded msgType:%d\n", message->msg_type );
    cimplog_debug("WRP-C: ", "decoded service_name:%s\n", message->u.reg.service_name );
    cimplog_debug("WRP-C: ", "decoded dest:%s\n", message->u.reg.url );
    wrp_free_struct( message );
    message = ( wrp_msg_t * ) malloc( sizeof( wrp_msg_t ) );
    cimplog_debug("WRP-C: ", "Testing invalid message type free\n" );
    message->msg_type = WRP_MSG_TYPE__UNKNOWN;
    wrp_free_struct( message );
}

void test_crud_message()
{
    ssize_t size, rv;
    void *bytes;
    wrp_msg_t *message;
    cimplog_debug( "\n WRP-C: ", "Inside Crud_encode_decode()....\n" );
    struct data meta_data2[] = {{"firmware", "PROD-DEV"}, {"model", "TG1680"}};
    data_t meta_data = {2, meta_data2};
    const struct money_trace_span crud_spans[] = {
        {
            .name = "hop-1",
            .start = 123000044,
            .duration = 11
        },
    };
    char updatePayload[]="{\n\"webpa-url\": \"fabric.webpa.comast.net\", \n\"webpa-uuid\": \"123d\"\n}";
    char createPayload[]="{\n\"tags\": {\n\t\"test\": {\n\t\t\"expires\": \"432123\"\n\t\t}\n\t}\n}";
    
    const wrp_msg_t create = {
        .msg_type = WRP_MSG_TYPE__CREATE,
        .u.crud.transaction_uuid = "c07ee5e1-70be-444c-a156-097c767ad8aa",
        .u.crud.source = "source-address",
        .u.crud.dest = "dest-address",
        .u.crud.headers = &single_headers,
        .u.crud.metadata = NULL,
        .u.crud.include_spans = false,
        .u.crud.spans.spans = NULL,
        .u.crud.spans.count = 0,
        .u.crud.status = 1,
        .u.crud.path = "/Harvester",
        .u.crud.payload = createPayload
    };
    const wrp_msg_t retreive = {
        .msg_type = WRP_MSG_TYPE__RETREIVE,
        .u.crud.transaction_uuid = "c07ee5e1-70be-444c-a156-097c767ad8aa",
        .u.crud.source = "source-address",
        .u.crud.dest = "dest-address",
        .u.crud.headers = &headers,
        .u.crud.metadata = &meta_data,
        .u.crud.include_spans = false,
        .u.crud.spans.spans = ( struct money_trace_span* ) crud_spans,
        .u.crud.spans.count = sizeof( crud_spans ) / sizeof( struct money_trace_span ),
        .u.crud.status = 1,
        .u.crud.path = "/IOT",
        .u.crud.payload = NULL
    };
    const wrp_msg_t update = {
        .msg_type = WRP_MSG_TYPE__UPDATE,
        .u.crud.transaction_uuid = "c07ee5e1-70be-444c-a156-097c767ad8aa",
        .u.crud.source = "source-address",
        .u.crud.dest = "dest-address",
        .u.crud.headers = NULL,
        .u.crud.metadata = NULL,
        .u.crud.include_spans = false,
        .u.crud.spans.spans = ( struct money_trace_span* ) spans,
        .u.crud.spans.count = sizeof( spans ) / sizeof( struct money_trace_span ),
        .u.crud.status = 0,
        .u.crud.path = "/Harvester",
        .u.crud.payload = updatePayload
    };
    const wrp_msg_t delete = {
        .msg_type = WRP_MSG_TYPE__DELETE,
        .u.crud.transaction_uuid = "c07ee5e1-70be-444c-a156-097c767ad8aa",
        .u.crud.source = "source-address",
        .u.crud.dest = "dest-address",
        .u.crud.headers = NULL,
        .u.crud.metadata = NULL,
        .u.crud.include_spans = false,
        .u.crud.spans.spans = NULL,
        .u.crud.spans.count = 0,
        .u.crud.status = 1,
        .u.crud.path = "/IOT",
        .u.crud.payload = NULL
    };
    const wrp_msg_t meta_payload = {
        .msg_type = WRP_MSG_TYPE__UPDATE,
        .u.crud.transaction_uuid = "c07ee5e1-70be-444c-a156-097c767ad8aa",
        .u.crud.source = "source-address",
        .u.crud.dest = "dest-address",
        .u.crud.headers = &headers,
        .u.crud.metadata = &meta_data,
        .u.crud.include_spans = false,
        .u.crud.spans.spans = ( struct money_trace_span* ) spans,
        .u.crud.spans.count = sizeof( spans ) / sizeof( struct money_trace_span ),
        .u.crud.status = 1,
        .u.crud.path = "/Harvester",
        .u.crud.payload = updatePayload
    };
    cimplog_debug("WRP-C: ", "     **************** CRUD Create*****************     \n" );
    // msgpack encode for CRUD
    size = wrp_struct_to( &create, WRP_BYTES, &bytes );
    /* print the encoded message */
    _internal_tva_xxd( bytes, size, 0 );
    // msgpck decode
    rv = wrp_to_struct( bytes, size, WRP_BYTES, &message );
    free( bytes );
    CU_ASSERT_EQUAL( rv, size );
    CU_ASSERT_EQUAL( message->msg_type, create.msg_type );
    CU_ASSERT_STRING_EQUAL( message->u.crud.source, create.u.crud.source );
    CU_ASSERT_STRING_EQUAL( message->u.crud.dest, create.u.crud.dest );
    CU_ASSERT_STRING_EQUAL( message->u.crud.transaction_uuid, create.u.crud.transaction_uuid );
    CU_ASSERT_STRING_EQUAL( message->u.crud.path, create.u.crud.path );

    if( message->u.crud.payload != NULL ) 
    {
            CU_ASSERT_STRING_EQUAL( create.u.crud.payload, message->u.crud.payload);
            cimplog_info("WRP-C: ", "Crud payload : %s\n",message->u.crud.payload);
    }
    if( message->u.crud.metadata != NULL ) {
        size_t n = 0;

        while( n < message->u.crud.metadata->count ) {
            CU_ASSERT_STRING_EQUAL( create.u.crud.metadata->data_items[n].name, message->u.crud.metadata->data_items[n].name );
            CU_ASSERT_STRING_EQUAL( create.u.crud.metadata->data_items[n].value, message->u.crud.metadata->data_items[n].value );
            cimplog_info("WRP-C: ", "Metadata Key value pair: %s = %s\n",message->u.crud.metadata->data_items[n].name,message->u.crud.metadata->data_items[n].value);
            n++;
        }
    }

    if( NULL != message->u.crud.headers ) {
        size_t n = 0;
        cimplog_debug("WRP-C: ", "headers count returned is %d\n", ( int ) message->u.req.headers->count );

        if( NULL != message->u.crud.headers ) {
            while( n < message->u.crud.headers->count ) {
                CU_ASSERT_STRING_EQUAL( create.u.crud.headers->headers[n],
                                        message->u.crud.headers->headers[n] );
                n++;
            }
        } else {
            CU_ASSERT( false );
        }
    }

    cimplog_debug("WRP-C: ", "decoded msgType:%d\n", message->msg_type );
    cimplog_debug("WRP-C: ", "decoded source:%s\n", message->u.crud.source );
    cimplog_debug("WRP-C: ", "decoded dest:%s\n", message->u.crud.dest );
    cimplog_debug("WRP-C: ", "decoded transaction_uuid:%s\n", message->u.crud.transaction_uuid );
    cimplog_debug("WRP-C: ", "decoded path %s\n", message->u.crud.path );
    wrp_free_struct( message );
    cimplog_debug("WRP-C: ", "      **************** CRUD retreive*****************     \n" );
    // msgpack encode for CRUD
    size = wrp_struct_to( &retreive, WRP_BYTES, &bytes );
    /* print the encoded message */
    _internal_tva_xxd( bytes, size, 0 );
    // msgpck decode
    rv = wrp_to_struct( bytes, size, WRP_BYTES, &message );
    free( bytes );
    CU_ASSERT_EQUAL( rv, size );
    CU_ASSERT_EQUAL( message->msg_type, retreive.msg_type );
    CU_ASSERT_STRING_EQUAL( message->u.crud.source, retreive.u.crud.source );
    CU_ASSERT_STRING_EQUAL( message->u.crud.dest, retreive.u.crud.dest );
    CU_ASSERT_STRING_EQUAL( message->u.crud.transaction_uuid, retreive.u.crud.transaction_uuid );
    CU_ASSERT_STRING_EQUAL( message->u.crud.path, retreive.u.crud.path );
    
     if( message->u.crud.payload != NULL ) 
     {
        CU_ASSERT_STRING_EQUAL( retreive.u.crud.payload, message->u.crud.payload);
        cimplog_info("WRP-C: ", "Crud payload : %s\n",message->u.crud.payload);
     }
    if( message->u.crud.metadata != NULL ) {
        size_t n = 0;

        while( n < message->u.crud.metadata->count ) {
            CU_ASSERT_STRING_EQUAL( retreive.u.crud.metadata->data_items[n].name, message->u.crud.metadata->data_items[n].name );
            CU_ASSERT_STRING_EQUAL( retreive.u.crud.metadata->data_items[n].value, message->u.crud.metadata->data_items[n].value );
            cimplog_info("WRP-C: ", "Metadata Key value pair: %s = %s\n",message->u.crud.metadata->data_items[n].name,message->u.crud.metadata->data_items[n].value);
            n++;
        }
    }
    else
    {
        cimplog_error("WRP-C: ", "retrieve metadata is NULL.Please fix it\n");
    }
    if( NULL != message->u.crud.headers ) {
        size_t n = 0;
        cimplog_info("WRP-C: ", "headers count returned is %d\n", ( int ) message->u.req.headers->count );

        if( NULL != message->u.crud.headers ) {
            while( n < message->u.crud.headers->count ) {
                CU_ASSERT_STRING_EQUAL( retreive.u.crud.headers->headers[n],
                                        message->u.crud.headers->headers[n] );
                n++;
            }
        } else {
            CU_ASSERT( false );
        }
    }

    cimplog_debug("WRP-C: ", "decoded msgType:%d\n", message->msg_type );
    cimplog_debug("WRP-C: ", "decoded source:%s\n", message->u.crud.source );
    cimplog_debug("WRP-C: ", "decoded dest:%s\n", message->u.crud.dest );
    cimplog_debug("WRP-C: ", "decoded transaction_uuid:%s\n", message->u.crud.transaction_uuid );
    cimplog_debug("WRP-C: ", "decoded path:%s\n", message->u.crud.path );
    wrp_free_struct( message );
    cimplog_debug("WRP-C: ", "     **************** CRUD Update*****************     \n" );
    // msgpack encode for CRUD
    size = wrp_struct_to( &update, WRP_BYTES, &bytes );
    /* print the encoded message */
    _internal_tva_xxd( bytes, size, 0 );
    // msgpck decode
    rv = wrp_to_struct( bytes, size, WRP_BYTES, &message );
    free( bytes );
    CU_ASSERT_EQUAL( rv, size );
    CU_ASSERT_EQUAL( message->msg_type, update.msg_type );
    CU_ASSERT_STRING_EQUAL( message->u.crud.source, update.u.crud.source );
    CU_ASSERT_STRING_EQUAL( message->u.crud.dest, update.u.crud.dest );
    CU_ASSERT_STRING_EQUAL( message->u.crud.transaction_uuid, update.u.crud.transaction_uuid );
    CU_ASSERT_STRING_EQUAL( message->u.crud.path, update.u.crud.path );

    if( message->u.crud.payload != NULL ) 
    {
        CU_ASSERT_STRING_EQUAL( update.u.crud.payload, message->u.crud.payload);
        cimplog_info("WRP-C: ", "Crud payload : %s\n",message->u.crud.payload);
    }
    if( message->u.crud.metadata != NULL ) {
        size_t n = 0;

        while( n < message->u.crud.metadata->count ) {
            CU_ASSERT_STRING_EQUAL( update.u.crud.metadata->data_items[n].name, message->u.crud.metadata->data_items[n].name );
            CU_ASSERT_STRING_EQUAL( update.u.crud.metadata->data_items[n].value, message->u.crud.metadata->data_items[n].value );
            cimplog_info("WRP-C: ", "Metadata Key value pair: %s = %s\n",message->u.crud.metadata->data_items[n].name,message->u.crud.metadata->data_items[n].value);
            n++;
        }
    }
    if( NULL != message->u.crud.headers ) {
        size_t n = 0;
        cimplog_debug("WRP-C: ", "headers count returned is %d\n", ( int ) message->u.req.headers->count );

        if( NULL != message->u.crud.headers ) {
            while( n < message->u.crud.headers->count ) {
                CU_ASSERT_STRING_EQUAL( update.u.crud.headers->headers[n],
                                        message->u.crud.headers->headers[n] );
                n++;
            }
        } else {
            CU_ASSERT( false );
        }
    }

    cimplog_debug("WRP-C: ", "decoded msgType:%d\n", message->msg_type );
    cimplog_debug("WRP-C: ", "decoded source:%s\n", message->u.crud.source );
    cimplog_debug("WRP-C: ", "decoded dest:%s\n", message->u.crud.dest );
    cimplog_debug("WRP-C: ", "decoded transaction_uuid:%s\n", message->u.crud.transaction_uuid );
    cimplog_debug("WRP-C: ", "decoded path:%s\n", message->u.crud.path );
    wrp_free_struct( message );
    cimplog_debug("WRP-C: ", "     **************** CRUD Delete*****************     \n" );
    // msgpack encode for CRUD
    size = wrp_struct_to( &delete, WRP_BYTES, &bytes );
    /* print the encoded message */
    _internal_tva_xxd( bytes, size, 0 );
    // msgpck decode
    rv = wrp_to_struct( bytes, size, WRP_BYTES, &message );
    free( bytes );
    CU_ASSERT_EQUAL( rv, size );
    CU_ASSERT_EQUAL( message->msg_type, delete.msg_type );
    CU_ASSERT_STRING_EQUAL( message->u.crud.source, delete.u.crud.source );
    CU_ASSERT_STRING_EQUAL( message->u.crud.dest, delete.u.crud.dest );
    CU_ASSERT_STRING_EQUAL( message->u.crud.transaction_uuid, delete.u.crud.transaction_uuid );
    CU_ASSERT_STRING_EQUAL( message->u.crud.path, delete.u.crud.path );
    if( message->u.crud.payload != NULL ) 
    {
        CU_ASSERT_STRING_EQUAL( delete.u.crud.payload, message->u.crud.payload);
        cimplog_info("WRP-C: ", "Crud payload : %s\n",message->u.crud.payload);
    }
    if( message->u.crud.metadata != NULL ) {
        size_t n = 0;

        while( n < message->u.crud.metadata->count ) {
            CU_ASSERT_STRING_EQUAL( delete.u.crud.metadata->data_items[n].name, message->u.crud.metadata->data_items[n].name );
            CU_ASSERT_STRING_EQUAL( delete.u.crud.metadata->data_items[n].value, message->u.crud.metadata->data_items[n].value );
            cimplog_info("WRP-C: ", "Delete Metadata Key value pair: %s = %s\n",message->u.crud.metadata->data_items[n].name,message->u.crud.metadata->data_items[n].value);
            n++;
        }
    }
    if( NULL != message->u.crud.headers ) {
        size_t n = 0;
        cimplog_debug("WRP-C: ", "headers count returned is %d\n", ( int ) message->u.req.headers->count );

        if( NULL != message->u.crud.headers ) {
            while( n < message->u.crud.headers->count ) {
                CU_ASSERT_STRING_EQUAL( delete.u.crud.headers->headers[n],
                                        message->u.crud.headers->headers[n] );
                n++;
            }
        } else {
            CU_ASSERT( false );
        }
    }

    cimplog_debug("WRP-C: ", "decoded msgType:%d\n", message->msg_type );
    cimplog_debug("WRP-C: ", "decoded source:%s\n", message->u.crud.source );
    cimplog_debug("WRP-C: ", "decoded dest:%s\n", message->u.crud.dest );
    cimplog_debug("WRP-C: ", "decoded transaction_uuid:%s\n", message->u.crud.transaction_uuid );
    cimplog_debug("WRP-C: ", "decoded path:%s\n", message->u.crud.path );
    wrp_free_struct( message );
    cimplog_debug("WRP-C: ", "     **************** Metadata *****************     \n" );
    // msgpack encode for METADATA
    size = wrp_struct_to( &meta_payload, WRP_BYTES, &bytes );
    /* print the encoded message */
    _internal_tva_xxd( bytes, size, 0 );
    // msgpck decode
    rv = wrp_to_struct( bytes, size, WRP_BYTES, &message );
    free( bytes );
    CU_ASSERT_EQUAL( rv, size );
    CU_ASSERT_EQUAL( message->msg_type, meta_payload.msg_type );
    CU_ASSERT_STRING_EQUAL( message->u.crud.source, meta_payload.u.crud.source );
    CU_ASSERT_STRING_EQUAL( message->u.crud.dest, meta_payload.u.crud.dest );
    CU_ASSERT_STRING_EQUAL( message->u.crud.transaction_uuid, meta_payload.u.crud.transaction_uuid );
    CU_ASSERT_STRING_EQUAL( message->u.crud.path, meta_payload.u.crud.path );
    if( message->u.crud.payload != NULL ) 
    {
        CU_ASSERT_STRING_EQUAL( meta_payload.u.crud.payload, message->u.crud.payload);
        cimplog_info("WRP-C: ", "Crud payload : %s\n",message->u.crud.payload);
    }
    if( message->u.crud.metadata != NULL ) {
        size_t n = 0;

        while( n < message->u.crud.metadata->count ) {
            CU_ASSERT_STRING_EQUAL( meta_payload.u.crud.metadata->data_items[n].name, message->u.crud.metadata->data_items[n].name );
            CU_ASSERT_STRING_EQUAL( meta_payload.u.crud.metadata->data_items[n].value, message->u.crud.metadata->data_items[n].value );
            cimplog_info("WRP-C: ", "Update Metadata Key value pair: %s = %s\n",message->u.crud.metadata->data_items[n].name,message->u.crud.metadata->data_items[n].value);
            n++;
        }
    }
    if( NULL != meta_payload.u.crud.headers ) {
        size_t n = 0;
        cimplog_debug("WRP-C: ", "headers count returned is %d\n", ( int ) message->u.crud.headers->count );

        if( NULL != meta_payload.u.crud.headers ) {
            while( n < meta_payload.u.crud.headers->count ) {
                CU_ASSERT_STRING_EQUAL( meta_payload.u.crud.headers->headers[n],
                                        message->u.crud.headers->headers[n] );
                n++;
            }
        } else {
            CU_ASSERT( false );
        }
    }

    cimplog_debug("WRP-C: ", "decoded msgType:%d\n", message->msg_type );
    cimplog_debug("WRP-C: ", "decoded source:%s\n", message->u.crud.source );
    cimplog_debug("WRP-C: ", "decoded dest:%s\n", message->u.crud.dest );
    cimplog_debug("WRP-C: ", "decoded transaction_uuid:%s\n", message->u.crud.transaction_uuid );
    wrp_free_struct( message );
    cimplog_debug("WRP-C: ", "********* Authorization ********\n" );
    const wrp_msg_t auth_msg = {
        .msg_type = WRP_MSG_TYPE__AUTH,
        .u.auth.status = 200
    };
    size = wrp_struct_to( &auth_msg, WRP_BYTES, &bytes );
    /* print the encoded message */
    _internal_tva_xxd( bytes, size, 0 );
    // msgpck decode
    rv = wrp_to_struct( bytes, size, WRP_BYTES, &message );
    free( bytes );
    CU_ASSERT_EQUAL( rv, size );
    CU_ASSERT_EQUAL( message->msg_type, auth_msg.msg_type );
    CU_ASSERT_EQUAL( message->u.auth.status, auth_msg.u.auth.status );
    cimplog_debug("WRP-C: ", "message->msg_type %d\n", message->msg_type );
    cimplog_debug("WRP-C: ", "message->u.auth.status %d\n", message->u.auth.status );
    wrp_free_struct( message );
    cimplog_debug("WRP-C: ", "******** METADATA packing **********\n" );
    // Failure case
    struct data metadata_null[2];
    memset(metadata_null,0,sizeof(struct data));
    const data_t metapackNull = {0, metadata_null};
    //message encode
    size = wrp_pack_metadata( &metapackNull , &bytes );

    if( size > 0 ) {
        _internal_tva_xxd( bytes, size, 0 );
        // Free only encode succeds
        free( bytes );
    } else {
        CU_ASSERT( true );
    }

    // Success case
    struct data meta_pack[] = {{"firmware", "PROD-DEV"}, {"model", "TG1680"}};
    const data_t metapack = {2, meta_pack};
    size = wrp_pack_metadata( &metapack , &bytes );

    if( size > 0 ) {
        _internal_tva_xxd( bytes, size, 0 );
        CU_ASSERT( true );
        free( bytes );
    } else {
        cimplog_error("WRP-C: ", "Metada Encoding failed\n " );
        CU_ASSERT( false );
    }

    cimplog_debug("WRP-C: ", "******** Append Metadata packing **********\n" );
    // Append Encoded data
    wrp_msg_t eventMsg, *finalMsg;
    void *metadataPack, *encodedData;
    size_t encodedSize;
    eventMsg.msg_type = WRP_MSG_TYPE__EVENT;
    eventMsg.u.event.source = "mac:format/iot" ;
    eventMsg.u.event.dest = "dns:scytale.webpa.comcast.net/iot" ;
    eventMsg.u.event.headers = NULL ;
    eventMsg.u.event.content_type = "application/json" ;
    eventMsg.u.event.metadata = NULL ;
    eventMsg.u.event.payload = "0123456789";
    eventMsg.u.event.payload_size = 10;
    // msgpck encode
    size = wrp_struct_to( &eventMsg, WRP_BYTES, &bytes );
    // msgpck decode
    rv = wrp_to_struct( bytes, size, WRP_BYTES, &message );

    if( rv > 0 ) {
        CU_ASSERT_EQUAL( rv, size );
        CU_ASSERT_EQUAL( message->msg_type, eventMsg.msg_type );
        CU_ASSERT_STRING_EQUAL( message->u.event.source, eventMsg.u.event.source );
        CU_ASSERT_STRING_EQUAL( message->u.event.dest, eventMsg.u.event.dest );
        CU_ASSERT_STRING_EQUAL( message->u.event.payload, eventMsg.u.event.payload );
        CU_ASSERT_EQUAL( message->u.event.payload_size, eventMsg.u.event.payload_size );
        cimplog_debug("WRP-C: ", "decoded event source:%s\n", message->u.event.source );
        cimplog_debug("WRP-C: ", "decoded event dest:%s\n", message->u.event.dest );
        cimplog_debug("WRP-C: ", "decoded event content_type:%s\n", message->u.event.content_type );
        cimplog_debug("WRP-C: ", "decoded event payload:%s\n", ( char * ) message->u.event.payload );
        cimplog_debug("WRP-C: ", "decoded event payload_size:%zu\n", message->u.event.payload_size );
        size = wrp_pack_metadata( &metapack , &metadataPack );

        if( size > 0 ) {
            _internal_tva_xxd( metadataPack, size, 0 );
            encodedSize = appendEncodedData( &encodedData, bytes, rv, metadataPack, size );

            if( encodedSize > 0 ) {
                // verify msgpck decode
                rv = wrp_to_struct( encodedData, encodedSize, WRP_BYTES, &finalMsg );

                if( rv > 0 ) {
                    CU_ASSERT_EQUAL( message->msg_type, eventMsg.msg_type );
                    CU_ASSERT_STRING_EQUAL( message->u.event.source, finalMsg->u.event.source );
                    CU_ASSERT_STRING_EQUAL( message->u.event.dest, finalMsg->u.event.dest );
                    CU_ASSERT_STRING_EQUAL( message->u.event.payload, finalMsg->u.event.payload );
                    CU_ASSERT_EQUAL( message->u.event.payload_size, finalMsg->u.event.payload_size );
                    if( message->u.event.metadata != NULL ) 
                    {
                        size_t n = 0;
                        while( n < finalMsg->u.event.metadata->count ) 
                        {
                            CU_ASSERT_STRING_EQUAL( metapack.data_items[n].name, finalMsg->u.event.metadata->data_items[n].name );
                            CU_ASSERT_STRING_EQUAL( metapack.data_items[n].value, finalMsg->u.event.metadata->data_items[n].value );
                            cimplog_info("WRP-C: ", "Append Metadata Key value pair: %s = %s\n",finalMsg->u.event.metadata->data_items[n].name,finalMsg->u.event.metadata->data_items[n].value);
                            n++;
                        }
                    }
                    cimplog_debug("WRP-C: ", "Complete Encode and Decode for appended metada is successfull ;) \n" );
                    cimplog_debug("WRP-C: ", "decoded final event source:%s\n", finalMsg->u.event.source );
                    cimplog_debug("WRP-C: ", "decoded final event dest:%s\n", finalMsg->u.event.dest );
                    cimplog_debug("WRP-C: ", "decoded final event content_type:%s\n", finalMsg->u.event.content_type );
                    cimplog_debug("WRP-C: ", "decoded final payload:%s\n", ( char * ) finalMsg->u.event.payload );
                    cimplog_debug("WRP-C: ", "decoded final payload_size:%zu\n", finalMsg->u.event.payload_size );
                } else {
                    cimplog_error("WRP-C: ", "Decode failed for appended data\n" );
                    CU_ASSERT( false );
                }
            }

            free( bytes );
            free( metadataPack );
            free( encodedData );
        } else {
            cimplog_error("WRP-C: ", "Metada Encoding failed for append encode data\n " );
            CU_ASSERT( false );
        }
    }

    wrp_free_struct( finalMsg );
    wrp_free_struct( message );
}

void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "wrp-c encoding tests", NULL, NULL );
    CU_add_test( *suite, "Test conversions", test_all );
    //CU_add_test( *suite, "Test struct_to_bytes()", test_to_bytes );
    //CU_add_test( *suite, "Test encode_decode()", test_encode_decode );
    CU_add_test( *suite, "Test CRUD message", test_crud_message );
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main( void )
{
    unsigned rv = 1;
    CU_pSuite suite = NULL;

    if( CUE_SUCCESS == CU_initialize_registry() ) {
        add_suites( &suite );

        if( NULL != suite ) {
            CU_basic_set_mode( CU_BRM_VERBOSE );
            CU_basic_run_tests();
            printf( "\n" );
            CU_basic_show_failures( CU_get_failure_list() );
            printf( "\n\n" );
            rv = CU_get_number_of_tests_failed();
        }

        CU_cleanup_registry();
    }

    if( 0 != rv ) {
        return 1;
    }

    return 0;
}

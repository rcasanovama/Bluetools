#ifndef __BLUETOOTH_OBEX_TYPES_H
#define __BLUETOOTH_OBEX_TYPES_H

#if defined(__cplusplus)
extern "C"
{
#endif

#include <stdint.h>

/**
 * OBEX Operations and Opcode definitions
 */

#define OBEX_CONNECT                0x80
#define OBEX_DISCONNECT             0x81

#define OBEX_PUT                    0x02
#define OBEX_PUT_FINAL              0x82
#define OBEX_GET                    0x03
#define OBEX_GET_FINAL              0x83

#define OBEX_SETPATH                0x85
#define OBEX_SESSION                0x87
#define OBEX_ABORT                  0xFF


/**
 * Response Code values
 */

#define OBEX_CONTINUE               0x90
#define OBEX_SUCCESS                0xA0
#define OBEX_CREATED                0xA1
#define OBEX_ACCEPTED               0xA2
#define OBEX_NON_AUTHORITATIVE      0xA3
#define OBEX_NO_CONTENT             0xA4
#define OBEX_RESET_CONTENT          0xA5
#define OBEX_PARTIAL_CONTENT        0xA6

#define OBEX_MULTIPLE_CHOICES       0xB0
#define OBEX_MOVED_PERMANENTLY      0xB1
#define OBEX_MOVED_TEMPORARILY      0xB2
#define OBEX_SEE_OTHER              0xB3
#define OBEX_NOT_MODIFIED           0xB4
#define OBEX_USE_PROXY              0xB5

#define OBEX_BAD_REQUEST            0xC0
#define OBEX_UNAUTHORIZED           0xC1
#define OBEX_PAYMENT_REQUIRED       0xC2
#define OBEX_FORBIDDEN              0xC3
#define OBEX_NOT_FOUND              0xC4
#define OBEX_METHOD_NOT_ALLOWED     0xC5
#define OBEX_NOT_ACCEPTABLE         0xC6
#define OBEX_PROXY_AUTH_REQUIRED    0xC7
#define OBEX_REQUEST_TIME_OUT       0xC8
#define OBEX_CONFLICT               0xC9
#define OBEX_GONE                   0xCA
#define OBEX_LENGTH_REQUIRED        0xCB
#define OBEX_PRECONDITION_FAILED    0xCC
#define OBEX_REQ_ENTITY_TOO_LARGE   0xCD
#define OBEX_REQ_URL_TOO_LARGE      0xCE
#define OBEX_UNSUPPORTED_MEDIA_TYPE 0xCF

#define OBEX_INTERNAL_SERVER_ERROR  0xD0
#define OBEX_NOT_IMPLEMENTED        0xD1
#define OBEX_BAD_GATEWAY            0xD2
#define OBEX_SERVICE_UNAVAILABLE    0xD3
#define OBEX_GATEWAY_TIMEOUT        0xD4
#define OBEX_VERSION_NOT_SUPPORTED  0xD5

#define OBEX_DATABASE_FULL          0xE0
#define OBEX_DATABASE_LOCKED        0xE1


/**
 * OBEX Headers
 */

#define OBEX_EMPTY                  0x00
#define OBEX_COUNT                  0xC0
#define OBEX_NAME                   0x01
#define OBEX_TYPE                   0x42
#define OBEX_LENGTH                 0xC3
#define OBEX_ISO_TIME               0x44
#define OBEX_COMPATIBILITY_TIME     0xC4
#define OBEX_DESCRIPTION            0x05
#define OBEX_TARGET                 0x46
#define OBEX_HTTP                   0x47
#define OBEX_BODY                   0x48
#define OBEX_BODY_END               0x49
#define OBEX_WHO                    0x4A
#define OBEX_CONNECTION_ID          0xCB
#define OBEX_APP_PARAMETERS         0x4C
#define OBEX_AUTH_CHALLENGE         0x4D
#define OBEX_AUTH_RESPONSE          0x4E
#define OBEX_CREATOR_ID             0xCF
#define OBEX_WAN_UUID               0x50
#define OBEX_OBJECT_CLASS           0x51
#define OBEX_SESSION_PARAMETERS     0x52
#define OBEX_SESSION_SEQ_NUMBER     0x93


/**
 * MTU values
 */

#define OBEX_DEFAULT_MTU            1024
#define OBEX_MINIMUM_MTU            255
#define OBEX_MAXIMUM_MTU            65535


#pragma pack(1)
struct obex_packet_t
{
	uint8_t opcode;
	uint16_t packet_length;

	struct obex_packet_info_t* info;
	struct obex_packet_header_t* headers;
};

struct obex_packet_info_t
{
	uint8_t version;
	uint8_t flags;
	uint16_t maximum_packet_length;
};

struct obex_packet_header_t
{
	uint8_t header_id;
	union
	{
		struct
		{
			uint64_t header_value;
		} basic;

		struct
		{
			uint16_t header_size;
			uint8_t* header_value;
		} extended;
	};

	struct obex_packet_header_t* next;
};
#pragma pack()

#define OBEX_MINIMUM_PACKET_SIZE    (sizeof(uint8_t) + sizeof(uint16_t))
#define OBEX_MAXIMUM_PACKET_SIZE    ((OBEX_MINIMUM_PACKET_SIZE) + sizeof(struct obex_packet_info_t))
#define OBEX_MINIMUM_HEADER_SIZE    (sizeof(uint8_t) + sizeof(uint16_t))

#define OFFSET_PACKET_LENGTH        (sizeof(uint8_t))
#define OFFSET_MAX_PACKET_LENGTH    (sizeof(uint8_t) + sizeof(uint8_t))
#define OFFSET_HEADER_LENGTH        (sizeof(uint8_t))
#define OFFSET_HEADER_VALUE         (sizeof(uint8_t) + sizeof(uint16_t))


#if defined(__cplusplus)

}
#endif

#endif // __BLUETOOTH_OBEX_TYPES_H
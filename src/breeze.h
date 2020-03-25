#ifndef BREEZE_LUA_BREEZE_H
#define BREEZE_LUA_BREEZE_H

#define BREEZE_OK 0
#define E_BREEZE_BUFFER_NOT_ENOUGH -1
#define E_BREEZE_OVERFLOW -2
#define E_BREEZE_UNSUPPORTED_TYPE -3
#define E_BREEZE_MEMORY_NOT_ENOUGH -4
#define E_BREEZE_WRONG_SIZE -5

#define BREEZE_MODNAME "breeze"
#ifndef BREEZE_REVISION
#define BREEZE_REVISION ""
#endif
#define BREEZE_VERSION "0.0.1" BREEZE_REVISION

#ifndef __unused
#define __unused __attribute__((unused))
#endif


#define	MaxWriteCount 0 
#define	MaxElemSize 100000

typedef enum {
	StringType              = 0x3f,
	DirectStringMinType     = 0x00,
	DirectStringMaxType     = 0x3e,
	Int32Type               = 0x7f,
	DirectInt32MinType      = 0x40,
	DirectInt32MaxType      = 0x7e,
	Int64Type               = 0x98,
	DirectInt64MinType      = 0x80,
	DirectInt64MaxType      = 0x97,
	NullType                = 0x99,
	TrueType                = 0x9a,
	FalseType               = 0x9b,
	ByteType                = 0x9c,
	BytesType               = 0x9d,
	Int16Type               = 0x9e,
	Float32Type             = 0x9f,
	Float64Type             = 0xa0,
	MapType                 = 0xd9,
	ArrayType               = 0xda,
	PackedMapType           = 0xdb,
	PackedArrayType         = 0xdc,
	SchemaType              = 0xdd,
	MessageType             = 0xde,
	RefMessageType          = 0xdf,
	DirectRefMessageMaxType = 0xff
} breeze_type_t;

typedef enum {
	Int32Zero                      = 0x50,
	Int64Zero                      = 0x88,
	DirectStringMaxLength          = DirectStringMaxType,
	DirectInt32MinValue            = DirectInt32MinType - Int32Zero,
	DirectInt32MaxValue            = DirectInt32MaxType - Int32Zero,
	DirectInt64MinValue            = DirectInt64MinType - Int64Zero,
	DirectInt64MaxValue            = DirectInt64MaxType - Int64Zero,
	DirectRefMessageMaxValue       = DirectRefMessageMaxType - RefMessageType
} breeze_direct_value_limit_t;

// type Field struct {
// 	Index int
// 	Name  string
// 	Type  string
// }

// Field describes a message field, include field index, field name and field type
//typedef struct {
//    uint16_t index;
//    const char *name;
//    const char *type;
//} field_t;
//
//typedef struct {
//    uint16_t index;
//    field_t *field;
//} index_field_map_t;
//
//typedef struct
//{
//    const char *name;
//    field_t *field;
//} name_field_map_t;
//
//// Schema describes a breeze message, include name, alias, all fields of message
//typedef struct {
//    const char *name;
//    const char *alias;
//    index_field_map_t *index_field_map;
//    name_field_map_t *name_field_map;
//} schema_t;
//
//extern field_t *
//get_field_by_index(schema_t *schema, uint16_t index);
//
//extern field_t *
//get_field_by_name(schema_t *schema, const char *name);
//
//extern void put_field(schema_t *schema, field_t *field);

// typedef struct
// {
//     const char *name;
//     const char *alias;
//     schema_t *schema;
//     field_t???
// };

extern const char *breeze_error(int err);

#endif //BREEZE_LUA_BREEZE_H

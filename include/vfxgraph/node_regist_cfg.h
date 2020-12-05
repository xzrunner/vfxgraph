#ifndef EXE_FILEPATH
#error "You must define EXE_FILEPATH macro before include this file"
#endif

// common

#define NO_FILEPATH_INCLUDE

#define PARM_NODE_TYPE Float
#define PARM_NODE_NAME float1
#include EXE_FILEPATH

#define PARM_NODE_TYPE Float2
#define PARM_NODE_NAME float2
#include EXE_FILEPATH

#define PARM_NODE_TYPE Float3
#define PARM_NODE_NAME float3
#include EXE_FILEPATH

#define PARM_NODE_TYPE Float4
#define PARM_NODE_NAME float4
#include EXE_FILEPATH

#undef NO_FILEPATH_INCLUDE

#define PARM_NODE_TYPE Texture
#define PARM_NODE_NAME texture
#include EXE_FILEPATH

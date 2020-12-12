#ifndef EXE_FILEPATH
#error "You must define EXE_FILEPATH macro before include this file"
#endif

#undef NO_PARM_FILEPATH

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

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE Add
#define PARM_NODE_NAME add
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE Sub
#define PARM_NODE_NAME sub
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE Mul
#define PARM_NODE_NAME mul
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE Div
#define PARM_NODE_NAME div
#include EXE_FILEPATH

#undef NO_FILEPATH_INCLUDE

#define PARM_NODE_TYPE Texture
#define PARM_NODE_NAME texture
#include EXE_FILEPATH

// fluid2d

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE Copy
#define PARM_NODE_NAME copy
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE AddSplat
#define PARM_NODE_NAME add_splat
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE RKAdvect
#define PARM_NODE_NAME rk_advect
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE MCAdvect
#define PARM_NODE_NAME mc_advect
#include EXE_FILEPATH

#define PARM_NODE_TYPE RBMethod
#define PARM_NODE_NAME rb_method
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE MaxReduce
#define PARM_NODE_NAME max_reduce
#include EXE_FILEPATH

#define PARM_NODE_TYPE BuoyantForce
#define PARM_NODE_NAME buoyant_force
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE DivCurl
#define PARM_NODE_NAME div_curl
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE ApplyVorticity
#define PARM_NODE_NAME apply_vorticity
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE UpdateQAndTheta
#define PARM_NODE_NAME update_q_and_theta
#include EXE_FILEPATH

#define PARM_NODE_TYPE SolvePressure
#define PARM_NODE_NAME solve_pressure
#include EXE_FILEPATH

#define NO_PARM_FILEPATH
#define PARM_NODE_TYPE PressureProjection
#define PARM_NODE_NAME pressure_projection
#include EXE_FILEPATH

#include "ops/gol.h"

#include "helpers.h"
#include "teletype.h"
#include "teletype_io.h"

static void op_GOL_GEN_get(const void* NOTUSED(data), scene_state_t* ss, exec_state_t* NOTUSED(es), command_state_t* NOTUSED(cs))
{
    gol_next_gen(ss);
    tele_gol_updated();
}

// Make ops
const tele_op_t op_GOL_GEN = MAKE_GET_OP(GOL.GEN, op_GOL_GEN_get, 0, false);
#include "environment.hpp"
#include "builtin/object.hpp"

using namespace rubinius;

typedef Environment* rbx_ctx;
typedef Object* rbx_obj;

#define RUBINIUS_CONST(ctx, n) ctx->state->globals().rubinius.get()->get_const(ctx->state, ctx->state->symbol(n));

extern "C"{
/**
 * Create a embedded rubinius environment
 */
rbx_ctx rbx_create_context();

/**
 * Requires a ruby file (like require 'file' in ruby)
 *
 * Returns true if it executed without exception, false if there was an error.
 * Finding out what went wrong will be defined later.
 */
bool rbx_require_file(rbx_ctx ctx, const char * file);

/**
 * Evaulate a ruby file
 *
 * Returns true if it executed without exception, false if there was an error.
 * Finding out what went wrong will be defined later.
 */
bool rbx_eval_file(rbx_ctx ctx, const char * file);

/**
 * Evaluate a ruby string
 *
 * Returns true if it executed without exception, false if there was an error.
 * Finding out what went wrong will be defined later.
 */
bool rbx_eval(rbx_ctx ctx, const char * code);

/**
 * Send a message to an object
 * 
 * Send +msg+ to object +rcv+ with arguments specified by +argc+ and the
 * variadic parameters that follow. All arguments *must* be of type rbx_obj.
 */
rbx_obj rbx_send(rbx_ctx ctx, rbx_obj recv, const char* msg, int argc, ...);

/**
 * Acquire global lock
 */
void rbx_lock(rbx_ctx ctx);

/**
 * Release global lock
 */
void rbx_unlock(rbx_ctx ctx);

/**
 * Clean up a context
 */
void rbx_close_ctx(rbx_ctx ctx);
}

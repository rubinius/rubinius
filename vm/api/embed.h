#include "environment.hpp"

typedef Environment* rbx_ctx;

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
 * Clean up a context
 */
void rbx_close_ctx(rbx_ctx ctx);
}

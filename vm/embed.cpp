#include "api/embed.h"

#include "builtin/object.hpp"
#include "builtin/string.hpp"
#include "builtin/array.hpp"
#include "builtin/module.hpp"

rbx_ctx rbx_create_context() {
  Environment* ctx = new Environment(0, NULL);
  ctx->setup_cpp_terminate();
  
  const char* runtime = getenv("RBX_RUNTIME");
  if(!runtime) runtime = RBX_RUNTIME;
  
  const char* lib_path = getenv("RBX_LIB_PATH");
  if(!lib_path) lib_path = RBX_LIB_PATH;
  
  std::string runtime_str(runtime);
  std::string lib_path_str(lib_path);
  
  int i = 0;
  ctx->state->set_stack_start(&i);
  
  ctx->load_platform_conf(runtime);
  ctx->boot_vm();
  ctx->state->initialize_config();
  ctx->load_tool();
  ctx->load_kernel(runtime);
  ctx->start_signals();
  
  ctx->run_file(runtime_str + "/loader.rbc");
  ctx->run_file(runtime_str + "/embed_loader.rbc");
  
  rbx_obj loader = RUBINIUS_CONST(ctx, "EmbedLoader");
  rbx_send(ctx, loader, "load_compiler", 0);
  
  return ctx;
}

bool rbx_require_file(rbx_ctx ctx, const char * file) {
  rbx_obj loader = RUBINIUS_CONST(ctx, "EmbedLoader");
  rbx_send(ctx, loader, "require_file", 1, String::create(ctx->state, file));
  
  return true;
}

bool rbx_eval_file(rbx_ctx ctx, const char * file) {
  rbx_obj loader = RUBINIUS_CONST(ctx, "EmbedLoader");
  rbx_send(ctx, loader, "eval_file", 1, String::create(ctx->state, file));
  
  return true;
}

bool rbx_eval(rbx_ctx ctx, const char * code) {
  rbx_obj loader = RUBINIUS_CONST(ctx, "EmbedLoader");
  rbx_send(ctx, loader, "eval_script", 1, String::create(ctx->state, code));
  
  return true;
}

rbx_obj rbx_send(rbx_ctx ctx, rbx_obj recv, const char* msg, int argc, ...) {
  rbx_lock(ctx);
  va_list args;
  va_start(args, argc);
  
  Array* arg_ary = Array::create(ctx->state, argc);
  for(int i=0; i<argc; i++)
    arg_ary->append(ctx->state, va_arg(args, Object*));
  
  va_end(args);
  
  rbx_obj ret = recv->send(ctx->state, 0, ctx->state->symbol(msg), arg_ary);
  rbx_unlock(ctx);
  return ret;
}

void rbx_lock(rbx_ctx ctx) {
  ctx->state->global_lock().take();
}

void rbx_unlock(rbx_ctx ctx) {
  ctx->state->global_lock().drop();
}

void rbx_close_ctx(rbx_ctx ctx) {
  ctx->halt();
  delete ctx;
}


#ifndef __STATE_H__
#define __STATE_H__

/* class.c */
#define rb_class_tbl STATE_VAR(rb_class_tbl)

/* dir.c */
#define chdir_blocking STATE_VAR(chdir_blocking)
#define chdir_thread STATE_VAR(chdir_thread)

/* dln.c */
#define dln_init_p STATE_VAR(dln_init_p)
#define dln_errno STATE_VAR(dln_errno)

/* eval.c */
#define ruby_frame STATE_VAR(frames)
#define ruby_scope STATE_VAR(ruby_scope)
#define top_frame STATE_VAR(top_frame)
#define top_scope STATE_VAR(top_scope)
#define ruby_top_self STATE_VAR(ruby_top_self)
#define ruby_in_eval STATE_VAR(ruby_in_eval)
#define ruby_class STATE_VAR(ruby_class)
#define ruby_dyna_vars STATE_VAR(ruby_dyna_vars)
#define scope_vmode STATE_VAR(scope_vmode)
#define ruby_current_node STATE_VAR(ruby_current_node)
#define ruby_safe_level STATE_VAR(safe_level)
#define ruby_running STATE_VAR(ruby_running)
#define ruby_errinfo STATE_VAR(ruby_errinfo)
#define frame_unique STATE_VAR(frame_unique)
#define ruby_iter STATE_VAR(ruby_iter)
#define ruby_wrapper STATE_VAR(ruby_wrapper)
#define ruby_cref STATE_VAR(ruby_cref)
#define top_cref STATE_VAR(top_cref)
#define trace_func STATE_VAR(trace_func)
#define tracing STATE_VAR(tracing)
#define ruby_initialized STATE_VAR(initialized)
#define cont_protect STATE_VAR(cont_protect)
#define last_call_status STATE_VAR(last_call_status)
#define rb_load_path STATE_VAR(rb_load_path)
#define ruby_dln_librefs STATE_VAR(ruby_dln_librefs)
#define rb_features STATE_VAR(rb_features)
#define loading_tbl STATE_VAR(loading_tbl)
#define end_procs STATE_VAR(end_procs)
#define ephemeral_end_procs STATE_VAR(ephemeral_end_procs)
#define tmp_end_procs STATE_VAR(tmp_end_procs)
#define ruby_block STATE_VAR(ruby_block)
#define block_unique STATE_VAR(block_unique)
#define prot_tag STATE_VAR(prot_tag)
#define rb_last_status STATE_VAR(rb_last_status)

/* eval.c (should be thread.c) */
#define curr_thread STATE_VAR(current_thread)
#define main_thread STATE_VAR(main_thread)
#define ruby_thid STATE_VAR(primary_thid)
#define rb_thread_pending STATE_VAR(thread_pending)
#define th_raise_exception STATE_VAR(th_raise_exception)
#define th_raise_node STATE_VAR(th_raise_node)
#define th_cmd STATE_VAR(th_cmd)
#define th_sig STATE_VAR(th_sig)
#define th_safe STATE_VAR(th_safe)
#define th_signm STATE_VAR(th_signm)
#define rb_thread_critical STATE_VAR(thread_critical)
#define rb_trap_immediate STATE_VAR(trap_immediate)
#define rb_trap_pending STATE_VAR(trap_pending)
#define rb_prohibit_interrupt STATE_VAR(prohibit_interrupt)
/* error.c */
#define ruby_nerrs STATE_VAR(ruby_nerrs)
#define syserr_tbl STATE_VAR(syserr_tbl)

/* gc.c */
#define global_List STATE_VAR(gc->global_List)
#define heaps STATE_VAR(gc->heaps)
#define malloc_increase STATE_VAR(gc->malloc_increase)
#define malloc_limit STATE_VAR(gc->malloc_limit)
#define dont_gc STATE_VAR(gc->dont_gc)
#define during_gc STATE_VAR(gc->during_gc)
#define need_call_final STATE_VAR(gc->need_call_final)
#define finalizer_table STATE_VAR(gc->finalizer_table)
#define freelist STATE_VAR(gc->freelist)
#define deferred_final_list STATE_VAR(gc->deferred_final_list)
#define heaps_length STATE_VAR(gc->heaps_length)
#define heaps_used STATE_VAR(gc->heaps_used)
#define heap_slots STATE_VAR(gc->heap_slots)
#define himem STATE_VAR(gc->gc_himem)
#define lomem STATE_VAR(gc->gc_lomem)
#define rb_gc_stack_start STATE_VAR(gc_stack_start)
#define mark_stack STATE_VAR(gc->mark_stack)
#define mark_stack_ptr STATE_VAR(gc->mark_stack_ptr)
#define mark_stack_overflow STATE_VAR(gc->mark_stack_overflow)
#define source_filenames STATE_VAR(gc->source_filenames)
#define finalizers STATE_VAR(gc->finalizers)

/* hash.c */
#define envtbl STATE_VAR(envtbl)
#define path_tainted STATE_VAR(path_tainted)

/* parse.y */
#define ruby_eval_tree_begin STATE_VAR(ruby_eval_tree_begin)
#define ruby_eval_tree STATE_VAR(ruby_eval_tree)
#define ruby_sourcefile STATE_VAR(ruby_sourcefile)
#define ruby_sourceline STATE_VAR(ruby_sourceline)
#define ruby_in_compile STATE_VAR(ruby_in_compile)
#define lvtbl STATE_VAR(lvtbl)
#define sym_tbl STATE_VAR(sym_tbl)
#define sym_rev_tbl STATE_VAR(sym_rev_tbl)

/* ruby.c */
#define ruby_debug STATE_VAR(ruby_debug)
#define ruby_verbose STATE_VAR(ruby_verbose)
#define sflag STATE_VAR(sflag)
#define xflag STATE_VAR(xflag)
#define rb_progname STATE_VAR(progname)
#define rb_argv STATE_VAR(argv)
#define rb_argv0 STATE_VAR(argv0)
#define ruby__end__seen STATE_VAR(end_seen)

/* variable.c */
#define rb_global_tbl STATE_VAR(rb_global_tbl)

/* runtime.c */
#define ruby_errhandler STATE_VAR(ruby_errhandler)

/* everywhere */
#define rb_rs STATE_VAR(record_seperator)
#define rb_output_fs STATE_VAR(output_field_seperator)
#define rb_output_rs STATE_VAR(output_record_seperator)
#define rb_default_rs STATE_VAR(default_record_seperator)
#define rb_stderr STATE_VAR(ruby_stderr)
#define rb_stdin STATE_VAR(ruby_stdin)
#define rb_stdout STATE_VAR(ruby_stdout)
#define rb_deferr STATE_VAR(ruby_deferr)
#define current_file STATE_VAR(current_file)
#define filename STATE_VAR(filename)
#define gets_lineno STATE_VAR(gets_lineno)
#define ruby_inplace_mode STATE_VAR(ruby_inplace_mode)
#define init_p STATE_VAR(init_p)
#define next_p STATE_VAR(next_p)

#endif

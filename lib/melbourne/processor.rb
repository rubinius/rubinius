module Rubinius
  class Melbourne
    attr_accessor :top
    attr_accessor :current

    def initialize
      @current = @top = Compiler::Node::AST.new(nil)
    end

    def insert(node)
      # TODO: delegate to node
      @current = node
    end

    def process_leave(node)
      # TODO: delegate to node
      @current = node if node
    end

    def process_dangling_node
      puts "Processing called but node was NULL"
      # TODO: output info about the current AST node
    end

    # This method is analogous to #method_missing. It is called
    # if there is no processing method defined for a node.
    def process_missing_node(file, line, node_name, node_type)
      puts "Unhandled node #{node_name} (#{node_type})"
    end

    # TODO: remove when all processors are defined
    def method_missing(sym, *args)
      puts "#{sym} #{args.map { |x| x.inspect}.join(", ")}"
    end


    # Processing methods

    def process_alias(file, line, to, from)
      # insert [:alias, [:lit, to], [:lit, from]]
    end

    def process_and(file, line)
    end

    def process_args(file, line)
    end

    def process_argscat(file, line)
    end

    def process_argspush(file, line)
    end

    def process_array(file, line)
    end

    def process_attrasgn(file, line, sym)
    end

    def process_attrset(file, line, sym)
    end

    def process_back_ref(file, line, ref)
    end

    def process_begin(file, line)
    end

    def process_block(file, line)
      # insert [:block]
    end

    def process_block_arg(file, line, foo)
    end

    def process_block_pass(file, line)
    end

    def process_break(file, line)
    end

    def process_call(file, line, arg)
    end

    def process_case(file, line)
    end

    def process_cdecl( file, line)
    end

    def process_class(file, line)
    end

    def process_colon2(file, line, sym)
    end

    def process_colon3(file, line, sym)
      # insert [:colon3, [:lit, sym]]
    end

    def process_const(file, line, sym)
    end

    def process_cvar(file, line, sym)
    end

    def process_cvasgn(file, line, sym)
    end

    def process_cvdecl(file, line, sym)
    end

    def process_defined(file, line)
    end

    def process_defn(file, line, sym)
    end

    def process_defs(file, line, sym)
    end

    def process_dot2(file, line)
    end

    def process_dot3(file, line)
    end

    def process_dvar(file, line, sym)
    end

    def process_ensure(file, line)
    end

    def process_evstr(file, line)
    end

    def process_false(file, line)
    end

    def process_fbody(file, line)
    end

    def process_fcall(file, line, arg)
    end

    def process_fixnum(file, line, value)
      # insert [:lit, value]
    end

    def process_flip2(file, line)
    end

    def process_flip3(file, line)
    end

    def process_float(file, line, str)
      # insert [:lit, str.to_f]
    end

    def process_for(file, line)
    end

    def process_gasgn(file, line, sym)
    end

    def process_gvar(file, line, sym)
    end

    def process_hash(file, line)
    end

    def process_iasgn(file, line, sym)
    end

    def process_if(file, line)
    end

    def process_iter(file, line)
    end

    def process_ivar(file, line, sym)
    end

    def process_lasgn(file, line, sym)
      # insert [:lasgn, sym]
    end

    def process_lit(file, line, sym)
      # insert [:lit, sym]
    end

    def process_lvar(file, line, sym)
    end

    def process_masgn(file, line)
    end

    def process_match(file, line, regexp, flag)
    end

    def process_match2(file, line)
    end

    def process_match3(file, line)
    end

    def process_method(file, line)
    end

    def process_module(file, line)
    end

    def process_negate(file, line)
    end

    def process_next(file, line)
    end

    def process_nil(file, line)
      # insert [:nil]
    end

    def process_not(file, line)
    end

    def process_nth_ref(file, line, ref)
    end

    def process_number(file, line, base, str)
      # insert [:lit, str.to_i(base)]
    end

    def process_op_asgn1(file, line)
    end

    def process_op_asgn2(file, line)
    end

    def process_op_asgn_and(file, line)
    end

    def process_op_asgn_or(file, line)
    end

    def process_opt_n(file, line)
    end

    def process_or(file, line)
    end

    def process_redo(file, line)
    end

    def process_regex(file, line, regexp, flags)
    end

    def process_resbody(file, line)
    end

    def process_rescue(file, line)
    end

    def process_retry(file, line)
    end

    def process_return(file, line)
    end

    def process_sclass(file, line)
    end

    def process_scope(file, line)
    end

    def process_self(file, line)
    end

    def process_splat(file, line)
    end

    def process_str(file, line, foo)
    end

    def process_super(file, line)
    end

    def process_svalue(file, line)
    end

    def process_to_ary(file, line)
    end

    def process_true(file, line)
    end

    def process_undef(file, line, sym)
      # insert [:undef, [:lit, sym]]
    end

    def process_until(file, line)
    end

    def process_valias(file, line, to, from)
      # insert [:valias, to, from]
    end

    def process_vcall(file, line, arg)
    end

    def process_when(file, line)
    end

    def process_while(file, line)
    end

    def process_xstr(file, line, str)
    end

    def process_yield(file, line)
    end

    def process_zarray(file, line)
    end

    def process_zsuper(file, line)
    end
  end
end


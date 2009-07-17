module Rubinius
  class Melbourne

    AST = Compiler::Node

    attr_accessor :top
    attr_accessor :current
    attr_accessor :compiler

    def initialize(name)
      @name = name
      @compiler = nil # Compiler.new TestGenerator
      @current = @top = AST::Top.new(nil)
    end

    def syntax_error
      raise @exc if @exc
    end

    def insert(node)
      # TODO: delegate to node
      @current.add node
    end

    def process_parse_error(message, column, line, source)
      @exc = SyntaxError.new message
      @exc.import_position column, line, source
      @exc.file = @name
    end

    def process_dangling_node
      puts "Processing called but node was NULL"
      # TODO: output info about the current AST node
    end

    # This method is analogous to #method_missing. It is called
    # if there is no processing method defined for a node.
    def process_missing_node(line, node_name, node_type)
      puts "Unhandled node #{node_name} (#{node_type})"
    end

    # TODO: remove when all processors are defined
    def method_missing(sym, *args)
      #puts "#{sym} #{args.map { |x| x.inspect}.join(", ")}"
    end


    # Processing methods

    def process_alias(line, to, from)
      # insert [:alias, [:lit, to], [:lit, from]]
    end

    def process_and(line)
    end

    def process_args(line)
    end

    def process_argscat(line)
    end

    def process_argspush(line)
    end

    def process_array(line)
    end

    def process_attrasgn(line, sym)
    end

    def process_attrset(line, sym)
    end

    def process_back_ref(line, ref)
    end

    def process_begin(line)
    end

    def process_block(line)
      # insert [:block]
    end

    def process_block_arg(line, foo)
    end

    def process_block_pass(line)
    end

    def process_break(line)
    end

    def process_call(line, arg)
    end

    def process_case(line)
    end

    def process_cdecl( line)
    end

    def process_class(line)
    end

    def process_colon2(line, sym)
    end

    def process_colon3(line, sym)
      # insert [:colon3, [:lit, sym]]
    end

    def process_const(line, sym)
    end

    def process_cvar(line, sym)
    end

    def process_cvasgn(line, sym)
    end

    def process_cvdecl(line, sym)
    end

    def process_defined(line)
    end

    def process_defn(line, sym)
    end

    def process_defs(line, sym)
    end

    def process_dot2(line)
    end

    def process_dot3(line)
    end

    def process_dvar(line, sym)
    end

    def process_ensure(line)
    end

    def process_evstr(line)
    end

    def process_false(line)
    end

    def process_fbody(line)
    end

    def process_fcall(line, arg)
    end

    def process_fixnum(line, value)
      # insert [:lit, value]
    end

    def process_flip2(line)
    end

    def process_flip3(line)
    end

    def process_float(line, str)
      # insert [:lit, str.to_f]
    end

    def process_for(line)
    end

    def process_gasgn(line, sym)
    end

    def process_gvar(line, sym)
    end

    def process_hash(line)
    end

    def process_iasgn(line, sym)
    end

    def process_if(line)
    end

    def process_iter(line)
    end

    def process_ivar(line, sym)
    end

    def process_lasgn(line, sym)
      # insert [:lasgn, sym]
    end

    def process_lit(line, sym)
      # insert [:lit, sym]
    end

    def process_lvar(line, sym)
    end

    def process_masgn(line)
    end

    def process_match(line, regexp, flag)
    end

    def process_match2(line)
    end

    def process_match3(line)
    end

    def process_method(line)
    end

    def process_module(line)
    end

    def process_negate(line)
    end

    def process_next(line)
    end

    def process_nil(line)
      # insert [:nil]
    end

    def process_not(line)
    end

    def process_nth_ref(line, ref)
    end

    def process_number(line, base, str)
      # insert [:lit, str.to_i(base)]
      number = AST::Literal.new @compiler
      number = number.normalize(str.to_i(base))
      insert number
    end

    def process_op_asgn1(line)
    end

    def process_op_asgn2(line)
    end

    def process_op_asgn_and(line)
    end

    def process_op_asgn_or(line)
    end

    def process_opt_n(line)
    end

    def process_or(line)
    end

    def process_redo(line)
    end

    def process_regex(line, regexp, flags)
    end

    def process_resbody(line)
    end

    def process_rescue(line)
    end

    def process_retry(line)
    end

    def process_return(line)
    end

    def process_sclass(line)
    end

    def process_scope(line)
    end

    def process_self(line)
    end

    def process_splat(line)
    end

    def process_str(line, foo)
    end

    def process_super(line)
    end

    def process_svalue(line)
    end

    def process_to_ary(line)
    end

    def process_true(line)
    end

    def process_undef(line, sym)
      # insert [:undef, [:lit, sym]]
    end

    def process_until(line)
    end

    def process_valias(line, to, from)
      # insert [:valias, to, from]
    end

    def process_vcall(line, arg)
    end

    def process_when(line)
    end

    def process_while(line)
    end

    def process_xstr(line, str)
    end

    def process_yield(line)
    end

    def process_zarray(line)
    end

    def process_zsuper(line)
    end
  end
end


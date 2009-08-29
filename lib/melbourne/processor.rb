require "melbourne/nodes"

module Rubinius
  class Melbourne

    AST = Compiler::Node

    attr_accessor :compiler

    def initialize(name)
      @name = name
      @compiler = Compiler.new TestGenerator
    end

    def syntax_error
      raise @exc if @exc
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
      puts " *** missing #{sym} #{args.map { |x| x.inspect}.join(", ")}"
    end


    # Processing methods

    def process_alias(line, to, from)
      AST::Alias.from self, to, from
    end

    def process_and(line, left, right)
      AST::And.from self, left, right
    end

    def process_args(line, args, defaults, splat)
      AST::FormalArguments.from self, args, defaults, splat
    end

    def process_argscat(line, array, rest)
      AST::ConcatArgs.from self, array, rest
    end

    def process_argspush(line, arguments, value)
      AST::PushArgs.from self, arguments, value
    end

    def process_array(line, array)
      AST::ArrayLiteral.from self, array
    end

    def process_attrasgn(line, receiver, name, arguments)
      AST::AttrAssign.from self, receiver, name, arguments
    end

    def process_back_ref(line, ref)
      AST::BackRef.from self, ref
    end

    def process_begin(line, body)
      AST::Begin.from self, body
    end

    def process_block(line, array)
      AST::Block.from self, array
    end

    def process_block_arg(line, name)
      AST::BlockArgument.from self, name
    end

    def process_block_pass(line, method_send, block)
      method_send.block = AST::BlockPass.from self, block
      method_send
    end

    def process_break(line, expr)
      AST::Break.from self, expr
    end

    def process_call(line, receiver, name, arguments)
      if arguments
        AST::SendWithArguments.from self, receiver, name, arguments, false
      else
        AST::Send.from self, receiver, name, false
      end
    end

    def process_case(line, receiver, whens, else_body)
      if receiver
        AST::ReceiverCase.from self, receiver, whens, else_body
      else
        AST::Case.from self, whens, else_body
      end
    end

    def process_cdecl(line, expr, value)
      AST::ConstSet.from self, expr, value
    end

    def process_class(line, name, superclass, body)
      AST::ClassWrapper.from self, name, superclass, body
    end

    def process_colon2(line, outer, name)
      if outer
        AST::ConstAccess.from self, outer, name
      else
        AST::ConstFind.from self, name
      end
    end

    def process_colon3(line, name)
      AST::ConstAtTop.from self, name
    end

    def process_const(line, name)
      AST::ConstFind.from self, name
    end

    def process_cvar(line, name)
      AST::CVar.from self, name
    end

    def process_cvasgn(line, name, value)
      AST::CVarAssign.from self, name, value
    end

    def process_cvdecl(line, name, value)
      AST::CVarDeclare.from self, name, value
    end

    def process_defined(line, expr)
      AST::Defined.from self, expr
    end

    def process_defn(line, name, body)
      AST::Define.from self, name, body
    end

    def process_defs(line, receiver, name, body)
      AST::DefineSingleton.from self, receiver, name, body
    end

    def process_dot2(line, start, finish)
      AST::Range.from self, start, finish
    end

    def process_dot3(line, start, finish)
      AST::RangeExclude.from self, start, finish
    end

    def process_dregx(line, str, array, flags)
      AST::DynamicRegex.from self, str, array, flags
    end

    def process_dregx_once(line, str, array, flags)
      AST::DynamicOnceRegex.from self, str, array, flags
    end

    def process_dstr(line, str, array)
      AST::DynamicString.from self, str, array
    end

    def process_dsym(line, str, array)
      AST::DynamicSymbol.from self, str, array
    end

    def process_dxstr(line, str, array)
      AST::DynamicExecuteString.from self, str, array
    end

    def process_ensure(line, body, ensr)
      AST::Ensure.from self, body, ensr
    end

    def process_evstr(line, value)
      if value
        AST::ToString.from self, value
      else
        AST::StringLiteral.from self, ""
      end
    end

    def process_false(line)
      AST::False.from self
    end

    def process_fcall(line, name, arguments)
      receiver = AST::Self.from self
      if arguments
        AST::SendWithArguments.from self, receiver, name, arguments
      else
        AST::Send.from self, receiver, name
      end
    end

    def process_file(line)
      AST::File.from self
    end

    def process_fixnum(line, value)
      node = AST::NumberLiteral.new @compiler
      node.args value
      node
    end

    def process_flip2(line, start, finish)
      AST::Flip2.from self, start, finish
    end

    def process_flip3(line, start, finish)
      AST::Flip3.from self, start, finish
    end

    def process_float(line, str)
      AST::Float.from self, str
    end

    def process_for(line, iter, arguments, body)
      AST::For.from self, iter, arguments, body
    end

    def process_gasgn(line, name, expr)
      AST::GVarAssign.from self, name, expr
    end

    def process_gvar(line, name)
      AST::GVar.from self, name
    end

    def process_hash(line, array)
      AST::HashLiteral.from self, array
    end

    def process_iasgn(line, name, expr)
      AST::IVarAssign.from self, name, expr
    end

    def process_if(line, cond, body, else_body)
      AST::If.from self, cond, body, else_body
    end

    def process_iter(line, method_send, arguments, body)
      method_send.block = AST::Iter.from self, arguments, body
      method_send
    end

    def process_ivar(line, name)
      AST::IVar.from self, name
    end

    def process_lasgn(line, name, expr)
      AST::LocalAssignment.from self, name, expr
    end

    def process_lit(line, sym)
      AST::Literal.from self, sym
    end

    def process_lvar(line, name)
      AST::LocalAccess.from self, name
    end

    def process_masgn(line, left, right, splat)
      AST::MAsgn.from self, left, right, splat
    end

    def process_match(line, pattern, flags)
      AST::Match.from self, pattern, flags
    end

    def process_match2(line, pattern, value)
      AST::Match2.from self, pattern, value
    end

    def process_match3(line, pattern, value)
      AST::Match3.from self, pattern, value
    end

    def process_module(line, name, body)
      AST::ModuleWrapper.from self, name, body
    end

    def process_negate(line, expr)
      AST::Negate.from self, expr
    end

    def process_next(line, expr)
      AST::Next.from self, expr
    end

    def process_nil(line)
      AST::Nil.from self
    end

    def process_not(line, expr)
      AST::Not.from self, expr
    end

    def process_nth_ref(line, ref)
      AST::NthRef.from self, ref
    end

    def process_number(line, base, str)
      AST::NumberLiteral.from self, base, str
    end

    def process_op_asgn1(line, receiver, index, op, value)
      AST::OpAssign1.from self, receiver, index, op, value
    end

    def process_op_asgn2(line, receiver, name, op, value)
      AST::OpAssign2.from self, receiver, name, op, value
    end

    def process_op_asgn_and(line, var, value)
      AST::OpAssignAnd.from self, var, value
    end

    def process_op_asgn_or(line, var, value)
      AST::OpAssignOr.from self, var, value
    end

    def process_or(line, left, right)
      AST::Or.from self, left, right
    end

    def process_postexe(line)
      AST::Send.from self, AST::Self.from(self), :at_exit
    end

    def process_redo(line)
      AST::Redo.from self
    end

    def process_regex(line, str, flags)
      AST::RegexLiteral.from self, str, flags
    end

    def process_resbody(line, conditions, body, nxt)
      AST::RescueCondition.from self, conditions, body, nxt
    end

    def process_rescue(line, body, resc, els)
      AST::Rescue.from self, body, resc, els
    end

    def process_retry(line)
      AST::Retry.from self
    end

    def process_return(line, expr)
      AST::Return.from self, expr
    end

    def process_sclass(line, receiver, body)
      AST::SClass.from self, receiver, body
    end

    def process_scope(line, body)
      AST::Scope.from self, body
    end

    def process_self(line)
      AST::Self.from self
    end

    def process_splat(line, expr)
      AST::SplatValue.from self, expr
    end

    def process_str(line, str)
      AST::StringLiteral.from self, str
    end

    def process_super(line, args)
      AST::Super.from self, args
    end

    def process_svalue(line, expr)
      AST::SValue.from self, expr
    end

    def process_to_ary(line, expr)
      AST::ToArray.from self, expr
    end

    def process_true(line)
      AST::True.from self
    end

    def process_undef(line, sym)
      AST::Undef.from self, sym
    end

    def process_until(line, cond, body, check_first)
      AST::Until.from self, cond, body, check_first
    end

    def process_vcall(line, name)
      AST::Send.from self, AST::Self.from(self), name
    end

    def process_valias(line, to, from)
      AST::VAlias.from self, to, from
    end

    def process_when(line, conditions, body)
      AST::When.from self, conditions, body
    end

    def process_while(line, cond, body, check_first)
      AST::While.from self, cond, body, check_first
    end

    def process_xstr(line, str)
      AST::ExecuteString.from self, str
    end

    def process_yield(line, arguments, unwrap)
      AST::Yield.from self, arguments, unwrap
    end

    def process_zarray(line)
      AST::EmptyArray.from self
    end

    def process_zsuper(line)
      AST::ZSuper.from self
    end
  end
end


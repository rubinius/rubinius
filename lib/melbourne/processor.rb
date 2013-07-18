module Rubinius
  class Melbourne
    def process_parse_error(message, column, line, source)
      msg = "#{message}: #{@name}:#{line}:#{column}"
      @syntax_errors << SyntaxError.from(msg, column, line, source, @name)
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
      AST::Alias.new line, to, from
    end

    def process_and(line, left, right)
      AST::And.new line, left, right
    end

    def process_args(line, args, defaults, splat)
      AST::FormalArguments.new line, args, defaults, splat
    end

    def process_argscat(line, array, rest)
      AST::ConcatArgs.new line, array, rest
    end

    def process_argspush(line, arguments, value)
      AST::PushArgs.new line, arguments, value
    end

    def process_array(line, array)
      AST::ArrayLiteral.new line, array
    end

    def process_attrasgn(line, receiver, name, arguments)
      if name == :[]=
        AST::ElementAssignment.new line, receiver, arguments
      else
        AST::AttributeAssignment.new line, receiver, name, arguments
      end
    end

    def process_back_ref(line, ref)
      AST::BackRef.new line, ref
    end

    def process_begin(line, body)
      AST::Begin.new line, body
    end

    def process_block(line, array)
      AST::Block.new line, array
    end

    def process_block_arg(line, name)
      AST::BlockArgument.new line, name
    end

    def process_block_pass(line, method_send, body)
      node = AST::BlockPass.new line, body
      if method_send
        method_send.block = node
        method_send
      else
        node
      end
    end

    def process_break(line, value)
      AST::Break.new line, value
    end

    def process_call(line, receiver, name, arguments)
      if arguments.kind_of? AST::BlockPass
        block = arguments
        arguments = block.arguments
        block.arguments = nil
      else
        block = nil
      end

      if node = process_transforms(line, receiver, name, arguments)
        node.block = block if block
        return node
      end

      if arguments
        node = AST::SendWithArguments.new line, receiver, name, arguments
      else
        node = AST::Send.new line, receiver, name
      end

      node.block = block
      node
    end

    def process_case(line, receiver, whens, else_body)
      if receiver
        AST::ReceiverCase.new line, receiver, whens, else_body
      else
        AST::Case.new line, whens, else_body
      end
    end

    def process_cdecl(line, expr, value)
      AST::ConstantAssignment.new line, expr, value
    end

    def process_class(line, name, superclass, body)
      AST::Class.new line, name, superclass, body
    end

    def process_colon2(line, outer, name)
      if outer
        if outer.kind_of? AST::ConstantAccess and
           outer.name == :Rubinius
          case name
          when :Type
            AST::TypeConstant.new line
          when :Mirror
            AST::MirrorConstant.new line
          else
            AST::ScopedConstant.new line, outer, name
          end
        else
          AST::ScopedConstant.new line, outer, name
        end
      else
        AST::ConstantAccess.new line, name
      end
    end

    def process_colon3(line, name)
      AST::ToplevelConstant.new line, name
    end

    def process_const(line, name)
      AST::ConstantAccess.new line, name
    end

    def process_cvar(line, name)
      AST::ClassVariableAccess.new line, name
    end

    def process_cvasgn(line, name, value)
      AST::ClassVariableAssignment.new line, name, value
    end

    def process_cvdecl(line, name, value)
      AST::ClassVariableDeclaration.new line, name, value
    end

    def process_defined(line, expr)
      AST::Defined.new line, expr
    end

    def process_defn(line, name, body)
      AST::Define.new line, name, body
    end

    def process_defs(line, receiver, name, body)
      AST::DefineSingleton.new line, receiver, name, body
    end

    def process_dot2(line, start, finish)
      AST::Range.new line, start, finish
    end

    def process_dot3(line, start, finish)
      AST::RangeExclude.new line, start, finish
    end

    def process_dregx(line, str, array, flags)
      AST::DynamicRegex.new line, str, array, flags
    end

    def process_dregx_once(line, str, array, flags)
      AST::DynamicOnceRegex.new line, str, array, flags
    end

    def process_dstr(line, str, array)
      AST::DynamicString.new line, str, array
    end

    def process_dsym(line, str, array)
      AST::DynamicSymbol.new line, str, array
    end

    def process_dxstr(line, str, array)
      AST::DynamicExecuteString.new line, str, array
    end

    def process_ensure(line, body, ensr)
      AST::Ensure.new line, body, ensr
    end

    def process_evstr(line, value)
      if value
        AST::ToString.new line, value
      else
        AST::StringLiteral.new line, ""
      end
    end

    def process_false(line)
      AST::FalseLiteral.new line
    end

    def process_fcall(line, name, arguments)
      receiver = AST::Self.new line

      if arguments.kind_of? AST::BlockPass
        block = arguments
        arguments = block.arguments
        block.arguments = nil
      else
        block = nil
      end

      if node = process_transforms(line, receiver, name, arguments, true)
        node.block = block if block
        return node
      end

      if arguments
        node = AST::SendWithArguments.new line, receiver, name, arguments, true
      else
        node = AST::Send.new line, receiver, name, true
      end

      node.block = block
      node
    end

    def process_file(line)
      AST::File.new line
    end

    def process_fixnum(line, value)
      AST::FixnumLiteral.new line, value
    end

    def process_flip2(line, start, finish)
      AST::Flip2.new line, start, finish
    end

    def process_flip3(line, start, finish)
      AST::Flip3.new line, start, finish
    end

    def process_float(line, str)
      AST::FloatLiteral.new line, str
    end

    def process_for(line, iter, arguments, body)
      method_send = AST::Send.new line, iter, :each
      method_send.block = AST::For.new line, arguments, body
      method_send
    end

    def process_gasgn(line, name, expr)
      AST::GlobalVariableAssignment.new line, name, expr
    end

    def process_gvar(line, name)
      AST::GlobalVariableAccess.for_name line, name
    end

    def process_hash(line, array)
      AST::HashLiteral.new line, array
    end

    def process_iasgn(line, name, value)
      AST::InstanceVariableAssignment.new line, name, value
    end

    def process_if(line, cond, body, else_body)
      AST::If.new line, cond, body, else_body
    end

    def process_iter(line, method_send, arguments, body)
      method_send.block = AST::Iter.new line, arguments, body
      method_send
    end

    def process_ivar(line, name)
      AST::InstanceVariableAccess.new line, name
    end

    def process_lasgn(line, name, value)
      AST::LocalVariableAssignment.new line, name, value
    end

    def process_lit(line, sym)
      AST::SymbolLiteral.new line, sym
    end

    def process_lvar(line, name)
      AST::LocalVariableAccess.new line, name
    end

    def process_masgn(line, left, right, splat)
      AST::MultipleAssignment.new line, left, right, splat
    end

    def process_match(line, pattern, flags)
      AST::Match.new line, pattern, flags
    end

    def process_match2(line, pattern, value)
      AST::Match2.new line, pattern, value
    end

    def process_match3(line, pattern, value)
      AST::Match3.new line, pattern, value
    end

    def process_module(line, name, body)
      AST::Module.new line, name, body
    end

    def process_negate(line, value)
      AST::Negate.new line, value
    end

    def process_next(line, value)
      AST::Next.new line, value
    end

    def process_nil(line)
      AST::NilLiteral.new line
    end

    def process_not(line, value)
      AST::Not.new line, value
    end

    def process_nth_ref(line, ref)
      AST::NthRef.new line, ref
    end

    # TODO: Fix the way 1.8 parser handles this
    def process_number(line, base, str)
      value = str.to_i base
      case value
      when Fixnum
        AST::FixnumLiteral.new line, value
      when Bignum
        AST::NumberLiteral.new line, value
      end
    end

    def process_op_asgn1(line, receiver, index, op, value)
      AST::OpAssign1.new line, receiver, index, op, value
    end

    def process_op_asgn2(line, receiver, name, op, value)
      AST::OpAssign2.new line, receiver, name, op, value
    end

    def process_op_asgn_and(line, var, value)
      AST::OpAssignAnd.new line, var, value
    end

    def process_op_asgn_or(line, var, value)
      AST::OpAssignOr.new line, var, value
    end

    def process_or(line, left, right)
      AST::Or.new line, left, right
    end

    def process_postexe(line)
      AST::Send.new line, AST::Self.new(line), :at_exit, true
    end

    def process_preexe(line)
      node = AST::PreExe.new line
      add_pre_exe node
      node
    end

    def process_redo(line)
      AST::Redo.new line
    end

    def process_regex(line, str, flags)
      AST::RegexLiteral.new line, str, flags
    end

    def process_resbody(line, conditions, body, nxt)
      AST::RescueCondition.new line, conditions, body, nxt
    end

    def process_rescue(line, body, rescue_body, else_body)
      AST::Rescue.new line, body, rescue_body, else_body
    end

    def process_retry(line)
      AST::Retry.new line
    end

    def process_return(line, value)
      AST::Return.new line, value
    end

    def process_sclass(line, receiver, body)
      AST::SClass.new line, receiver, body
    end

    def process_scope(line, body)
      if body.kind_of? AST::Block
        body
      elsif body
        AST::Block.new line, [body]
      end
    end

    def process_self(line)
      AST::Self.new line
    end

    def process_splat(line, expr)
      AST::SplatValue.new line, expr
    end

    def process_str(line, str)
      AST::StringLiteral.new line, str
    end

    def process_super(line, arguments)
      AST::Super.new line, arguments
    end

    def process_svalue(line, expr)
      AST::SValue.new line, expr
    end

    def process_to_ary(line, expr)
      AST::ToArray.new line, expr
    end

    def process_true(line)
      AST::TrueLiteral.new line
    end

    def process_undef(line, sym)
      AST::Undef.new line, sym
    end

    def process_until(line, cond, body, check_first)
      AST::Until.new line, cond, body, check_first
    end

    def process_vcall(line, name)
      receiver = AST::Self.new line

      if node = process_transforms(line, receiver, name, nil, true)
        return node
      end

      AST::Send.new line, receiver, name, true, true
    end

    def process_valias(line, to, from)
      AST::VAlias.new line, to, from
    end

    def process_values(line, first, rest)
      rest.body.unshift first
      rest
    end

    def process_when(line, conditions, body)
      AST::When.new line, conditions, body
    end

    def process_while(line, cond, body, check_first)
      AST::While.new line, cond, body, check_first
    end

    def process_xstr(line, str)
      AST::ExecuteString.new line, str
    end

    def process_yield(line, arguments, unwrap)
      AST::Yield.new line, arguments, unwrap
    end

    def process_zarray(line)
      AST::EmptyArray.new line
    end

    def process_zsuper(line)
      AST::ZSuper.new line
    end
  end

  class Melbourne19 < Melbourne
    def process_args(line, required, optional, splat, post, block)
      AST::FormalArguments19.new line, required, optional, splat, post, block
    end

    def process_block_pass(line, arguments, body)
      AST::BlockPass19.new line, arguments, body
    end

    def process_encoding(line, name)
      AST::Encoding.new line, name
    end

    def process_postarg(line, into, rest)
      AST::PostArg.new line, into, rest
    end

    def process_iter(line, method_send, scope)
      ary = scope && scope.array || []
      arguments = nil

      if ary.first.kind_of? AST::FormalArguments
        arguments = scope.array.shift
      end

      unless arguments
        arguments = AST::FormalArguments19.new line, nil, nil, nil, nil, nil
      end

      case ary.size
      when 0
        body = nil
      when 1
        if scope.locals
          body = scope
        else
          body = scope.array.shift
        end
      else
        body = scope
      end

      method_send.block = AST::Iter19.new line, arguments, body
      method_send
    end

    def process_for(line, iter, arguments, body)
      send = AST::Send.new line, iter, :each
      send.block = AST::For19.new line, arguments, body
      send
    end

    def process_lambda(line, scope)
      arguments = scope.array.shift
      if scope.array.size == 1
        body = scope.array.shift
      else
        body = scope
      end

      receiver = AST::Self.new line
      method_send = AST::Send.new line, receiver, :lambda, true

      method_send.block = AST::Iter19.new line, arguments, body
      method_send
    end

    def process_number(line, value)
      case value
      when Fixnum
        AST::FixnumLiteral.new line, value
      when Bignum
        AST::NumberLiteral.new line, value
      end
    end

    def process_op_asgn_or(line, var, value)
      AST::OpAssignOr19.new line, var, value
    end

    def process_opt_arg(line, arguments)
      AST::Block.new line, arguments
    end

    def process_postexe(line, body)
      node = AST::Send.new line, AST::Self.new(line), :at_exit, true
      node.block = AST::Iter.new line, nil, body
      node
    end

    def process_preexe(line, body)
      node = AST::PreExe19.new line
      node.block = AST::Iter19.new line, nil, body
      add_pre_exe node
      node
    end

    def process_scope(line, arguments, body, locals)
      case body
      when AST::Begin
        if body.rescue.kind_of? AST::NilLiteral
          return nil unless arguments
        end
        body = AST::Block.new line, [body.rescue]
      when AST::Block
        ary = body.array
        if ary.size > 1 and
           ary.first.kind_of?(AST::Begin) and
           ary.first.rescue.kind_of?(AST::NilLiteral)
          ary.shift
        end
      when nil
        # Nothing
      else
        body = AST::Block.new line, [body]
      end

      if arguments and body
        body.array.unshift arguments
      end

      body.locals = locals if locals

      body
    end

    def process_super(line, arguments)
      if arguments.kind_of? AST::BlockPass
        block = arguments
        arguments = block.arguments
        block.arguments = nil
      else
        block = nil
      end

      node = AST::Super.new line, arguments
      node.block = block
      node
    end
  end
end

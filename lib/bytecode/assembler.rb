require 'bytecode/encoder'

module Bytecode
  class Assembler
    
    class Error < RuntimeError
    end
    
    class ExceptionLayoutError < Error
    end
    
    class UnknownException < Error
    end
    
    class Label
      def initialize(name)
        @name = name
        @location = nil
        @set = false
      end
      
      attr_accessor :location, :set
    end
    
    def initialize(literals=[], name=nil)
      @name = name
      reset(literals)
    end
    
    def reset(literals=[])
      @labels = Hash.new { |h,k| h[k] = Label.new(k) }
      @current_op = 0
      @output = []
      @locals = Hash.new
      @literals = literals
      @source_lines = []
      @exceptions = Hash.new
      @exception_depth = []
      @primitive = nil
      @arguments = []
      @cache_idx = 0
    end
    
    CacheSlotsPerEntry = 4
    CacheSlotsForConst = 1
        
    def next_cache_index(count)
      c = @cache_idx
      @cache_idx += count
      return c
    end
    
    def add_cache_index
      @current_op += 5
      @output << [:set_cache_index, next_cache_index(CacheSlotsPerEntry)]
    end
    
    def add_const_cache_index
      @current_op += 5
      @output << [:set_cache_index, next_cache_index(CacheSlotsForConst)]
    end
    
    attr_reader :labels, :source_lines, :primitive, :literals
    attr_accessor :locals, :exceptions, :arguments
    
    def sorted_exceptions
      initial = @exceptions.values.sort do |a,b|
        if a[0] == b[0]
          a[1] <=> b[1]
        else
          b[1] <=> a[1]
        end
      end
      
      split = []
      idx = 0
      initial.each do |ent|
        nxt = initial[idx + 1]
        if nxt
          # Detect if ent should be split.
          if nxt[0] > ent[0] and nxt[1] < ent[1]
            n1 = [ent[0], nxt[0] - 1, ent.last]
            n2 = [nxt[0], ent[1], ent.last]
            split << n1
            split << n2
          else
            split << ent
          end
        else
          split << ent
        end
        idx += 1
      end
      
      # Resort.
      output = split.sort do |a,b|
        if a[0] == b[0]
          a[1] <=> b[1]
        else
          a[1] <=> b[1]
        end
      end
      
      if output.first == [0,0,0]
        output.shift
      end
      
      return output
    end
    
    def assemble(str)
      puts str if ENV["DEBUG_ASSEMBLER"]
      lines = str.split("\n")
      lines.each { |l| parse_line l }
      translate_labels
      if ent = @source_lines.last
        ent[1] = @current_op
      end
      out = @output
      puts out.inspect if ENV["DEBUG_ASSEMBLER"]
      return out
    end

    def lines_as_tuple
      tuple_of_int_tuples @source_lines
    end
    
    def arguments_as_tuple
      tuple_of_syms @arguments
    end
    
    def literals_as_tuple
      tuple_of_syms @literals      
    end
    
    def bytecodes
      enc = Bytecode::InstructionEncoder.new
      str = @output.inject("") { |a,i| a + enc.encode(*i) }      
    end

    def translate_labels
      @output.map! do |i|
        next i unless Array === i   
        i.map { |j| Label === j ? j.location : j }
      end
    end
    
    def parse_command(kind, args)
      case kind
      when :line
        if ent = @source_lines.last
          # If we're already tracking this line, don't add anything.
          return if ent.last == args.to_i
          ent[1] = @current_op - 1
          # puts "#{ent[0]}-#{ent[1]}: line #{ent.last}"
        elsif @current_op > 0
          @source_lines << [0, @current_op, 0]
        end
        @source_lines << [@current_op, nil, args.to_i]
      when :exc_start
        name = args.strip.to_sym
        start = @labels[name]
        start.location = @current_op
        @exceptions[name] = [@current_op, nil, nil]
        @exception_depth << name
      when :exceptions
        ename = args.strip.to_sym
        unless @exception_depth.last == ename
          raise ExceptionLayoutError, "Expected #{@exception_depth.last}, got #{ename}"
        end
        name = (args.strip + "_rescue").to_sym
        cur = @labels[name]
        cur.location = @current_op
        
        if ent = @exceptions[ename]
          if ent[1]
            raise ExceptionLayoutError, "Already defined exceptions for #{ename}"
          else
            ent[1] = @current_op
          end
        else
          raise UnknownException, "Couldn't resolve #{ename}"
        end
                
        @exceptions[ename][2] = @current_op
        
      when :exc_end
        ename = args.strip.to_sym
        unless @exception_depth.last == ename
          raise ExceptionLayoutError, "Invalid exception layout"
        end
        
        unless ent = @exceptions[ename]
          raise UnknownException, "Couldn't resolve #{ename}"
        end
        
        name = (args.strip + "_end").to_sym
        fin = @labels[name]
        if fin.location
          raise ExceptionLayoutError, "Already defined end of #{ename}"
        end
        fin.location = @current_op
        @exception_depth.pop
      when :primitive
        @primitive = args.to_i
      when :arg
        @arguments << args.strip.to_sym
      else
        raise "Unknown command '#{kind}'"
      end
    end
    
    def parse_line(line)
      if m = /^\s*([^\s]*):(.*)/.match(line)
        name = m[1].to_sym
        if @labels.key?(name)
          if @labels[name].set
            raise "Re-defined label #{name}"
          end
        end
        lbl = @labels[name]
        lbl.location = @current_op
        lbl.set = true
        line = m[2]
      elsif m = /^s*#([^\s]+) (.*)/.match(line)
        kind = m[1].to_sym
        parse_command kind, m[2]
        return
      end
      
      line.strip!
      return if line.empty?
      
      parts = line.split(/\s+/)
      parts[0] = parts[0].to_sym
      # puts "#{@current_op}: #{parts.inspect}"
      parse_operation *parts
    end
    
    Translations = {
      :git => :goto_if_true,
      :gif => :goto_if_false,
      :gid => :goto_if_defined,
      :swap => :swap_stack,
      :dup => :dup_top,
      :find => :find_const,
      :attach => :attach_method,
      :add => :add_method
    }
    
    Simple = [:noop, :pop, :swap_stack, :dup_top]
    
    def number_of_locals
      @locals.size + 2
    end
    
    def find_local(name)
      cnt = @locals[name]
      return cnt if cnt
      
      cnt = @locals.size + 2
      @locals[name] = cnt
      return cnt
    end
    
    def find_literal(val)
      cnt = @literals.index(val)
      return cnt if cnt
      sz = @literals.size
      @literals << val
      return sz
    end
    
    def parse_lvar(what)
      if /^[a-z_][A-Za-z0-9_]*$/.match(what)
        cnt = find_local(what.to_sym)
        return cnt
      elsif m = /(^[a-z_][A-Za-z0-9_]*):(\d+)$/.match(what)
        name = m[1].to_sym
        cnt = m[2].to_i
        @locals[name] = cnt
        return cnt
      end
      return nil
    end
    
    def parse_const(what)
      if /^[A-Z][A-Za-z0-9_]*$/.match(what)
        return find_literal(what.to_sym)
      end
      return nil
    end
    
    def parse_ivar(what)
      if m = /(^[a-z_][A-Za-z0-9_]*)\.(.*)/.match(what)
        cnt = find_local(m[1].to_sym)
        lit = find_literal(("@" +m[2]).to_sym)
        return [cnt, lit]
      end
      return nil
    end
    
    def parse_aref(what)
      if m = /(.*)\[(\d+)\]/.match(what)
        parse_push(m[1])
        return m[2].to_i
      end
      return nil
    end
    
    def parse_push(what)
      if what[0] == ?&
        lbl = @labels[what[1..-1].to_sym]
        raise "Unknown label '#{lbl}'" unless lbl
        @output << [:push_int, lbl]
      elsif what[0] == ?@
        lit = find_literal(what.to_sym)
        @output << [:push_ivar, lit]
      elsif what.to_i.to_s == what
        num = what.to_i
        if num == -1
          @current_op += 1
          @output << :meta_push_neg_1
        elsif num == 0
          @current_op += 1
          @output << :meta_push_0
        elsif num == 1
          @current_op += 1
          @output << :meta_push_1
        elsif num == 2
          @current_op += 1
          @output << :meta_push_2
        else
          @current_op += 5
          @output << [:push_int, num]
        end
        return
      elsif idx = parse_aref(what)
        @output << [:push_int, idx]
        @output << :fetch_field
        @current_op += 1
      elsif what[0] == ?:
        lit = find_literal(what[1..-1].to_sym)
        @output << [:push_literal, lit]
      else
        case what
        when "true", true
          @output << :push_true
          @current_op += 1
          return
        when "false", false
          @output << :push_false
          @current_op += 1
          return
        when "nil", nil
          @output << :push_nil
          @current_op += 1
          return
        when "self", :self
          @output << :push_self
          @current_op += 1
        else
          if cnt = parse_lvar(what)
            @output << [:push_local, cnt]
            @current_op += 5
          elsif info = parse_ivar(what)
            @output << [:push_local, info.first]
            @output << [:push_ivar, info.last]
            @current_op += 10
          elsif cnt = parse_const(what)
            add_const_cache_index
            @output << [:push_const, cnt]
            @current_op += 5
          else
            raise "Unknown push argument '#{what}'"
          end
        end
        return
      end
      @current_op += 5
    end
    
    def parse_set(what)
      if cnt = parse_lvar(what)
        @output << [:set_local, cnt]
        @current_op += 5
      elsif info = parse_ivar(what)
        @output << [:push_local, info.first]
        @output << [:set_ivar, info.last]
        @current_op += 10
      elsif what[0] == ?@
        lit = find_literal(what.to_sym)
        @output << [:set_ivar, lit]
        @current_op += 5
      elsif idx = parse_aref(what)
        @output << [:push_int, idx]
        @output << :store_field
        @current_op += 6
      elsif cnt = parse_const(what)
        @output << [:set_const, cnt]
        @current_op += 5
      elsif what.index("::")
        parent, chld = what.split("::", 2)
        if cnt = parse_const(parent)
          add_const_cache_index
          @output << [:push_const, cnt]
        else
          raise "Invalid lhs to double colon (#{parent})"
        end
        
        if cnt = parse_const(chld)
          @output << [:set_const_at, cnt]
        else
          raise "Invalid rhs to double colon (#{chld})"
        end
        
        @current_op += 10
      elsif what.index("+") == 0
        if cnt = parse_const(what[1..-1])
          @output << [:set_const_at, cnt]
        else
          raise "Unknown + argument (#{what})"
        end
      else
        raise "Unknown set argument '#{what}'"
      end
    end
    
    def parse_operation(*parts)
      op = parts.shift
      if lop = Translations[op]
        op = lop
      end
      
      if Simple.include?(op)
        @output << op
        @current_op += 1
        return
      end
      
      if op == :set
        parse_set parts.shift
        return
      elsif [:goto, :goto_if_true, :goto_if_false].include?(op)
        label = parts.shift.to_sym
        @output << [op, @labels[label]]
        @current_op += 5
        return
      elsif op == :push
        parse_push parts.shift
        return
      elsif [:open_class, :find_const, :add_method, :attach_method, :send_method, :open_class_under, :open_module, :open_module_under].include?(op)
        sym = parts.shift.to_sym
        idx = find_literal(sym)
        add_const_cache_index if op == :find_const
        add_cache_index if op == :send_method
        @output << [op, idx]
        @current_op += 5
        return
      elsif op == :push
        parse_push parts.shift
        return
      elsif op == :send_stack
        sym = parts.shift.to_sym
        idx = find_literal(sym)
        add_cache_index
        @output << [op, idx, parts.shift.to_i]
        @current_op += 9
        return
      elsif [:"&send", :send].include?(op)
        sym = parts.shift.to_sym
        idx = find_literal(sym)
        add_cache_index
        @current_op += 5
        if args = parts.shift
          if args.to_i.to_s == args
            meth = (op == :send ? :send_stack : :send_stack_with_block)
          elsif args == "+"
            meth = :send_with_arg_register
            @output << [meth, idx]
            return            
          else
            raise "Unknown send argument type '#{args}'"
          end
          na = args.to_i
          @output << [meth, idx, na]
          @current_op += 4
        else
          @output << [:send_method, idx]
        end
        return
      elsif op == :super
        sym = parts.shift.to_sym
        idx = find_literal(sym)
        add_cache_index
        args = parts.shift
        if args == "+"
          @current_op += 5
          @output << [:send_super_with_arg_register, idx]          
        else  
          @current_op += 9
          @output << [:send_super_stack_with_block, idx, args.to_i]
        end
        return
      elsif op == :send_primitive
        sym = parts.shift.to_sym
        idx = primitive_to_index(sym)
        num_args = parts.shift.to_i
        @current_op += 9
        @output << [:send_primitive, idx, num_args]
        return
      elsif op == :check_argcount
        @output << [op, parts.shift.to_i, parts.shift.to_i]
        @current_op += 9
        return
      end
      
      if Bytecode::InstructionEncoder::OpCodes.include?(op)
        @current_op += 1
        if Bytecode::InstructionEncoder::IntArg.include?(op)
          @current_op += 4
          @output << [op, parts.first.to_i]
        else
          @output << op
        end
        return
      end
      
      raise "Unknown op '#{op}'"
    end
  end
end

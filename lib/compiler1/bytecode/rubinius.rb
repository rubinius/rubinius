require 'compiler1/bytecode/compiler'
require 'compiler1/bytecode/assembler'
require 'compiler1/bytecode/primitive_names'
require 'compiler1/bytecode/system_hints'

Compiler1::Bytecode::Compiler.load_system_hints(Rubinius::Bootstrap::HINTS)

module Compiler1
module Bytecode
  class MethodDescription
    
    def to_cmethod
      if $DEBUG_COMPILER
        puts "==================================="
        puts "Assembly of #{@name} in #{@file} (#{@path.inspect}):"
        puts @assembly
      end
      
      asm = Compiler1::Bytecode::Assembler.new(@literals, @name, @state)
      begin
        stream = asm.assemble @assembly
      rescue Object => e
        if $DEBUG
          puts "Failed to assemble #{@name}"
          puts @assembly
        end
        raise "Unable to assemble #{@name} in #{@file}:#{@last_line}. #{e.message}"
      end
      
      if $DEBUG_COMPILER
        puts "\nPre-encoded:"
        p stream
      end
      
      enc = Compiler1::Bytecode::InstructionEncoder.new
      bc = enc.encode_stream stream
      lcls = @state.number_of_locals
      iseq = bc.data.dup(InstructionSequence)
      cmeth = CompiledMethod.new.from_string iseq, lcls, @required
      cmeth.bonus = Tuple[@state.slot_locals_tuple, @state.stack_locals_tuple]
      cmeth.exceptions = asm.exceptions_as_tuple

      idx = nil

      if @primitive.kind_of? Symbol
        idx = Compiler1::Bytecode::Compiler::Primitives.index(@primitive)
        unless idx
          raise ArgumentError, "Unknown primitive '#{@primitive}'"
        end
        idx += 1
      elsif @primitive
        idx = @primitive
      end
      
      if idx
        cmeth.primitive = idx
      end
      
      cmeth.literals = encode_literals   # Also recursively processes contained methods
      if $DEBUG_COMPILER
        puts "\nLiterals:"
        p cmeth.literals
        puts "==================================="
      end
      
      if @file
        # Log.info "Method #{@name} is contained in #{@file}."
        cmeth.file = @file.to_sym
      else
        # Log.info "Method #{@name} is contained in an unknown place."
        cmeth.file = nil
      end
      
      if @name
        cmeth.name = @name.to_sym
      end
      
      cmeth.lines = asm.lines_as_tuple
      cmeth.path = encode_path()
      cmeth.cache = asm.cache_tuple
      cmeth.serial = 0
      return cmeth
    end

    def encode_path
      tup = Tuple.new(@path.size)
      i = 0
      @path.each do |pth|
        out = pth.to_sym
        tup.put i, out
        i += 1
      end
      
      return tup
    end
    
    def encode_literals
      tup = Tuple.new(@literals.size)
      i = 0
      lits = @literals
      # puts " => literals: #{lits.inspect}"
      lits.each do |lit|
        if lit.kind_of?(MethodDescription)
          lit = lit.to_cmethod
        end
        tup.put i, lit
        i += 1
      end
      
      return tup
    end
    
    def assembly
      @assembly
    end
    
    def output
      @output
    end
  end
  
  class Assembler
    
    def cache_tuple
      Tuple.new(@cache_idx)
    end
    
    def exceptions_as_tuple
      return nil if @exceptions.empty?
      excs = sorted_exceptions()
      tuple_of_int_tuples(excs)
    end
    
    def tuple_of_int_tuples(excs)
      exctup = Tuple.new(excs.size)
      i = 0
      excs.each do |ary|
        tup = Tuple.new(3)
        tup.put 0, ary[0]
        tup.put 1, ary[1]
        tup.put 2, ary[2]
        exctup.put i, tup
        i += 1
      end
      return exctup
    end
    
    def tuple_of_syms(ary)
      tup = Tuple.new(ary.size)
      i = 0
      ary.each do |t|
        sym = t.to_sym
        tup.put i, sym
        i += 1
      end
      return tup
    end
        
    def primitive_to_index(sym)
      Compiler1::Bytecode::Compiler::Primitives.index(sym) + 1 # add 1 for noop padding
    end
  end
end
end

require 'bytecode/assembler'
require 'object'
require 'cpu/runtime'

module Bytecode
  class MethodDescription
    
    def to_cmethod
      asm = Bytecode::Assembler.new(@literals, @name)
      begin
        stream = asm.assemble @assembly
      rescue Object => e
        raise "Unable to assemble #{@name} in #{@file}. #{e.message}"
      end
                  
      enc = Bytecode::InstructionEncoder.new
      bc = enc.encode_stream stream
      lcls = asm.number_of_locals
      cmeth = Rubinius::CompiledMethod.from_string bc, lcls
      cmeth.required = RObject.wrap(@required)
      cmeth.exceptions = asm.exceptions_as_tuple
      
      if @primitive.kind_of? Symbol
        idx = CPU::Primitives.name_to_index(@primitive)
        begin
          cmeth.primitive = RObject.wrap(idx)
        rescue Object
          raise ArgumentError, "Unknown primitive '#{@primitive}'"
        end
      elsif @primitive
        cmeth.primitive = RObject.wrap(@primitive)
      end

      cmeth.literals = encode_literals
      if @file
        # Log.info "Method #{@name} is contained in #{@file}."
        sym = Rubinius::String.new(@file).to_sym
        cmeth.file = sym
      else
        # Log.info "Method #{@name} is contained in an unknown place."
        cmeth.file = RObject.nil
      end
      
      if @name
        sym = Rubinius::String.new(@name.to_s).to_sym
        cmeth.name = sym
      end
      
      cmeth.lines = asm.lines_as_tuple
      cmeth.path = encode_path()
      return cmeth
    end
    
    def encode_path
      tup = Rubinius::Tuple.new(@path.size)
      i = 0
      @path.each do |pth|
        lit = Rubinius::String.new(pth.to_s)
        out = lit.to_sym
        tup.put i, out
        i += 1
      end
      
      return tup
    end
    
    def encode_literals
      tup = Rubinius::Tuple.new(@literals.size)
      i = 0
      lits = @literals
      # puts " => literals: #{lits.inspect}"
      lits.each do |lit|
        case lit
        when Symbol
          str = Rubinius::String.new(lit.to_s)
          out = str.to_sym
          # puts "Encoded #{lit.inspect} as #{out.inspect}"
        when Bytecode::MethodDescription
          out = lit.to_cmethod
        when String
          out = Rubinius::String.new(lit)
        when Bignum
          out = Rubinius::Bignum.new(lit.to_s)
        when Float
          out = Rubinius::Float.new(lit.to_s)
        else
          raise "Unable to encode literal: #{lit.inspect}"
        end
        tup.put i, out
        i += 1
      end
      
      return tup
    end
  end
  
  class Assembler
    def exceptions_as_tuple
      return RObject.nil if @exceptions.empty?
      excs = sorted_exceptions()
      tuple_of_int_tuples(excs)
    end
    
    def tuple_of_int_tuples(excs)
      exctup = Rubinius::Tuple.new(excs.size)
      i = 0
      excs.each do |ary|
        tup = Rubinius::Tuple.new(3)
        tup.put 0, RObject.wrap(ary[0])
        tup.put 1, RObject.wrap(ary[1])
        tup.put 2, RObject.wrap(ary[2])
        exctup.put i, tup
        i += 1
      end
      return exctup
    end
    
    def tuple_of_syms(ary)
      tup = Rubinius::Tuple.new(ary.size)
      i = 0
      ary.each do |t|
        sym = Rubinius::String.new(t.to_s).to_sym
        tup.put i, sym
        i += 1
      end
      return tup
    end
    
    def into_method
      cm = Rubinius::CompiledMethod.from_string(bytecodes, @locals.size)
      if @primitive
        cm.primitive = RObject.wrap(@primitive)
      end
      cm.literals = literals_as_tuple()
      cm.arguments = arguments_as_tuple()
      cm.exceptions = exceptions_as_tuple()
      cm.lines = lines_as_tuple()
      return cm
    end
    
    def primitive_to_index(sym)
      CPU::Primitives.name_to_index(sym)
    end
  end
end
  

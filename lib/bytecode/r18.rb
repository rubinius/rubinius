require 'bytecode/assembler'

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
        cmeth.primitive = RObject.wrap(idx)
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
      return cmeth
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
        else
          raise "Unable to encode literal: #{lit.inspect}"
        end
        tup.put i, out
        i += 1
      end
      
      return tup
    end
  end
end
  
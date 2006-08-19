# The Constructor is the highest level piece of taking strings
# of code and creating structures capable of being executed.

require 'sydparse'
require 'bytecode/compiler'
require 'bytecode/encoder'
require 'cpu/simple_marshal'

module Bytecode
  class Constructor
    
    def initialize(cpu)
      @cpu = cpu
      @enc = Bytecode::InstructionEncoder.new
      @sm = SimpleMarshal.new
    end
    
    def convert_to_sexp(code)
      syd = SydneyParser.load_string(code)
      return syd.sexp
    end
    
    def compile(code)
      sexp = convert_to_sexp(code)
      comp = Bytecode::Compiler.new
      desc = comp.compile_as_script sexp, :__script__
      return desc.to_cmethod
    end
    
    def compile_file(path)
      fd = File.open(path)
      code = fd.read
      meth = compile(code)
      fd.close
      return meth
    end
    
    def compiled_path(path)
      idx = path.index(".rb")
      if idx and idx == path.size - 3
        out = path[0, idx] + ".rbc"
        return out
      end
      
      return path + ".rbc"
    end
    
    def file_newer?(comp, orig)
      return false unless File.exists?(comp)
      return true  unless File.exists?(orig)
      
      cmt = File.mtime(comp)
      omt = File.mtime(orig)
            
      if cmt >= omt
        return true
      end
      
      return false
    end
    
    def load_file(path)
      cp = compiled_path(path)
      if file_newer?(cp, path)
        fd = File.open(cp)
        str = fd.read
        fd.close
        return @sm.unmarshal(str)
      end
      
      cm = compile_file(path)
      fd = File.open(cp, "w")
      fd << @sm.marshal(cm)
      fd.close
      return cm
    end
  end
end
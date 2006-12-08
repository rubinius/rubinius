module Compile
  def self.compile_file(path)
    require 'bytecode/compiler'
    sexp = File.to_sexp(path, true)
    comp = Bytecode::Compiler.new
    meth = comp.compile_as_method(sexp, :__script__)
    return meth
  end
end
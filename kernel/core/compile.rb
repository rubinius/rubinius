module Compile
  def self.compile_file(path)
    require 'bytecode/compiler'
    sexp = File.to_sexp(path, true)
    comp = Bytecode::Compiler.new
    desc = comp.compile_as_method(sexp, :__script__)
    require 'bytecode/rubinius'
    return desc.to_cmethod
  end
end

module Kernel
  def load(path)
    if path.suffix? ".rbc"
      cm = CompiledMethod.load_from_file(path)
      raise LoadError, "Unable to load file at path: #{path}" unless cm
      return cm.activate_as_script
    elsif path.suffix? ".rb"
      cm = Compile.compile_file(path)
      return cm.activate_as_script
    else
      raise LoadError, "Can't figure out how to load '#{path}'"
    end
  end
  
  def require(thing)
    kinds = [thing + ".rbc", thing + ".rb"]
    
    $:.each do |dir|
      kinds.each do |filename|
        path = "#{dir}/#{filename}"
        return false if $".include?(path)
        if File.exists?(path)
          $" << path
          return load(path)
        end
      end
    end
    raise LoadError, "Unable to find '#{thing}' to load"
  end
end

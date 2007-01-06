module Compile
  def self.compile_file(path)
    require 'bytecode/compiler'
    sexp = File.to_sexp(path, true)
    comp = Bytecode::Compiler.new
    desc = comp.compile_as_script(sexp, :__script__)
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
      comp = "#{path}c"
      if File.exists?(comp)
        cm = CompiledMethod.load_from_file(comp)
        raise LoadError, "Unable to load file at path: #{path}" unless cm
      else
        cm = Compile.compile_file(path)
        Marshal.dump_to_file cm, "#{path}c"
      end
      
      return cm.activate_as_script
      
    else
      raise LoadError, "Can't figure out how to load '#{path}'"
    end
  end
  
  def compile(path)
    cm = Compile.compile_file(path)
    Marshal.dump_to_file cm, "#{path}c"
  end
  
  def run_code(src)
    name = "/tmp/rubinius-tmpfile-#{src.hash}.rb"
    f = File.new(name, 'w')
    f.write src
    f.close
    load(name)
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

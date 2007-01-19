module Compile
  def self.compile_file(path)
    require 'bytecode/compiler'
    sexp = File.to_sexp(path, true)
    comp = Bytecode::Compiler.new
    desc = comp.compile_as_script(sexp, :__script__)
    require 'bytecode/rubinius'
    return desc.to_cmethod
  end
  
  def self.compile_string(string)
    require 'bytecode/compiler'
    sexp = string.to_sexp
    comp = Bytecode::Compiler.new
    desc = comp.compile_as_method(sexp, :__eval_script__)
    require 'bytecode/rubinius'
    return desc.to_cmethod
  end
  
  def self.execute(string)
    cm = compile_string(string)
    cm.activate MAIN, []
  end
end

class String
  def compile_as_method(filename = "(eval)", line = 1, newlines = true)
    sexp = self.to_sexp(filename, line, newlines)
    require 'bytecode/compiler'
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
      puts "[Loading #{path}]" if $DEBUG_LOADING
      return cm.activate_as_script
    elsif path.suffix? ".rb"
      comp = "#{path}c"
      if File.exists?(comp) && File.mtime(comp) >= File.mtime(path)
        puts "[Loading #{comp} for #{path}]" if $DEBUG_LOADING
        cm = CompiledMethod.load_from_file(comp)
        raise LoadError, "Unable to load file at path: #{path}" unless cm
      else
        puts "[Compiling and loading #{path}]" if $DEBUG_LOADING
        cm = Compile.compile_file(path)
        Marshal.dump_to_file cm, "#{path}c"
      end
      
      return cm.activate_as_script
      
    else
      raise LoadError, "Can't figure out how to load '#{path}'"
    end
  end
  
  def compile(path)
    out = "#{path}c"
    cm = Compile.compile_file(path)
    Marshal.dump_to_file cm, out
    return out
  end
    
  def require(thing)
    kinds = [thing + ".rbc", thing + ".rb"]
    
    $:.each do |dir|
      kinds.each do |filename|
        path = "#{dir}/#{filename}"
        return false if $".include?(path)
        if dir.suffix?(".rba") and File.exists?(dir)
          # puts "looking for #{filename} in #{dir}" #(#{Archive.list_files(dir).inspect})"
          cm = Archive.get_object(dir, filename)
          if cm
            # puts "Found #{filename} in #{dir}"
            $" << path
            return cm.activate_as_script
          end
        elsif File.exists?(path)
          $" << path
          return load(path)
        end
      end
    end
    raise LoadError, "Unable to find '#{thing}' to load"
  end
end

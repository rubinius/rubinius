module Rubinius
  # This const controls what the lowest version of compiled methods we can
  # allow is. This allows us to cut off compability at some point, or just
  # increment when major changes are made to the compiler.
  CompiledMethodVersion = 2
end

module Compile
  def self.compile_file(path)
    require 'bytecode/compiler'
    require 'bytecode/rubinius'
    sexp = File.to_sexp(path, true)
    comp = Bytecode::Compiler.new
    desc = comp.compile_as_script(sexp, :__script__)
    return desc.to_cmethod
  end
  
  def self.compile_string(string)
    require 'bytecode/compiler'
    require 'bytecode/rubinius'
    sexp = string.to_sexp
    comp = Bytecode::Compiler.new
    desc = comp.compile_as_method(sexp, :__eval_script__)
    return desc.to_cmethod
  end
  
  def self.execute(string)
    cm = compile_string(string)
    cm.activate MAIN, []
  end
end

class String
  def compile_as_method(filename = "(eval)", line = 1, newlines = true)
    require 'bytecode/compiler'
    require 'bytecode/rubinius'
    sexp = self.to_sexp(filename, line, newlines)
    comp = Bytecode::Compiler.new
    desc = comp.compile_as_method(sexp, :__script__)
    return desc.to_cmethod
  end
end

module Functions
  def load(path)
    if path.suffix? ".rbc"
      cm = CompiledMethod.load_from_file(path, Rubinius::CompiledMethodVersion)
      raise LoadError, "Unable to load file at path: #{path}" unless cm
      puts "[Loading #{path}]" if $DEBUG_LOADING
      return cm.activate_as_script
    end

    if path.suffix? ".rb"
      comp = "#{path}c"
    else
      comp = "#{path}.rbc"
    end

    # do we have a compiled file that's not older than the source?
    if File.exists?(comp) && File.mtime(comp) >= File.mtime(path)
      puts "[Loading #{comp} for #{path}]" if $DEBUG_LOADING
      cm = CompiledMethod.load_from_file(comp, Rubinius::CompiledMethodVersion)

      unless cm
        puts "[Skipping #{comp}, was invalid.]" if $DEBUG_LOADING
      else
        return cm.activate_as_script
      end
    end

    # compile the source
    puts "[Compiling and loading #{path}]" if $DEBUG_LOADING
    cm = Compile.compile_file(path)

    raise LoadError, "Unable to compile file at path: #{comp}" unless cm

    # and store it
    Marshal.dump_to_file cm, comp, Rubinius::CompiledMethodVersion

    return cm.activate_as_script
  end
  
  def compile(path, out=nil)
    out = "#{path}c" unless out
    cm = Compile.compile_file(path)
    Marshal.dump_to_file cm, out, Rubinius::CompiledMethodVersion
    return out
  end
    
  def require(thing)
    # puts "Requiring '#{thing}'"
    # TODO: #{Config::CONFIG["DLEXT"]}
    kinds = [thing + ".rb", thing + ".rbc", thing]
    # Non extension thing added for C extension check
    
    $:.each do |dir|
      kinds.each do |filename|
        path = "#{dir}/#{filename}"
        return false if $".include?(path)
        if dir.suffix?(".rba") and File.exists?(dir)
          # puts "looking for #{filename} in #{dir}" #(#{Archive.list_files(dir).inspect})"
          cm = Archive.get_object(dir, filename, Rubinius::CompiledMethodVersion)
          if cm
            # puts "Found #{filename} in #{dir}"
            $" << path
            return cm.activate_as_script
          end
          
        # Don't accidentally load non-extension files
        elsif File.exists?(path) && (path.suffix?(".rb") or path.suffix?(".rbc"))
          $" << path
          return load(path)
        elsif (load_result = VM.load_library(path, File.basename(path)))
          case load_result
          when true
            $" << path
            return true
          when 1
            raise LoadError, "Invalid extension at #{thing}. Did you define Init_#{File.basename(path)}?"
          end
        end
      end
    end
    raise LoadError, "Unable to find '#{thing}' to load"
  end
end

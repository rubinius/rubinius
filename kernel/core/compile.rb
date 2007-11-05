module Rubinius
  # This const controls what the lowest version of compiled methods we can
  # allow is. This allows us to cut off compability at some point, or just
  # increment when major changes are made to the compiler.
  CompiledMethodVersion = 5
end

module Compile
  def self.compile_file(path, flags=nil)
    require 'bytecode/compiler'
    require 'bytecode/rubinius'
    sexp = File.to_sexp(path, true)
    comp = Bytecode::Compiler.new
    comp.import_flags(flags) if flags
    desc = comp.compile_as_script(sexp, :__script__)
    return desc.to_cmethod
  end
  
  def self.compile_string(string, flags=nil)
    require 'bytecode/compiler'
    require 'bytecode/rubinius'
    sexp = string.to_sexp
    comp = Bytecode::Compiler.new
    comp.import_flags(flags) if flags
    state = RsLocalState.new
    state.uses_eval = true
    desc = comp.compile_as_method(sexp, :__eval_script__, state)
    return desc.to_cmethod
  end
  
  def self.execute(string)
    cm = compile_string(string)
    cm.compile
    cm.activate MAIN, []
  end

  # Called when we encounter a break keyword that we do not support
  # TODO - This leaves a moderately lame stack trace entry
  def self.__unexpected_break__
    raise LocalJumpError, "unexpected break"
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

module Kernel
  def load(path)
    if path.suffix? ".rbc"
      raise LoadError, "Unable to directly load compiled file -- #{File.basename(path)}"
    elsif path.suffix? ".rb"
        compiled = "#{path}c"
    else
        # compute a compiled version name, even if the path is not to a .rb file
        compiled = "#{path}.rbc"
    end

    if (!File.exists?(path) and !File.exists?(compiled))
      raise LoadError, "No such file to load -- #{path}"
    end

    # Use compiled version if the original is missing, or compiled version newer
    if !File.exists?(path) or (File.exists?(compiled) and File.mtime(path) <= File.mtime(compiled) )
      puts "[Loading #{compiled} for #{path}]" if $DEBUG_LOADING
      cm = CompiledMethod.load_from_file(compiled, Rubinius::CompiledMethodVersion)

      unless cm
        puts "[Skipping #{compiled}, was invalid.]" if $DEBUG_LOADING
      else
        return cm.activate_as_script
      end
    end

    # compile the source
    puts "[Compiling and loading #{path}]" if $DEBUG_LOADING
    cm = Compile.compile_file(path)

    raise LoadError, "Unable to compile file at path: #{path}" unless cm

    # and store it
    Marshal.dump_to_file cm, compiled, Rubinius::CompiledMethodVersion
    
    # since we just created it, 'compile it', ie, let the VM finish
    # preparing it to be run
    cm.compile
    
    return cm.activate_as_script
  end
  
  def compile(path, out=nil, flags=nil)
    out = "#{path}c" unless out
    cm = Compile.compile_file(path, flags)
    Marshal.dump_to_file cm, out, Rubinius::CompiledMethodVersion
    return out
  end
  
  # look in each directory of $: for .rb, .rbc, or .<library extension>
  def require(thing)
    if thing.suffix? '.rbc'
      raise LoadError, "unable to directly require compiled file #{thing}"
    elsif thing.suffix? '.rb'
      base_file = thing.chomp('.rb')
      rb_file   = thing
      rbc_file  = thing + 'c'
    else
      base_file = thing
      rb_file   = thing + '.rb'
      rbc_file  = thing + '.rbc'
    end
    
    $:.each do |dir|
      rb_path   = "#{dir}/#{rb_file}"
      rbc_path  = "#{dir}/#{rbc_file}"
      base_path = "#{dir}/#{base_file}"

      [rb_path, rbc_path, base_path].each do |path|
        return false if $".include?(path)
      end
      
      if dir.suffix?('.rba') and File.exists?(dir)
        cm = Archive.get_object(dir, rbc_file, Rubinius::CompiledMethodVersion)
        if cm
          $" << rbc_path          
          return cm.activate_as_script
        end
      elsif File.exists?(rb_path) or File.exists?(rbc_path)
        # Don't accidentally load non-extension files
        $" << rb_path
        return load(rb_path)
      else
        load_result = VM.load_library(base_path, File.basename(base_path))
        case load_result
        when true
          $" << base_path
          return true
        when 1
          raise LoadError, "Invalid extension at #{thing}. Did you define Init_#{File.basename(path)}?"
        end
      end
    end
    raise LoadError, "Unable to find '#{thing}' to load"
  end
end

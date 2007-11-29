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
    cm.activate MAIN, Object, []
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
        return cm.as_script
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
    
    return cm.as_script
  end
  
  def compile(path, out=nil, flags=nil)
    out = "#{path}c" unless out
    cm = Compile.compile_file(path, flags)
    Marshal.dump_to_file cm, out, Rubinius::CompiledMethodVersion
    return out
  end

  # look in each directory of $LOAD_PATH for .rb, .rbc, or .<library extension>
  def require(thing)
    if thing.suffix? '.rbc'
      raise LoadError, "unable to directly require compiled file #{thing}"
    end

    if thing.suffix? '.rb'
      base_file = thing.chomp('.rb')
      rb_file   = thing
      rbc_file  = thing + 'c'
    else
      base_file = thing
      rb_file   = thing + '.rb'
      rbc_file  = thing + '.rbc'
    end

    paths = $LOAD_PATH.map do |dir|
      rb_path   = File.join dir, rb_file
      rbc_path  = File.join dir, rbc_file
      base_path = File.join dir, base_file

      [dir, rb_path, rbc_path, rbc_file]
    end

    # HACK windows
    paths.unshift ['', rb_file, rbc_file, base_file] if thing.prefix? '/'

    paths.each do |dir, rb_path, rbc_path, base_path|

      if dir.suffix? '.rba' and File.file? dir then
        return false if $LOADED_FEATURES.include? rb_path

        cm = Archive.get_object(dir, rbc_path, Rubinius::CompiledMethodVersion)
        $LOADED_FEATURES << rb_path
        return cm.as_script
      elsif File.file? rb_path or File.file? rbc_path then
        return false if $LOADED_FEATURES.include? rb_path

        # Don't accidentally load non-extension files
        $LOADED_FEATURES << rb_path
        load rb_path
        return true
      else
        return false if $LOADED_FEATURES.include? base_path

        load_result = VM.load_library(base_path, File.basename(base_path))
        case load_result
        when true
          $LOADED_FEATURES << base_path
          return true
        when 1
          raise LoadError, "Invalid extension at #{thing}. Did you define Init_#{File.basename(base_path)}?"
        end
      end
    end

    raise LoadError, "no such file to load -- #{thing}"
  end

end


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
  
  def self.require_feature(dir, rb_file, rbc_file, base_file)
    if dir.suffix? '.rba' and File.file? dir then
      return false if $LOADED_FEATURES.include? rbc_file

      cm = Archive.get_object(dir, rbc_file, Rubinius::CompiledMethodVersion)
      return nil unless cm
      
      $LOADED_FEATURES << rbc_file
      cm.as_script
      return true
    else
      
      rb_path   = "#{dir}/#{rb_file}"
      rbc_path  = "#{dir}/#{rbc_file}"
      
      # Order is important here. We have to check for the rb_path first because
      # it's possible both exist, and we want to give preference to rb_path.
      
      if File.file? rb_path
        return false if $LOADED_FEATURES.include? rb_file
        
        $LOADED_FEATURES << rb_file
        load rb_path
        return true
      elsif File.file? rbc_path
        return false if $LOADED_FEATURES.include? rbc_file
        
        $LOADED_FEATURES << rbc_file
        load rbc_path
        return true
      else
        ext_file = "#{base_file}.#{Rubinius::LIBSUFFIX}"
        ext_path = "#{dir}/#{ext_file}"
        
        if File.file? ext_path
          return false if $LOADED_FEATURES.include? ext_file

          case VM.load_library(ext_path, File.basename(base_file))
          when true
            $LOADED_FEATURES << ext_file
            return true
          when 0 # absent or invalid
            return nil
          when 1 # valid library, but no entry point
            raise LoadError, "Invalid extension at '#{ext_path}'. Did you define Init_#{File.basename(base_file)}?"
          end
        end
      end
    end
    
    return nil
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
    path = StringValue(path)

    if path.suffix? ".rbc"
      compiled = path
    elsif path.suffix? ".rb"
      compiled = "#{path}c"
    else
      # compute a compiled version name, even if the path is not to a .rb file
      compiled = "#{path}.rbc"
    end

    # If neither the original nor the compiled version are there, bail.
    if !File.exists?(path) and !File.exists?(compiled)
      raise LoadError, "No such file to load -- #{path}"
    end

    # Use compiled version if the original is missing, or compiled version newer
    if !File.exists?(path) or 
          (File.exists?(compiled) and File.mtime(path) <= File.mtime(compiled))
      puts "[Loading #{compiled} for #{path}]" if $DEBUG_LOADING
      cm = CompiledMethod.load_from_file(compiled, Rubinius::CompiledMethodVersion)

      if cm
        return cm.as_script
      else
        puts "[Skipping #{compiled}, was invalid.]" if $DEBUG_LOADING
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
    raise LoadError, "Unable to compile '#{path}'" unless cm
    Marshal.dump_to_file cm, out, Rubinius::CompiledMethodVersion
    return out
  end

  # look in each directory of $LOAD_PATH for .rb, .rbc, or .<library extension>
  def require(thing)    
    thing = StringValue(thing)

    if thing.suffix? '.rbc'
      base_file = thing.chomp('.rbc')
      rb_file   = base_file + '.rb'
      rbc_file  = thing
    elsif thing.suffix? '.rb'
      base_file = thing.chomp('.rb')
      rb_file   = thing
      rbc_file  = thing + 'c'
    elsif thing.suffix? Rubinius::LIBSUFFIX
      base_file = thing.chomp(Rubinius::LIBSUFFIX)
      rb_file   = thing
      rbc_file  = thing
    else
      base_file = thing
      rb_file   = thing + '.rb'
      rbc_file  = thing + '.rbc'
    end
    
    # HACK this wont work on windows
    if thing.prefix? '/'
      res = Compile.require_feature '', rb_file, rbc_file, base_file
      return res unless res.nil?
    end
    
    $LOAD_PATH.each do |dir|      
      res = Compile.require_feature dir, rb_file, rbc_file, base_file
      return res unless res.nil?
    end

    raise LoadError, "no such file to load -- #{thing}"
  end

end


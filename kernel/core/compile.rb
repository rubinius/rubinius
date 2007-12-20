module Rubinius
  # This const controls what the lowest version of compiled methods we can
  # allow is. This allows us to cut off compability at some point, or just
  # increment when major changes are made to the compiler.
  CompiledMethodVersion = 6
end

module Compile
  
  @compiler = nil
  
  DefaultCompiler = "compiler1"
  
  def self.register_compiler(obj)
    if $DEBUG
      $stderr.puts "[Registered #{obj} as system compiler]"
    end
    @compiler = obj
  end
    
  def self.find_compiler
    begin
      load "#{DefaultCompiler}/init"
    rescue Exception
      raise "Unable to load default compiler"
    end
    
    unless @compiler
      raise "Attempted to load DefaultCompiler, but no compiler was registered"
    end
    
    return @compiler
  end
  
  def self.compiler
    return @compiler if @compiler
    return find_compiler
  end
  
  def self.compile_file(path, flags=nil)
    compiler.compile_file(path, flags)
  end
  
  def self.compile_string(string, flags=nil, filename="(eval)", line=1)
    compiler.compile_string(string, flags, filename, line)
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
  
  # Internal. See #require for interface.
  def self.require_feature(dir, rb_file, rbc_file, ext_file)
    rb_path   = "#{dir}/#{rb_file}"
    rbc_path  = "#{dir}/#{rbc_file}"
    ext_path  = "#{dir}/#{ext_file}"

    # Be mindful of the order and the difference between _file and _path.

    if File.file? rb_path
      return false if $LOADED_FEATURES.include? rb_file

      # Use source only if it is newer
      if !File.file?(rbc_path) or File.mtime(rb_path) > File.mtime(rbc_path)
        cm = Compile.compile_file(rb_path)
        raise LoadError, "Unable to compile: #{rb_path}" unless cm

        # Store it
        Marshal.dump_to_file cm, rbc_path, Rubinius::CompiledMethodVersion

      else 
        cm = CompiledMethod.load_from_file(rbc_path, Rubinius::CompiledMethodVersion)
        raise LoadError, "Invalid .rbc: #{rbc_path}" unless cm
      end

      cm.compile
      cm.as_script
      $LOADED_FEATURES << rb_file
      return true

    elsif File.file? rbc_path
      return false if $LOADED_FEATURES.include? rb_file

      cm = CompiledMethod.load_from_file(rbc_path, Rubinius::CompiledMethodVersion)
      raise LoadError, "Invalid .rbc: #{rbc_path}" unless cm

      cm.compile
      cm.as_script
      $LOADED_FEATURES << rb_file
      return true

    elsif File.file? ext_file
      return false if $LOADED_FEATURES.include? ext_file

      if File.file? ext_path
        ext_name = File.basename(ext_file).chomp(Rubinius::LIBSUFFIX))

        case VM.load_library(ext_path, ext_name)
          when true
            $LOADED_FEATURES << ext_file
            return true
          when 0 # Absent or invalid
            return nil
          when 1 # Valid library, but no entry point
            raise LoadError, "Invalid extension at '#{ext_path}'. " << 
                             "Did you define Init_#{ext_name}?"
        end
      end
    end

    return nil
  end

  # Internal. See #load for interface.
  def self.single_load(dir, rb, rbc, ext)

    # .rb and no extension
    unless rb.equal? nil
      rb_path = "#{dir}/#{rb}"

      if File.file? rb_path
        rbc_path = "#{dir}/#{rbc}"

        # Use source only if it is newer
        if !File.file?(rbc_path) or File.mtime(rb_path) > File.mtime(rbc_path)
          cm = Compile.compile_file(rb_path)
          raise LoadError, "Unable to compile: #{rb_path}" unless cm

          # Store it for the future
          Marshal.dump_to_file cm, rbc_path, Rubinius::CompiledMethodVersion

        else
          cm = CompiledMethod.load_from_file(rbc_path, Rubinius::CompiledMethodVersion)
          raise LoadError, "Invalid .rbc: #{rbc_path}" unless cm
        end

        cm.compile
        cm.as_script
        return true
      end
    end

    # .rbc
    unless rbc.equal? nil
      rbc_path = "#{dir}/#{rbc}"

      return nil unless File.file? rbc_path

      cm = CompiledMethod.load_from_file(rbc_path, Rubinius::CompiledMethodVersion)
      raise LoadError, "Invalid .rbc: #{rbc_path}" unless cm

      cm.compile
      cm.as_script
      return true
    end

    # .<ext>
    unless ext.equal? nil
      ext_path = "#{dir}/#{ext}"
      ext_name = File.basename(ext).chomp Rubinius::LIBSUFFIX

      return nil unless File.file? ext_path

      case VM.load_library(ext_path, ext_name)
        when true
          return true
        when 0 # Absent or invalid
          return nil
        when 1 # Valid library, but no entry point
          raise LoadError, "Invalid extension at '#{ext_path}'. " << 
                           "Did you define Init_#{ext_name}?"
      end
    end

    nil
  end   

end       # Compile

module Kernel
  def compile(path, out=nil, flags=nil)
    out = "#{path}c" unless out
    cm = Compile.compile_file(path, flags)
    raise LoadError, "Unable to compile '#{path}'" unless cm
    Marshal.dump_to_file cm, out, Rubinius::CompiledMethodVersion
    return out
  end
  
  # Loads the given file as executable code and returns true. If
  # the file cannot be found, cannot be compiled or some other 
  # error occurs, LoadError is raised with an explanation.
  #
  # Unlike #require, the file extension if any must be present but
  # is not restricted to .rb, .rbc or .<platform shared lib ext>.
  # Any other extensions (or no extension) are assumed to be plain
  # Ruby files. The only exceptions to this rule are:
  #
  #   1.  if given a .rb or no/any-extensioned file and there is a
  #       compiled version of the same file that is not older than
  #       the source file (based on File.mtime), the compiled one
  #       is loaded directly to avoid the compilation overhead.
  #
  #   2.  if a .rb file is given but it does not exist, the system
  #       will try to load the corresponding .rbc instead (to allow
  #       distributing just .rbc files.)
  #
  # If the path given starts with ./, ../, ~/ or /, it is treated
  # as a "qualified" file and will be loaded directly (after path
  # expansion) instead of matching against $LOAD_PATH. The relative
  # paths use Dir.pwd.
  #
  # If the filename is plain (unqualified) then it is sequentially
  # prefixed with each path in $LOAD_PATH ($:) to locate the file,
  # using the first one that exists. If none of the resulting paths
  # exist, LoadError is raised. Unqualified names may contain path
  # elements so directories are valid targets and can be used with
  # $LOAD_PATH.
  #
  # TODO: Support non-UNIX paths.
  #
  # TODO: The anonymous module wrapping is not implemented at all.
  #
  # TODO: load 'somefile' should only succeed if that exact filename
  #       exists. However, due to the requirement that a .rbc should
  #       be usable even if .rb does not exist, load 'somefile' will
  #       attempt load 'somefile.rbc' also. This is problematic because
  #       if 'somefile.rb' does exist but 'somefile' does not, the
  #       load should fail. Currently it does not if 'somefile.rb'
  #       has been compiled to 'somefile.rbc'.
  #       
  def load(path)
    path = StringValue(path)
    
    if path.suffix? '.rbc'
      rb, rbc, ext = nil, path, nil
    elsif path.suffix? '.rb'
      rb, rbc, ext = path, "#{path}c", nil
    elsif path.suffix? ".#{Rubinius::LIBSUFFIX}"
      rb, rbc, ext = nil, nil, path
    else
      rb, rbc, ext = path, "#{path}.rbc", nil
    end

    # Qualified files
    if path =~ %r{\A(?:(\.\.?)|(~))?/}
      if $1       # Relative
        res = Compile.single_load Dir.pwd, rb, rbc, ext

      elsif $2    # ~ 
        rb.slice!(0..1) if rb
        rbc.slice!(0..1) if rbc
        ext.slice!(0..1) if ext
        res = Compile.single_load ENV['HOME'], rb, rbc, ext

      else        # Absolute
        res = Compile.single_load '', rb, rbc, ext
      end

      return true unless res.equal? nil
    
    # Unqualified
    else
      $LOAD_PATH.each do |dir|      
        if dir.suffix? '.rba' and File.file? dir 
          # TODO: This may need to change for efficiency
          cm = Archive.get_object_fuzzy(dir, path, Rubinius::CompiledMethodVersion)
          
          if cm
            cm.compile
            cm.as_script
            return true
          end
        end

        res = Compile.single_load dir, rb, rbc, ext
        return true unless res.equal? nil
      end
    end

    raise LoadError, "No such file: #{path}"
  end

  # Attempt to load the given file, returning true if successful.
  # If the file has already been successfully loaded and exists
  # in $LOADED_FEATURES, it will not be re-evaluated and false 
  # is returned instead. If the filename cannot be resolved, 
  # a LoadError is raised.
  #
  # The file can have one of the following extensions:
  #
  #   .rb                 Plain Ruby source file.
  #   .rbc                Compiled Ruby source file.
  #   .o|.so|.dylib|.dll  Shared library (platform-specific.)
  #   <none>              Filename without extension.
  #
  # (.rba files should be loaded using CodeArchive.load_everything.)
  #
  # If the file does not have an extension, #require attempts to
  # match it using .rb, .rbc and .<shared extension> as extensions,
  # in that order, instead. If foo.rb does not exist but foo.rbc
  # does, the latter will be loaded even if called with foo.rb.
  #
  # If the path given starts with ./, ../, ~/ or /, it is treated
  # as a "qualified" file and will be loaded directly (after path
  # expansion) instead of matching against $LOAD_PATH. The relative
  # paths use Dir.pwd.
  #
  # If the filename is plain (unqualified) then it is sequentially
  # prefixed with each path in $LOAD_PATH ($:) to locate the file,
  # using the first one that exists. If none of the resulting paths
  # exist, LoadError is raised. Unqualified names may contain path
  # elements so directories are valid targets and can be used with
  # $LOAD_PATH.
  #
  # TODO: Support non-UNIX paths.
  #
  # TODO: See if we can safely use 1.9 rules with $LOADED_FEATURES,
  #       i.e. expand paths into it. This should be possible if it
  #       is completely transparent to the user in all normal cases.
  #
  # Each successfully loaded file is added to $LOADED_FEATURES
  # ($"), using the original unexpanded filename (with the 
  # exception that the file extension is added.) 
  #
  def require(path)    
    path = StringValue(path)

    if path.suffix? '.rbc'
      rb_file   = path.chomp('c')
      rbc_file  = path
      ext_file  = ''
    elsif path.suffix? '.rb'
      rb_file   = path
      rbc_file  = "#{path}c"
      ext_file  = ''
    elsif path.suffix? Rubinius::LIBSUFFIX
      rb_file   = ''
      rbc_file  = ''
      ext_file  = path
    else
      rb_file  = "#{path}.rb"
      rbc_file  = "#{path}.rbc"
      ext_file  = path + '.' + Rubinius::LIBSUFFIX
    end
    
    # Qualified files
    if path =~ %r{^(?:(\.\.?)|(~))?/}
      if $1
        res = Compile.require_feature Dir.pwd, rb_file, rbc_file, ext_file
      elsif $2
        rb_file, rbc_file, ext_file = rb_file[2..-1], rbc_file[2..-1], ext_file[2..-1]
        res = Compile.require_feature ENV['HOME'], rb_file, rbc_file, ext_file
      else
        res = Compile.require_feature '', rb_file, rbc_file, ext_file
      end

      return true unless res.equal? nil
    
    # Unqualified
    else
      $LOAD_PATH.each do |dir|      
        if dir.suffix? '.rba' and File.file? dir 
          return false if $LOADED_FEATURES.include? rb_file

          cm = Archive.get_object(dir, rbc_file, Rubinius::CompiledMethodVersion)
          
          if cm
            cm.compile
            cm.as_script
            $LOADED_FEATURES << rb_file
            return true
          end
        end

        res = Compile.require_feature dir, rb_file, rbc_file, ext_file
        return res unless res.equal? nil
      end
    end

    raise LoadError, "No such file: #{path}"
  end

end


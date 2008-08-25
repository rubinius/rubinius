# depends on: module.rb

module Rubinius

  ##
  # This const controls what the lowest version of compiled methods we can
  # allow is. This allows us to cut off compability at some point, or just
  # increment when major changes are made to the compiler.

  CompiledMethodVersion = 6

end

##
# Main entrace point to the Rubinius compiler.  Handles loading and compiling
# ruby code.

module Compile

  @compiler = nil

  DefaultCompiler = "compiler"

  def self.register_compiler(obj)
    if $DEBUG
      $stderr.puts "[Registered #{obj} as system compiler]"
    end
    @compiler = obj
  end

  def self.find_compiler
    @compiler = :loading
    begin
      loading_rbc_directly do
        require "#{DefaultCompiler}/init"
      end
    rescue Exception => e
      STDERR.puts "Unable to load default compiler: #{e.message}"
      puts e.awesome_backtrace.show
      raise e
    end

    if @compiler == :loading
      raise "Attempted to load DefaultCompiler, but no compiler was registered"
    end

    return @compiler
  end

  def self.compiler
    return @compiler if @compiler and @compiler != :loading
    return find_compiler
  end

  def self.version_number
    # Until the compiler is loaded, load any versions. This
    # lets us bootstrap the compiler into the system no matter
    # what version it is. This is important because we don't want
    # to keep the compiler from loading at all because it might have
    # older versioned files.
    return @compiler.version_number if @compiler and @compiler != :loading
    return 0
  end

  def self.compile_file(path, flags=nil)
    compiler.compile_file(path, flags)
  end

  def self.compile_string(string, context=nil, filename="(eval)", line=1)
    compiler.compile_string(string, context, filename, line)
  end

  def self.execute(string)
    eval(string, TOPLEVEL_BINDING)
  end

  # Sets a flag so that the next script to be loaded gets a breakpoint set at
  # the first instruction
  def self.debug_script!
    @debug_script = true
  end

  # By calling require in the block passed to this, require will
  # load rbc if they exist without checking mtime's and such.
  @load_rbc_directly = false

  def self.loading_rbc_directly
    begin
      @load_rbc_directly = true
      yield
    ensure
      @load_rbc_directly = false
    end
  end

  # Called when we encounter a break keyword that we do not support
  # TODO - This leaves a moderately lame stack trace entry
  def self.__unexpected_break__
    raise LocalJumpError, "unexpected break"
  end

  # Internally used by #load and #require. Determines whether to
  # load the file directly or by prefixing it with the paths in
  # $LOAD_PATH and then attempts to locate and load the file.
  def self.unified_load(path, rb, rbc, ext, requiring = nil, options = {:recompile => false})
    # forces the compiler to be loaded. We need this to get
    # the proper version_number calculation.
    #
    self.compiler unless @compiler

    # ./ ../ ~/ /
    if path =~ %r{\A(?:(\.\.?)|(~))?/}
      if $2    # ~ 
        rb.slice! '~/' if rb
        rbc.slice! '~/' if rbc
        ext.slice! '~/' if ext
        res = Compile.single_load "#{ENV['HOME']}/", rb, rbc, ext, requiring, options

      else
        res = Compile.single_load '', rb, rbc, ext, requiring, options
      end

      return res unless res.nil?      # false is valid

    # Unqualified
    else
      $LOAD_PATH.each do |dir|
        if rbc and dir.suffix? '.rba' and File.file? dir and !options[:recompile]
          begin
            _, _, _, _, _, data = Ar.new(dir).extract rbc
          rescue Ar::Error
          end

          cm = if data then
                 unmarshal_object data, 0
               else
                 data = Ar.new(dir).extract rbc
                 unmarshal_object data, version_number
               end

          if cm
            return false if requiring and $LOADED_FEATURES.include? rb

            cm.compile
            cm.hints = { :source => :rba }
            cm.as_script do |script|
              script.path = rb
            end

            $LOADED_FEATURES << rb if requiring

            # Add script CM to CompiledMethod.scripts
            CompiledMethod.scripts[rb] = cm

            return true
          end
          # Fall through
        end

        res = Compile.single_load "#{dir}/", rb, rbc, ext, requiring, options
        return res unless res.nil?      # false is valid
      end
    end

    raise LoadError, "no such file to load -- #{path}"
  end

  def self.compile_feature(rb, requiring, &block)
    $LOADED_FEATURES << rb if requiring
    begin
      yield
    rescue Exception => e
      $LOADED_FEATURES.delete(rb) if requiring
      raise e
    end
  end

  # Internally used by #unified_load. This attempts to load the
  # designated file from a single prefix path.
  def self.single_load(dir, rb, rbc, ext, requiring, options)
    # Force compiler loading, required for version calculation
    self.compiler unless @compiler

    if rb
      return false if requiring and $LOADED_FEATURES.include? rb

      rb_path = "#{dir}#{rb}"

      if File.file? rb_path
        rbc_path = "#{dir}#{rbc}"
        
        cm = nil

        # Try to load rbc directly if requested
        if @load_rbc_directly and File.file?(rbc_path)
          compile_feature(rb, requiring) do
            cm = CompiledMethod.load_from_file(rbc_path, version_number)
            raise LoadError, "Invalid .rbc: #{rbc_path}" unless cm
          end

        # Prefer compiled whenever possible
        elsif !File.file?(rbc_path) or File.mtime(rb_path) > File.mtime(rbc_path) or options[:recompile]
          if $DEBUG_LOADING
            if !File.file?(rbc_path)
              STDERR.puts "[Compiling #{rb_path}: Missing compiled version]"
            else
              STDERR.puts "[Compiling #{rb_path}: Newer source file]"
            end
          end

          compile_feature(rb, requiring) do
            cm = Compile.compile_file(rb_path)
            raise LoadError, "Unable to compile: #{rb_path}" unless cm
          end

          # Store it for the future
          Marshal.dump_to_file cm, rbc_path, version_number
        else
          compile_feature(rb, requiring) do
            cm = CompiledMethod.load_from_file(rbc_path, version_number)
            # cm is nil if the file is out of date, version wise.
            unless cm
              if $DEBUG_LOADING
                STDERR.puts "[Recompling #{rb_path}, old version]"
              end

              compile_feature(rb, requiring) do
                cm = Compile.compile_file(rb_path)
                raise LoadError, "Unable to compile: #{rb_path}" unless cm
              end

              Marshal.dump_to_file cm, rbc_path, version_number
            end
          end
        end

        # Add script CM to CompiledMethod.scripts
        CompiledMethod.scripts[rb] = cm

        begin
          cm.compile
          cm.hints = { :source => :rb }
          # Set a breakpoint on the script CompiledMethod if flag is set
          if @debug_script
            Debugger.instance.set_breakpoint cm, 0
            @debug_script = false
          end
          cm.as_script do |script|
            script.path = rb_path
          end
        rescue Exception => e
          $LOADED_FEATURES.delete(rb) if requiring
          raise e
        end

        return true
      end
    end

    if rbc
      rb = rbc.chomp 'c'
      return false if requiring and $LOADED_FEATURES.include?(rb)

      rbc_path = "#{dir}#{rbc}"

      if File.file? rbc_path and !options[:recompile]
        compile_feature(rb, requiring) do
          cm = CompiledMethod.load_from_file(rbc_path, version_number)
          raise LoadError, "Invalid .rbc: #{rbc_path}" unless cm
        end

        begin
          cm.compile
          cm.hints = { :source => :rbc }
          cm.as_script do |script|
            script.path = rb_path
          end
        rescue Exception => e
          $LOADED_FEATURES.delete(rb) if requiring
          raise e
        end

        return true
      end
    end

    if ext
      return false if requiring and $LOADED_FEATURES.include? ext
      
      ext_path = "#{dir}#{ext}"
      ext_name = File.basename ext, ".#{Rubinius::LIBSUFFIX}"

      if File.file? ext_path
        case Rubinius::VM.load_library(ext_path, ext_name)
        when true
          $LOADED_FEATURES << ext if requiring
          return true
        when 0 # Absent or invalid
          return nil
        when 1 # Valid library, but no entry point
          raise LoadError, "Invalid extension at '#{ext_path}'. " \
                           "Did you define Init_#{ext_name}?"
        end
      end
    end

    nil
  end

  def self.load_from_extension(path)
    path = StringValue(path)
    # Remap all library extensions behind the scenes, just like MRI
    path.gsub!(/\.(so|bundle|dll|dylib)$/, ".#{Rubinius::LIBSUFFIX}")
    if path.suffix? '.rbc'
      rb, rbc, ext = nil, path, nil
    elsif path.suffix? '.rb'
      rb, rbc, ext = path, "#{path}c", nil
    elsif path.suffix? ".#{Rubinius::LIBSUFFIX}"
      rb, rbc, ext = nil, nil, path
    else
      dir, name = File.split(path)
      name = ".#{name}" unless name[0] == ?.
      rb, rbc, ext = path, "#{dir}/#{name}.compiled.rbc", nil
    end

    Compile.single_load '', rb, rbc, ext, false, {}
  end

  def self.unmarshal_object(data, version)
    Ruby.primitive :unmarshal_object
  end

end       # Compile

module Kernel
  def compile(path, out=nil, flags=nil)
    out = "#{path}c" unless out
    cm = Compile.compile_file(path, flags)
    raise LoadError, "Unable to compile '#{path}'" unless cm
    Marshal.dump_to_file cm, out, Compile.version_number
    return out
  end
  module_function :compile

  ##
  # Loads the given file as executable code and returns true. If
  # the file cannot be found, cannot be compiled or some other
  # error occurs, LoadError is raised with an explanation.
  #
  # Unlike #require, the file extension if any must be present but
  # is not restricted to .rb, .rbc or .<platform shared lib ext>.
  # Any other extensions (or no extension) are assumed to be plain
  # Ruby files. The only exceptions to this rule are:
  #
  # 1.  if given a .rb or no/any-extensioned file and there is a
  #     compiled version of the same file that is not older than
  #     the source file (based on File.mtime), the compiled one
  #     is loaded directly to avoid the compilation overhead.
  # 2.  if a .rb file is given but it does not exist, the system
  #     will try to load the corresponding .rbc instead (to allow
  #     distributing just .rbc files.)
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
  # A few extra options are supported. If the second parameter is
  # true, then the module is wrapped inside an anonymous module for
  # loading to avoid polluting the namespace. This is actually a
  # shorthand for passing in :wrap => true-ish in the second arg
  # which may be an option Hash.
  #
  # If :recompile in option Hash is true-ish then the file in
  # question is recompiled each time. If the source file is not
  # present when recompiling is requested, a LoadError is raised.
  #
  # TODO: Support non-UNIX paths.
  #
  # TODO: The anonymous module wrapping is not implemented at all.
  #
  def load(path, opts = {:wrap => false, :recompile => false})
    path = StringValue(path)
    # Remap all library extensions behind the scenes, just like MRI
    path.gsub!(/\.(so|bundle|dll|dylib)$/, ".#{Rubinius::LIBSUFFIX}")
    
    opts = {:wrap => !!opts, :recompile => false} unless Hash === opts

    if path.suffix? '.rbc'
      rb, rbc, ext = nil, path, nil
    elsif path.suffix? '.rb'
      rb, rbc, ext = path, "#{path}c", nil
    elsif path.suffix? ".#{Rubinius::LIBSUFFIX}"
      rb, rbc, ext = nil, nil, path
    else
      dir, name = File.split(path)
      name = ".#{name}" unless name[0] == ?.
      rb, rbc, ext = path, "#{dir}/#{name}.compiled.rbc", nil
    end

    Compile.unified_load path, rb, rbc, ext, nil, opts
  end
  module_function :load

  
  # Attempt to load the given file, returning true if successful.
  # If the file has already been successfully loaded and exists
  # in $LOADED_FEATURES, it will not be re-evaluated and false
  # is returned instead. If the filename cannot be resolved,
  # a LoadError is raised.
  #
  # The file can have one of the following extensions:
  #
  # [.rb]                   Plain Ruby source file.
  # [.rbc]                  Compiled Ruby source file.
  # [.o, .so, .dylib, .dll] Shared library (platform-specific.)
  # [<none>]                Filename without extension.
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
    rb, rbc, ext = __split_path__ path
    Autoload.remove(rb)
    Compile.unified_load path, rb, rbc, ext, true
  end
  module_function :require

  def __split_path__(path)
    # Remap all library extensions behind the scenes, just like MRI
    path.gsub!(/\.(so|bundle|dll|dylib)$/, ".#{Rubinius::LIBSUFFIX}")

    if path.suffix? '.rbc'
      rb, rbc, ext = nil, path, nil
    elsif path.suffix? '.rb'
      rb, rbc, ext = path, "#{path}c", nil
    elsif path.suffix? ".#{Rubinius::LIBSUFFIX}"
      rb, rbc, ext = nil, nil, path
    else
      rb, rbc, ext = "#{path}.rb", "#{path}.rbc", "#{path}.#{Rubinius::LIBSUFFIX}"
    end
    return rb,rbc,ext
  end
  private :__split_path__
end


module Rubinius

  def self.compile_file(file, save=true)
    cm = Rubinius::Compiler.compile_file file, 1

    if save
      if file.suffix? ".rb"
        rbc_path = "#{file}c"
      else
        rbc_path = "#{file}.rbc"
      end

      Rubinius::CompiledFile.dump cm, rbc_path
    end

    return cm
  end

  class InvalidRBC < RuntimeError; end
end

##
# Main entrace point to the Rubinius compiler.  Handles loading and compiling
# ruby code.

class Requirer
  # TODO: This is temporary until the compiler is refactored
  def self.version_number
    Rubinius::Signature
  end

  # TODO: This is temporary until the compiler is refactored
  module Utils

  def self.compiler
    return ::Requirer
  end

  def self.version_number
    Rubinius::Signature
  end

  @load_rbc_directly = false
  def self.loading_rbc_only(val=true)
    old = @load_rbc_directly
    @load_rbc_directly = val
    yield
  ensure
    @load_rbc_directly = old
  end

  def self.execute(string)
    eval(string, TOPLEVEL_BINDING)
  end

  # Sets a flag so that the next script to be loaded gets a breakpoint set at
  # the first instruction
  def self.debug_script!
    @debug_script = true
  end

  # Called when we encounter a break keyword that we do not support
  # TODO - This leaves a moderately lame stack trace entry
  def self.__unexpected_break__
    raise LocalJumpError, "unexpected break"
  end

  def self.load_from_rbc(path, version)
    Ruby.primitive :compiledfile_load

    raise Rubinius::InvalidRBC, path

    # HACK: remove the primitive above when compiled_file.rb
    # unmarshal_data method is fixed and ruby performance is better
    puts "[Loading: #{path}]" if Rubinius::TaskProbe.enabled? :load_runtime
    File.open(path) do |io|
      cf = Rubinius::CompiledFile.load(io)
      # HACK check version!
      return cf.body
    end
  end

  # Internally used by #load and #require. Determines whether to
  # load the file directly or by prefixing it with the paths in
  # $LOAD_PATH and then attempts to locate and load the file.
  def self.unified_load(path, rb, rbc, ext, requiring = nil,
                        options = {:recompile => false})

    # ./ ../ ~/ /
    if path =~ %r{\A(?:(\.\.?)|(~))?/}
      if $2    # ~
        rb.slice! '~/' if rb
        rbc.slice! '~/' if rbc
        ext.slice! '~/' if ext
        res = Requirer::Utils.single_load "#{ENV['HOME']}/", rb, rbc, ext, requiring, options

      else
        res = Requirer::Utils.single_load '', rb, rbc, ext, requiring, options
      end

      return res unless res.nil?      # false is valid

    # Unqualified
    else
      $LOAD_PATH.each do |dir|
        if rbc and File.file?(dir) and dir.suffix?('.rba') and !options[:recompile]
          begin
            _, _, _, _, _, data = Ar.new(dir).extract rbc
          rescue Ar::Error
          end

          cm = if data then
                 # HACK check version
                 Rubinius::CompiledFile.load(data).body
               else
                 data = Ar.new(dir).extract rbc
                 # HACK check version
                 Rubinius::CompiledFile.load(data).body if data
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
            Rubinius::CompiledMethod.scripts[rb] = cm

            return true
          end
          # Fall through
        end

        res = Requirer::Utils.single_load "#{dir}/", rb, rbc, ext, requiring, options
        return res unless res.nil?      # false is valid
      end
    end

    # Crazy special case.
    # MRI does NOT use rb_f_raise(rb_eLoadError, msg),
    # it does rb_exc_raise(rb_exc_new2(rb_eLoadError, msg)).
    #
    # This is significant because some versions of rails change LoadError.new
    # but NOT LoadError.exception, so this code must use LoadError.new
    # directly here to match the behavior.
    raise LoadError.new("no such file to load -- #{path}")
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

  # Indicate if +feature+ is already provied. Inspects the extension
  # to figure out all possible things +feature+ could be.
  def self.provided?(feature)
    rb, rbc, ext = split_path(feature)

    if rb
      return true if $LOADED_FEATURES.include? rb
    end

    # Only check rbc also if we didn't just check rb
    if rbc and !rb
      rb = rbc.chomp 'c'
      return true if $LOADED_FEATURES.include?(rb)
    end

    if ext
      return true if $LOADED_FEATURES.include?(ext)
    end

    return false
  end

  # Internally used by #unified_load. This attempts to load the
  # designated file from a single prefix path.
  def self.single_load(dir, rb, rbc, ext, requiring, options)
    if rb
      return false if requiring and $LOADED_FEATURES.include? rb

      rb_path = "#{dir}#{rb}"
      rb_stat = File::Stat.stat rb_path

      if rb_stat and rb_stat.file?
        if rbc
          rbc_path = "#{dir}#{rbc}"
          rbc_stat = File::Stat.stat rbc_path
        else
          rbc_path = nil
          rbc_stat = nil
        end

        cm = nil

        # Try to load rbc directly if requested. In this mode, we require
        # that the .rbc file exist already if the .rb file does. We fail
        # hard if it doesn't.
        if @load_rbc_directly

          if rbc_stat and rbc_stat.file?
            ver = (@load_rbc_directly == :force ? 0 : version_number)
            compile_feature(rb, requiring) do
              cm = load_from_rbc(rbc_path, ver)
              raise LoadError, "Invalid .rbc: #{rbc_path}" unless cm
            end
          else
            raise LoadError, "Unable to find '#{rbc_path}' to load directly"
          end

        # Prefer compiled whenever possible
        elsif !rbc_stat or
              !rbc_stat.file? or
               rb_stat.mtime > rbc_stat.mtime or
               options[:recompile]

          if $DEBUG_LOADING
            if !rbc_path
              STDERR.puts "[Compiling #{rb_path}: .rbc file disable]"
            elsif !File.file?(rbc_path)
              STDERR.puts "[Compiling #{rb_path}: Missing compiled version]"
            else
              STDERR.puts "[Compiling #{rb_path}: Newer source file]"
            end
          end

          compile_feature(rb, requiring) do
            cm = Rubinius::Compiler.compile_file_old(rb_path)
            raise LoadError, "Unable to compile: #{rb_path}" unless cm
          end

          # Store it for the future
          Rubinius::CompiledFile.dump cm, rbc_path if rbc_path && !options[:no_rbc]
        else
          if $DEBUG_LOADING
            STDERR.puts "[Loading #{rbc_path}]"
          end

          compile_feature(rb, requiring) do
            begin
              cm = load_from_rbc(rbc_path, version_number)
            rescue Rubinius::InvalidRBC
              if $DEBUG_LOADING
                STDERR.puts "[Invalid .rbc #{rbc_path} detected, ignoring]"
              end
              cm = nil
            rescue TypeError, IOError
              cm = nil
            end

            # cm is nil if the file is out of date, version wise.
            unless cm
              if $DEBUG_LOADING
                STDERR.puts "[Recompling #{rb_path}, old version]"
              end

              compile_feature(rb, requiring) do
                cm = Rubinius::Compiler.compile_file_old(rb_path)
                raise LoadError, "Unable to compile: #{rb_path}" unless cm
              end

              Rubinius::CompiledFile.dump cm, rbc_path unless options[:no_rbc]
            end
          end
        end

        # Add script CM to CompiledMethod.scripts
        Rubinius::CompiledMethod.scripts[rb] = cm

        begin
          cm.compile
          cm.hints = { :source => :rb }
          # Set a breakpoint on the script CompiledMethod if flag is set
          if @debug_script
            Rubinius::Debugger.instance.set_breakpoint cm, 0
            @debug_script = false
          end
          cm.as_script do |script|
            script.path = rb_path
            script.root_script = options[:root_script]
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
          cm = load_from_rbc(rbc_path, version_number)
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
      ext_name = File.basename ext, "#{Rubinius::LIBSUFFIX}"

      if File.file? ext_path
        case Rubinius::NativeMethod.load_extension(ext_path, ext_name)
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

  def self.load_from_extension(path, options = {})
    path = StringValue(path)
    # Remap all library extensions behind the scenes, just like MRI
    path.gsub!(/\.(so|bundle|dll|dylib)$/, "#{Rubinius::LIBSUFFIX}")
    if path.suffix? '.rbc'
      rb, rbc, ext = nil, path, nil
    elsif path.suffix? '.rb'
      rb, rbc, ext = path, "#{path}c", nil
    elsif path.suffix? "#{Rubinius::LIBSUFFIX}"
      rb, rbc, ext = nil, nil, path
    else
      dir, name = File.split(path)
      rb = path
      ext = nil

      if name[0] == ?.
        rbc = nil
      else
        rbc = "#{dir}/#{name}.compiled.rbc"
      end
    end

    Requirer::Utils.single_load '', rb, rbc, ext, false, options
  end

  def self.split_path(path)
    # Remap all library extensions behind the scenes, just like MRI
    path.gsub!(/\.(so|bundle|dll|dylib)$/, "#{Rubinius::LIBSUFFIX}")

    if path.suffix? '.rbc'
      rb, rbc, ext = nil, path, nil
    elsif path.suffix? '.rb'
      rb, rbc, ext = path, "#{path}c", nil
    elsif path.suffix? "#{Rubinius::LIBSUFFIX}"
      rb, rbc, ext = nil, nil, path
    else
      rb =  "#{path}.rb"
      ext = "#{path}#{Rubinius::LIBSUFFIX}"

      if name[0] == ?.
        rbc = nil
      else
        rbc = "#{path}.rbc"
      end
    end
    return rb,rbc,ext
  end

  end   # module Utils
end   # class Requirer

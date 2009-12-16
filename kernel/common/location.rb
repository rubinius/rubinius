module Rubinius
  class Location

    attr_accessor :is_jit
    attr_accessor :is_block
    attr_accessor :method
    attr_accessor :name

    def inlined?
      @name.nil?
    end

    def describe
      # We can't call @receiver.class because it might be overriden (or in
      # the case of Mocha, mocked out) and easily causes stack overflows
      klass = Rubinius.object_class @receiver

      if @method_module.equal?(Kernel)
        str = "Kernel."
      elsif @method_module.kind_of? Class and ao = @method_module.__metaclass_object__
        str = "#{ao}."
      elsif @method_module and @method_module != klass
        str = "#{@method_module}(#{klass})#"
      else
        str = "#{klass}#"
      end

      str << describe_method

      return str
    end

    def describe_method
      if @is_block
        "#{@name} {}"
      elsif @name == @method.name
        "#{@name}"
      elsif !@name # inlined methods have no name
        "#{@method.name}"
      else
        "#{@name} (#{@method.name})"
      end

    end

    # Current line being executed by the VM.
    def line
      return 0 unless @method
      # We subtract 1 because the ip is actually set to what it should do
      # next, not what it's currently doing (unless we are at the start of
      # a new context).
      ip = @ip - 1
      ip = 0 if @ip < 0
      @method.line_from_ip(ip)
    end

    def file(relative_to=nil)
      if relative_to
        # Be sure we can bail out if something doesn't work and still
        # show something.
        begin
          full = File.expand_path @method.file.to_s
          if full.prefix? relative_to
            return full[relative_to.size+1..-1]
          else
            return @method.file
          end
        rescue Object => e
          return @method.file
        end
      end

      @method.file
    end

    ##
    # Place in the source that this method was created at.
    def position(relative_to=nil)
      l = line()

      if l == 0
        "#{file(relative_to)}+#{@ip-1}"
      else
        "#{file(relative_to)}:#{l}"
      end
    end

    # See Backtrace.backtrace
    class Missing
      def describe
        "*** Missing backtrace! Did the VM not create one? ***"
      end

      def describe_method
        "missing_backtrace_unknown_location"
      end

      def line
        "<no line>"
      end

      def position
        "<no file>"
      end
    end
  end
end

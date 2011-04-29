module Rubinius
  class Location

    attr_reader :method
    attr_reader :name

    attr_reader :ip
    attr_reader :variables
    attr_reader :static_scope

    def is_block
      @flags & 1 == 1
    end

    def is_jit
      @flags & 2 == 2
    end

    def ip_on_current?
      @flags & 4 == 4
    end

    def inlined?
      @name.nil?
    end

    def describe_receiver
      # We can't call @receiver.class because it might be overriden (or in
      # the case of Mocha, mocked out) and easily causes stack overflows
      klass = Rubinius::Type.object_class @receiver

      if @method_module.equal?(Kernel)
        "Kernel."
      elsif Rubinius::Type.object_kind_of?(@method_module, Class) and
            ao = Rubinius::Type.singleton_class_object(@method_module)
        "#{ao}."
      elsif @method_module and @method_module != klass
        "#{@method_module}(#{klass})#"
      else
        "#{klass}#"
      end
    end

    def describe
      if is_block
        "{ } in #{describe_receiver}#{describe_method}"
      else
        "#{describe_receiver}#{describe_method}"
      end
    end

    def describe_method
      if is_block or @name == @method.name
        @name.to_s
      elsif !@name # inlined methods have no name
        @method.name.to_s
      else
        "#{@name} (#{@method.name})"
      end
    end

    # Current line being executed by the VM.
    def line
      return 0 unless @method

      ip = @ip
      ip -= 1 unless ip_on_current?

      return @method.first_line unless ip > 0

      @method.line_from_ip(ip)
    end

    def file(relative_to=nil)
      path = @method.active_path

      if relative_to
        # Be sure we can bail out if something doesn't work and still
        # show something.
        begin
          full = File.expand_path path
          if full.prefix? relative_to
            return full[relative_to.size+1..-1]
          else
            return path
          end
        rescue StandardError
          return path
        end
      end

      path
    end

    ##
    # Place in the source that this method was created at.
    def position(relative_to=nil)
      l = line()

      if l == 0
        "#{file(relative_to)}+#{@ip}"
      else
        "#{file(relative_to)}:#{l}"
      end
    end

    # See Rubinius::Backtrace.backtrace
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

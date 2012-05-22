class Rubinius::Debugger
  class Frame
    def initialize(debugger, number, loc)
      @debugger = debugger
      @number = number
      @location = loc
    end

    attr_reader :number, :location

    def run(code)
      eval(code, binding)
    end

    def binding
      @binding ||= Binding.setup(
                     @location.variables,
                     @location.method,
                     @location.constant_scope)
    end

    def method
      @location.method
    end

    def line
      @location.line
    end

    def ip
      @location.ip
    end

    def variables
      @location.variables
    end

    def local_variables
      method.local_names
    end

    def describe
      if method.required_args > 0
        locals = []
        0.upto(method.required_args-1) do |arg|
          locals << method.local_names[arg].to_s
        end

        arg_str = locals.join(", ")
      else
        arg_str = ""
      end

      loc = @location

      if loc.is_block
        if arg_str.empty?
          recv = "{ } in #{loc.describe_receiver}#{loc.name}"
        else
          recv = "{|#{arg_str}| } in #{loc.describe_receiver}#{loc.name}"
        end
      else
        if arg_str.empty?
          recv = loc.describe
        else
          recv = "#{loc.describe}(#{arg_str})"
        end
      end

      str = "#{recv} at #{loc.method.active_path}:#{loc.line} (#{loc.ip})"
      if @debugger.variables[:show_ip]
        str << " (+#{loc.ip})"
      end

      str
    end
  end
end

class Rubinius::Debugger
  class BreakPoint

    def self.for_ip(exec, ip, name=:anon)
      line = exec.line_from_ip(ip)

      BreakPoint.new(name, exec, ip, line)
    end

    def initialize(descriptor, method, ip, line, condition=nil)
      @descriptor = descriptor
      @method = method
      @ip = ip
      @line = line
      @for_step = false
      @paired_bp = nil
      @temp = false
      @commands = nil
      @condition = condition

      @set = false
    end

    attr_reader :method, :ip, :line, :paired_bp, :descriptor, :commands, :condition

    def location
      "#{@method.active_path}:#{@line} (+#{ip})"
    end

    def describe
      "#{descriptor} - #{location}"
    end

    def for_step!(scope)
      @temp = true
      @for_step = scope
    end

    def set_temp!
      @temp = true
    end

    def for_step?
      @for_step
    end

    def paired_with(bp)
      @paired_bp = bp
    end

    def activate
      @set = true
      @method.set_breakpoint @ip, self
    end

    def remove!
      return unless @set

      @set = false
      @method.clear_breakpoint(@ip)
    end

    def hit!(loc)
      return true unless @temp

      if @for_step
        return false unless loc.variables == @for_step
      end

      remove!

      @paired_bp.remove! if @paired_bp

      return true
    end

    def delete!
      remove!
    end

    def set_commands(commands)
      @commands = commands
    end

    def has_commands?
      !@commands.nil?
    end

    def set_condition(condition)
      @condition = condition
    end

    def has_condition?
      !@condition.nil?
    end
  end

  class DeferredBreakPoint
    def initialize(debugger, frame, klass, which, name, line=nil, list=nil)
      @debugger = debugger
      @frame = frame
      @klass_name = klass
      @which = which
      @name = name
      @line = line
      @list = list
    end

    def descriptor
      "#{@klass_name}#{@which}#{@name}"
    end

    def resolve!
      begin
        klass = @frame.run(@klass_name)
      rescue NameError
        return false
      end

      begin
        if @which == "#"
          method = klass.instance_method(@name)
        else
          method = klass.method(@name)
        end
      rescue NameError
        return false
      end

      @debugger.info "Resolved breakpoint for #{@klass_name}#{@which}#{@name}"

      @debugger.set_breakpoint_method descriptor, method, @line

      return true
    end

    def describe
      "#{descriptor} - unknown location (deferred)"
    end

    def delete!
      if @list
        @list.delete self
      end
    end
  end
end

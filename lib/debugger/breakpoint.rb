class Debugger
  class BreakPoint

    def self.for_ip(exec, ip, name=:anon)
      line = exec.line_from_ip(ip)

      BreakPoint.new(name, exec, ip, line)
    end

    def initialize(descriptor, method, ip, line)
      @descriptor = descriptor
      @method = method
      @ip = ip
      @line = line
      @for_step = false
      @paired_bp = nil
      @temp = false

      @set = false
    end

    attr_reader :method, :ip, :line, :paired_bp, :descriptor

    def location
      "#{@method.active_path}:#{@line} (+#{ip})"
    end

    def describe
      "#{descriptor} - #{location}"
    end

    def for_step!
      @temp = true
      @for_step = true
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

    def hit!
      return unless @temp

      remove!

      @paired_bp.remove! if @paired_bp
    end

    def delete!
      remove!
    end
  end

  class DeferedBreakPoint
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
      "#{descriptor} - unknown location (defered)"
    end

    def delete!
      if @list
        @list.delete self
      end
    end
  end
end

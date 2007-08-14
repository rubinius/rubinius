module Functions
  def puts(*a)
    a = [""] if a.empty?
    a.each do |obj| 
      str = obj.to_s
      if str[-1] == 10
        $CONSOLE.print str
      else
        $CONSOLE.puts str
      end
    end
    nil
  end
  
  def p(*a)
    a = [nil] if a.empty?
    a.each { |obj| $CONSOLE.puts obj.inspect }
    nil
  end

  def print(*args)
    args.each do |obj|
      $CONSOLE.write obj.to_s
    end
    nil
  end

  def raise(exc=$!, msg=nil, trace=nil)
    if exc.respond_to? :exception
      exc = exc.exception msg
      raise TypeError, 'exception class/object expected' unless Exception === exc
      exc.set_backtrace trace if trace
    elsif exc.kind_of? String or !exc
      exc = RuntimeError.exception exc
    else
      raise TypeError, 'exception class/object expected'
    end

    if $DEBUG
      STDERR.puts "Exception: #{exc.message} (#{exc.class})"
    end

    exc.set_backtrace MethodContext.current.sender unless exc.backtrace
    Ruby.asm "#local exc\nraise_exc"
  end

  def at_exit(&block)
    Rubinius::AtExit.unshift(block)
  end
end

module Kernel
end

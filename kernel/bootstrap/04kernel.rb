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

  def raise(exc=$!, msg=nil)
    if Exception === exc
      # Do nothing, just fast-track it to raising.
    elsif exc.kind_of?(String)
      exc = RuntimeError.new(exc)
    elsif exc.respond_to? :exception
      exc = exc.exception msg
    elsif exc.respond_to? :ancestors
      # This isn't necessary in MRI; Should be fixed in rubinius.
      # All Exceptions _should_ expose an exception method.
      exc = exc.new msg if exc.ancestors.include? Exception
    elsif !exc
      exc = RuntimeError.new("An unknown exception occurred")
    end
    unless Exception === exc
      raise TypeError.new('exception class/object expected')
    end
    
    if $DEBUG
      STDERR.puts "Exception: #{exc.message} (#{exc.class})"
    end
    
    Ruby.asm "push exc\nraise_exc"
  end

  def at_exit(&block)
    Rubinius::AtExit.unshift(block)
  end
end

module Kernel
end

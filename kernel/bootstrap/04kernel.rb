module Kernel
  def puts(*a)
    a = [""] if a.empty?
    a.each do |obj| 
      str = obj.to_s
      if str[-1] == 10
        STDOUT.print str
      else
        STDOUT.puts str
      end
    end
    nil
  end
  
  def p(*a)
    a = [nil] if a.empty?
    a.each { |obj| STDOUT.puts obj.inspect }
    nil
  end

  def print(*args)
    args.each do |obj|
      STDOUT.write obj.to_s
    end
    nil
  end
  
  def raise(exc=$!, msg=nil)
    if !exc
      exc = RuntimeError.new("An unknown exception occurred")
    elsif exc.kind_of?(String)
      exc = RuntimeError.new(exc)
    elsif msg
      cls = exc
      exc = cls.new(msg)
    end
    Ruby.asm "push exc\nraise_exc"
  end
    
  def at_exit(&block)
    Rubinius::AtExit << block
  end
end

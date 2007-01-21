module Kernel
  def puts(*a)
    a = [""] if a.empty?
    a.each { |obj| STDOUT.puts obj.to_s }
  end
  
  def p(*a)
    a = [nil] if a.empty?
    a.each { |obj| STDOUT.puts obj.inspect }
  end

  def print(*args)
    args.each do |obj|
      STDOUT.write obj.to_s
    end
  end
  
  def raise(exc=$!, msg=nil)
    if exc.kind_of?(String)
      exc = RuntimeError.new(exc)
    elsif msg
      cls = exc
      exc = cls.new(msg)
    end
    Ruby.asm "push exc\nraise_exc"
  end
    
  def exit(code=0)
    Process.exit(code)
  end
  
  def at_exit(&block)
    Ruby::AtExit << block
  end
end

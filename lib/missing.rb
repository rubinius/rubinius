# Records missing calls and dumps them at the end.
# Execute with 
#   -rmissing -e '$early = true' script.rb
# to have it raise NoMethodError, otherwise it continues,
# passing a mock down, gathering up missing behavior.
# 
unless defined? $early
  $early = false
end

MissedCalls = []

class MissedCall
  def initialize(recv, name, args)
    @recv, @name = recv.inspect, name
    if args.empty?
      @args = nil
    else
      @args = args.map { |i| i.inspect }.join(", ")
    end
    @sub_calls = []
  end
  
  def method_missing(meth, *args)
    if args.empty?
      args = nil
    else
      args = args.map { |i| i.inspect }.join(", ")
    end
    
    @sub_calls << [meth, args]
    self
  end
  
  def show
    print "#{@recv}.#{@name}"
    if @args
      puts ": #{@args}" 
    else
      puts " (no args)"
    end
    
    @sub_calls.each do |meth, args|
      print "  #{meth}"
      if args
        puts ": #{args}"
      else
        puts " (no args)"
      end
    end
  end
end

class Object
  def method_missing(meth, *args)
    obj = MissedCall.new(self, meth, args)
    MissedCalls << obj
    if $early
      raise NoMethodError, "Couldn't find a '#{meth}' on a #{self.inspect}"
    end
    obj
  end
end

at_exit do
  print "\nDetected #{MissedCalls.size} missed calls.\n\n"
  MissedCalls.each { |c| c.show }
end
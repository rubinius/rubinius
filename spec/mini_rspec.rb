# minispec
#
# Very minimal set of features to support specs like this:
#
# context "Array" do
#   specify "should respond to new" do
#     Array.new.should == []
#   end
# end

class PostiveSpec
  def initialize(obj)
    @obj = obj
  end
  
  def ==(other)
    if @obj != other
      raise Exception.new("equality expected")
    end
  end
end

class NegativeSpec
  def initialize(obj)
    @obj = obj
  end
  
  def ==(other)
    if @obj == other
      raise Exception.new("inequality expected")
    end
  end
end

class Object
  def should
    PostiveSpec.new(self)
  end
  
  def should_not
    NegativeSpec.new(self)
  end
end

def specify(msg)
  print '.'
  begin
    yield
  rescue Exception => e
    print msg
    print " FAILED\n"
    print e.message
    print "\n"
  end
end

def context(msg)
  yield
end

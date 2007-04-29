# mini_rspec.rb
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

def setup
  yield
end

def specify(msg)
  begin
    yield
    STDERR.print '.'
  rescue Exception => e
    STDERR.print 'F'
    STDOUT.print msg
    STDOUT.print " FAILED\n"
    STDOUT.print e.message
    STDOUT.print "\n"
  end
end

def context(msg)
  yield
end

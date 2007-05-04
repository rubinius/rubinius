# mini_rspec.rb
#
# Very minimal set of features to support specs like this:
#
# context "Array" do
#   specify "should respond to new" do
#     Array.new.should == []
#   end
# end

class PositiveSpec
  def initialize(obj)
    @obj = obj
  end
  
  def ==(other)
    if @obj != other
      raise Exception.new("Equality expected for " + @obj.inspect + " and " + other.inspect)
    end
  end
end

class NegativeSpec
  def initialize(obj)
    @obj = obj
  end
  
  def ==(other)
    if @obj == other
      raise Exception.new("Inequality expected for " + @obj.inspect + " and " + other.inspect)
    end
  end
end

class Object
  def should
    PositiveSpec.new(self)
  end
  
  def should_not
    NegativeSpec.new(self)
  end
end

def setup
  yield
end

## PLAIN VERSION
#def specify(msg)
#  begin
#    yield
#    STDOUT.print '.'
#  rescue Exception => e
#    STDOUT.print 'F'
#    STDERR.print msg
#    STDERR.print " FAILED\n"
#    STDERR.print e.message
#    STDERR.print "\n\n"
#  end
#end
#
#def context(msg)
#  yield
#end

# VERBOSE SPEC-LIKE OUTPUT VERSION
def specify(msg)
  STDOUT.print " - "
  STDOUT.print msg

  begin
    yield
  rescue Exception => e
    STDOUT.print " FAILED:\n"

    if e.message != ""
      STDOUT.print e.message
      STDOUT.print ": "
      STDOUT.print "\n"
      STDOUT.print e.backtrace.show
    else
      STDOUT.print "<No message>"
    end
  end

  STDOUT.print "\n"
end

def context(msg)
  STDOUT.print msg
  STDOUT.print "\n-------------------\n"

  yield

  STDOUT.print "\n"
end


# Alternatives
class Object
  alias describe context
  alias it specify
  alias before setup
end

require File.dirname(__FILE__) + '/../simple'

class Simpleton
  def one() 1 end
  def zero() 2 end
end

describe Simpleton do
  it "has #one method" do
    Simpleton.new.one.should == 1
  end
  
  it "does not know what zero is" do
    Simpleton.new.zero.should_not == 0
  end
end

formatter.summary

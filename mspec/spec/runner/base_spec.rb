require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../matchers/output'
require File.dirname(__FILE__) + '/../../runner/formatters/dotted'

class Object; alias_method :spec_describe, :describe; end
require File.dirname(__FILE__) + '/../../runner/base'
class Object
  alias_method :mspec_describe, :describe
  alias_method :mspec_it, :it
  alias_method :describe, :spec_describe
end

describe Object, "#it" do
  before :each do
    @o = Object.new
  end
  
  it "yields to the given block" do
    a = []
    lambda {
      @o.it("") { a << "visited" }
    }.should output(".")
    a.should == ["visited"]
  end
  
  it "rescues exceptions raised in the block" do
    lambda {
      lambda {
        @o.it("") { raise Exception, "I'm unhappy" }
      }.should_not raise_error
    }.should output("E")
  end
end

describe Object, "#describe" do
  before :each do
    @o = Object.new
  end
  
  it "yields to the given block" do
    a = []
    @o.describe("") { a << "visited" }
    a.should == ["visited"]
  end
end

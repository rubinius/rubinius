require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

# DO NOT PUT ANYTHING ABOVE THIS
describe 'Kernel#caller' do
  before :each do
    def a(skip)
      caller(skip)
    end
    def b(skip)
      a(skip)
    end
    def c(skip)
      b(skip)
    end
  end
  
  it "returns the current call stack" do
    c(0)[0][-23..-1].should == "caller_spec.rb:8:in `a'"
    c(0)[1][-24..-1].should == "caller_spec.rb:11:in `b'"
    c(0)[2][-24..-1].should == "caller_spec.rb:14:in `c'"
  end
  
  it "omits a number of frames corresponding to the parameter" do
    c(0)[1..-1].should == c(1)
    c(0)[2..-1].should == c(2)
    c(0)[3..-1].should == c(3)
  end
end

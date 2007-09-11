require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

# DO NOT PUT ANYTHING ABOVE THIS
describe 'Examining the call stack' do
  it "may be viewed through .caller" do
    def a(skip)
      caller(skip)
    end
    def b(skip)
      a(skip)
    end
    def c(skip)
      b(skip)
    end

    c(0)[0][-23..-1].should == "caller_spec.rb:7:in `a'"
    c(0)[1][-24..-1].should == "caller_spec.rb:10:in `b'"
    c(0)[2][-24..-1].should == "caller_spec.rb:13:in `c'"
  end
end

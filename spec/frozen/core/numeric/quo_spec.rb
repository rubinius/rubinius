require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Numeric#quo" do
  ruby_version_is ""..."1.9" do
    it "returns the result of calling self#/ with other" do
      obj = NumericSub.new
      obj.should_receive(:/).with(19).and_return(:result)
      
      obj.quo(19).should == :result
    end
  end

  ruby_version_is "1.9" do
    it "returns the result of calling self#/ with other" do
      obj = NumericSub.new
      obj.should_receive(:coerce).twice.and_return([19,19])
      obj.should_receive(:<=>).any_number_of_times.and_return(1)
      obj.should_receive(:/).and_return(20)
     
      obj.quo(19).should == 20
    end
  end
end

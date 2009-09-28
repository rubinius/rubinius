require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Numeric#modulo" do 
  ruby_version_is "".."1.9" do
    it "returns the result of calling self#% with other" do
      obj = NumericSub.new
      obj.should_receive(:%).with(20).and_return(:result)
      
      obj.modulo(20).should == :result
    end
  end

  ruby_version_is "1.9" do
    it "returns the result of self#- (self#/ other) * other" do
      obj = NumericSub.new
      obj.should_receive(:/).with(15).and_return(13)
      obj.should_receive(:-).with(195).and_return(5)
      # 200 % 15 == 5
      obj.modulo(15).should == 5
    end
  end
end

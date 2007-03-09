require File.dirname(__FILE__) + '/../../../spec_helper.rb'

require 'spec/expectations/differs/default'

context "should ==" do
  
  specify "should pass when objects are ==" do
    lambda do
      "apple".should == "apple"
    end.should_pass
  end

  specify "should fail when objects are not ==" do
    lambda do
      "1".should == 1
    end.should_fail_with "expected 1, got \"1\" (using ==)"
  end

end

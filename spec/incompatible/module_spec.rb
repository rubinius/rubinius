require File.dirname(__FILE__) + '/../spec_helper'

context "Module" do

  specify "const_defined? should return true if the name is defined" do
    class Blah
      class Whee
      end
    end

    Object.const_defined?(:Object).should == true
    Blah.const_defined?("Whee").should == true
    
    # MRI doesn't allow Blah::Whee
    Object.const_defined?("Blah::Whee").should == true
    Object.const_defined?("Blah::Zargle").should == false
  end

end

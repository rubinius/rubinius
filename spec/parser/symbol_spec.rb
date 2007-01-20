require File.dirname(__FILE__) + '/../spec_helper'

context "Parser" do
  specify "should parse a symbol literal that uses single quotes" do
    example do
      :'$4 for one or two'
    end.should == :'$4 for one or two'
  end
end

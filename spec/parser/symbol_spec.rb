require File.dirname(__FILE__) + '/../spec_helper'

describe "Parser" do
  it "parses a symbol literal that uses single quotes" do
    :'$4 for one or two'.should == :'$4 for one or two'
    :'&smack'.should == :'&smack'
  end
end

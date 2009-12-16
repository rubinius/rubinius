require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

describe "YAML.tagurize" do
  it "converts a type_id to a taguri" do
    YAML.tagurize('wtf').should == "tag:yaml.org,2002:wtf"
    YAML.tagurize(1).should == 1
  end  
end

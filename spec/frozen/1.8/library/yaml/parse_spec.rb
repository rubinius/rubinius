require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

describe "YAML#parse with an empty string" do
  it "returns false" do
    YAML.parse('').should be_false
  end
end
  
describe "YAML#parse" do
  before :each do
    @string_yaml = "foo".to_yaml
  end

  it "returns a YAML::Syck::Scalar object " do
    YAML.parse(@string_yaml).should be_kind_of(YAML::Syck::Scalar)
  end
  
  it "returns the value from the object" do
    YAML.parse(@string_yaml).value.should == "foo"
  end  
end

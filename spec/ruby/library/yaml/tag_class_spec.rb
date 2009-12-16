require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

describe "YAML.tag_class" do
  it "associates a taguri tag with a ruby class" do
    YAML.tag_class('rubini.us','rubinius').should == "rubinius"
  end  
end

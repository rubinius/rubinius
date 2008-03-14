require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'

describe "Object#to_yaml" do
  it "returns the YAML representation of a Symbol" do
    :symbol.to_yaml.should ==  "--- :symbol\n"
  end
end

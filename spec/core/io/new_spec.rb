require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO.new" do
  it "raises TypeError if given invalid arguments" do
    lambda { IO.new(nil) }.should raise_error(TypeError)
  end
end

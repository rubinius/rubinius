require File.dirname(__FILE__) + '/../../spec_helper'

describe "Prec.included" do
  it "raises a TypeError when a class mixed with Precision does not specify induced_from" do
    class Foo ;include Precision ;end
    lambda { Foo.induced_from(1) }.should raise_error(TypeError)
  end
end

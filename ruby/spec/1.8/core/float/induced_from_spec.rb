require File.dirname(__FILE__) + '/../../spec_helper'

describe "Float.induced_from" do
  it "returns the argument when passed a Float" do
    Float.induced_from(5.5).eql?(5.5).should == true
  end

  it "returns a Float of the correct value when sent a Fixnum" do
    Float.induced_from(36).should == 36.0
  end

  it "returns a Float of the correct value when sent a Bignum" do
    Float.induced_from(23472398472349872349872349872348972348972439423).should be_close(2.34723984723499e+46, 0.00003e+46)
  end

  it "raises a TypeError if the argument is not a Float, Fixnum, or Bignum" do
    class Foo
      def to_f; 9.9; end
    end
    lambda { Float.induced_from(Foo.new) }.should raise_error(TypeError)
  end
end 

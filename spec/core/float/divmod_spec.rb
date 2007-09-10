require File.dirname(__FILE__) + '/../../spec_helper'

describe "Float#divmod" do
  it "returns an [quotient, modulus] from dividing self by other" do
    3.14.divmod(2).inspect.should == "[1, 1.14]"
    2.8284.divmod(3.1415).inspect.should == "[0, 2.8284]"
    -1.0.divmod(0xffffffff).inspect.should == "[-1, 4294967294.0]"
  end

  # This failed for me on MRI. I'm assuming it is platform dependent -- flgr
  if RUBY_PLATFORM["darwin"] && !defined?(RUBY_ENGINE) then
    it "returns [NaN, NaN] if other is zero" do
      1.0.divmod(0).inspect.should == '[NaN, NaN]'
      1.0.divmod(0.0).inspect.should == '[NaN, NaN]'
    end
  else
    it "raises FloatDomainError if other is zero" do
      should_raise(FloatDomainError) { 1.0.divmod(0) }
      should_raise(FloatDomainError) { 1.0.divmod(0.0) }
    end
  end
end

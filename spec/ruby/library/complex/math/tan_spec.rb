require File.dirname(__FILE__) + '/../../../spec_helper'
require 'complex'
require File.dirname(__FILE__) + '/shared/tan'

describe "Math#tan" do
  it_behaves_like :complex_math_tan, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:tan)
  end
end

ruby_version_is ""..."1.9" do
  describe "Math#tan!" do
    it_behaves_like :complex_math_tan_bang, :_, IncludesMath.new

    it "should be private" do
      IncludesMath.should have_private_instance_method(:tan!)
    end
  end
end

describe "Math.tan" do
  it_behaves_like :complex_math_tan, :_, Math
end

ruby_version_is ""..."1.9" do
  describe "Math.tan!" do
    it_behaves_like :complex_math_tan_bang, :_, Math
  end
end

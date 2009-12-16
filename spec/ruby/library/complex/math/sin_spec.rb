require File.dirname(__FILE__) + '/../../../spec_helper'
require 'complex'
require File.dirname(__FILE__) + '/shared/sin'

describe "Math#sin" do
  it_behaves_like :complex_math_sin, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:sin)
  end
end

ruby_version_is ""..."1.9" do
  describe "Math#sin!" do
    it_behaves_like :complex_math_sin_bang, :_, IncludesMath.new

    it "should be private" do
      IncludesMath.should have_private_instance_method(:sin!)
    end
  end
end

describe "Math.sin" do
  it_behaves_like :complex_math_sin, :_, Math
end

ruby_version_is ""..."1.9" do
  describe "Math.sin!" do
    it_behaves_like :complex_math_sin_bang, :_, Math
  end
end

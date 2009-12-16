require File.dirname(__FILE__) + '/../../../spec_helper'
require 'complex'
require File.dirname(__FILE__) + '/shared/sqrt'

describe "Math#sqrt" do
  it_behaves_like :complex_math_sqrt, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:sqrt)
  end
end

ruby_version_is ""..."1.9" do
  describe "Math#sqrt!" do
    it_behaves_like :complex_math_sqrt_bang, :_, IncludesMath.new

    it "should be private" do
      IncludesMath.should have_private_instance_method(:sqrt!)
    end
  end
end

describe "Math.sqrt" do
  it_behaves_like :complex_math_sqrt, :_, Math
end

ruby_version_is ""..."1.9" do
  describe "Math.sqrt!" do
    it_behaves_like :complex_math_sqrt_bang, :_, Math
  end
end

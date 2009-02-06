require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/cos'

describe "Math#cos" do
  it_behaves_like :complex_math_cos, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:cos)
  end
end

describe "Math#cos!" do
  it_behaves_like :complex_math_cos_bang, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:cos!)
  end
end

describe "Math.cos" do
  it_behaves_like :complex_math_cos, :_, Math
end

describe "Math.cos!" do
  it_behaves_like :complex_math_cos_bang, :_, Math
end

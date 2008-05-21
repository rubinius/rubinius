require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/cos'

describe "Math#cos" do
  it_behaves_like :complex_math_cos, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("cos")
  end
end

describe "Math#cos!" do
  it_behaves_like :complex_math_cos_bang, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("cos!")
  end
end

describe "Math.cos" do
  it_behaves_like :complex_math_cos, Math
end

describe "Math.cos!" do
  it_behaves_like :complex_math_cos_bang, Math
end
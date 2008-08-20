require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/atan'

describe "Math#atan" do
  it_behaves_like :complex_math_atan, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("atan")
  end
end

describe "Math#atan!" do
  it_behaves_like :complex_math_atan_bang, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("atan!")
  end
end

describe "Math.atan" do
  it_behaves_like :complex_math_atan, :_, Math
end

describe "Math.atan!" do
  it_behaves_like :complex_math_atan_bang, :_, Math
end

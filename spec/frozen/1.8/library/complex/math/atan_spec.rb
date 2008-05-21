require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/atan'

describe "Math#atan" do
  it_behaves_like :complex_math_atan, IncludesMath.new
  
  it "should be private" do
    IncludesMath.private_instance_methods.should include("atan")
  end
end

describe "Math#atan!" do
  it_behaves_like :complex_math_atan_bang, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("atan!")
  end
end

describe "Math.atan" do
  it_behaves_like :complex_math_atan, Math
end

describe "Math.atan!" do
  it_behaves_like :complex_math_atan_bang, Math
end

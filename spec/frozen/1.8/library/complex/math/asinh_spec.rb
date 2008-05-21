require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/asinh'

describe "Math#asinh" do
  it_behaves_like :complex_math_asinh, IncludesMath.new
  
  it "should be private" do
    IncludesMath.private_instance_methods.should include("asinh")
  end
end

describe "Math#asinh!" do
  it_behaves_like :complex_math_asinh_bang, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("asinh!")
  end
end

describe "Math.asinh" do
  it_behaves_like :complex_math_asinh, Math
end

describe "Math.asinh!" do
  it_behaves_like :complex_math_asinh_bang, Math
end

require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/acosh'

describe "Math#acosh" do
  it_behaves_like :complex_math_acosh, IncludesMath.new
  
  it "should be private" do
    IncludesMath.private_instance_methods.should include("acosh")
  end
end

describe "Math#acosh!" do
  it_behaves_like :complex_math_acosh_bang, IncludesMath.new

  it "should be private" do
    IncludesMath.private_instance_methods.should include("acosh!")
  end
end

describe "Math.acosh" do
  it_behaves_like :complex_math_acosh, Math
end

describe "Math.acosh!" do
  it_behaves_like :complex_math_acosh_bang, Math
end

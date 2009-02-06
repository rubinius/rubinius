require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/acosh'

describe "Math#acosh" do
  it_behaves_like :complex_math_acosh, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:acosh)
  end
end

describe "Math#acosh!" do
  it_behaves_like :complex_math_acosh_bang, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:acosh!)
  end
end

describe "Math.acosh" do
  it_behaves_like :complex_math_acosh, :_, Math
end

describe "Math.acosh!" do
  it_behaves_like :complex_math_acosh_bang, :_, Math
end

require File.dirname(__FILE__) + '/../../../spec_helper'
require 'complex'
require File.dirname(__FILE__) + '/shared/acos'

describe "Math#acos" do
  it_behaves_like :complex_math_acos, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:acos)
  end
end

ruby_version_is ""..."1.9" do
  describe "Math#acos!" do
    it_behaves_like :complex_math_acos_bang, :_, IncludesMath.new

    it "should be private" do
      IncludesMath.should have_private_instance_method(:acos!)
    end
  end
end

describe "Math.acos" do
  it_behaves_like :complex_math_acos, :_, Math
end

ruby_version_is ""..."1.9" do
  describe "Math.acos!" do
    it_behaves_like :complex_math_acos_bang, :_, Math
  end
end

require File.dirname(__FILE__) + '/../../../spec_helper'
require 'complex'
require File.dirname(__FILE__) + '/shared/atan2'

describe "Math#atan2" do
  it_behaves_like :complex_math_atan2, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:atan2)
  end
end

ruby_version_is ""..."1.9" do
  describe "Math#atan2!" do
    it_behaves_like :complex_math_atan2_bang, :_, IncludesMath.new

    it "should be private" do
      IncludesMath.should have_private_instance_method(:atan2!)
    end
  end
end

describe "Math.atan2" do
  it_behaves_like :complex_math_atan2, :_, Math
end

ruby_version_is ""..."1.9" do
  describe "Math.atan2!" do
    it_behaves_like :complex_math_atan2_bang, :_, Math
  end
end

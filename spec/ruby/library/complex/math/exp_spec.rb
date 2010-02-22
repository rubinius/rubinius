require File.expand_path('../../../../spec_helper', __FILE__)
require 'complex'
require File.expand_path('../shared/exp', __FILE__)

describe "Math#exp" do
  it_behaves_like :complex_math_exp, :_, IncludesMath.new

  it "should be private" do
    IncludesMath.should have_private_instance_method(:exp)
  end
end

ruby_version_is ""..."1.9" do
  describe "Math#exp!" do
    it_behaves_like :complex_math_exp_bang, :_, IncludesMath.new

    it "should be private" do
      IncludesMath.should have_private_instance_method(:exp!)
    end
  end
end

describe "Math.exp" do
  it_behaves_like :complex_math_exp, :_, Math
end

ruby_version_is ""..."1.9" do
  describe "Math.exp!" do
    it_behaves_like :complex_math_exp_bang, :_, Math
  end
end

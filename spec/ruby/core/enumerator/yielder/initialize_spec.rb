# -*- encoding: us-ascii -*-

require File.expand_path('../../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "Enumerator::Yielder#initialize" do
    before(:each) do
      @class = enumerator_class::Yielder
      @uninitialized = @class.allocate
    end

    it "is a private method" do
      @class.should have_private_instance_method(:initialize, false)
    end

    it "returns self when given a block" do
      @uninitialized.send(:initialize) {}.should equal(@uninitialized)
    end
  end
end

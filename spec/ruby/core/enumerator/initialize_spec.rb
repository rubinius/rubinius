# -*- encoding: us-ascii -*-

require File.expand_path('../../../spec_helper', __FILE__)

describe "Enumerator#initialize" do
  before(:each) do
    @uninitialized = enumerator_class.allocate
  end

  it "is a private method" do
    enumerator_class.should have_private_instance_method(:initialize, false)
  end

  it "returns self when given an object" do
    @uninitialized.send(:initialize, Object.new).should equal(@uninitialized)
  end

  ruby_version_is "1.9" do
    it "returns self when given a block" do
      @uninitialized.send(:initialize) {}.should equal(@uninitialized)
    end
  end
end

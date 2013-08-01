# -*- encoding: us-ascii -*-

require File.expand_path('../../../../spec_helper', __FILE__)

ruby_version_is "2.0" do
  describe "Enumerator::Lazy" do
    it "is a subclass of Enumerator" do
      enumerator_class::Lazy.superclass.should equal(enumerator_class)
    end
  end

  describe "Enumerator::Lazy#lazy" do
    it "returns self" do
      lazy = (1..3).to_enum.lazy
      lazy.lazy.should equal(lazy)
    end
  end
end

# -*- encoding: us-ascii -*-

require File.expand_path('../../../../spec_helper', __FILE__)

ruby_version_is "2.0" do
  describe "Enumerator::Lazy" do
    it "is a subclass of Enumerator" do
      enumerator_class::Lazy.superclass.should equal(enumerator_class)
    end
  end
end

# -*- encoding: us-ascii -*-

require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

ruby_version_is "2.0" do
  describe "Enumerable#lazy" do
    it "returns an instance of Enumerator::Lazy" do
      EnumerableSpecs::Numerous.new.lazy.should be_an_instance_of(Enumerator::Lazy)
    end
  end
end

# -*- encoding: us-ascii -*-

require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../shared/select', __FILE__)

ruby_version_is "2.0" do
  describe "Enumerator::Lazy#select" do
    it_behaves_like :enumerator_lazy_select, :select
  end
end

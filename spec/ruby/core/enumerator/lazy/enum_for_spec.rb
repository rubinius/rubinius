# -*- encoding: us-ascii -*-

require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../shared/to_enum', __FILE__)

ruby_version_is "2.0" do
  describe "Enumerator::Lazy#enum_for" do
    it_behaves_like :enumerator_lazy_to_enum, :enum_for
  end
end

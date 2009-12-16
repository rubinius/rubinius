require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/to_a'

ruby_version_is ""..."1.9" do
  describe "String#to_a" do
    it_behaves_like :string_to_a, :to_a
  end
end

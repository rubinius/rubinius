require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/values_at_spec'

ruby_version_is ""..."1.9" do
  describe "Hash#indexes" do
    it_behaves_like(:hash_values_at, :indexes)
  end
end

require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../values_at_spec', __FILE__)

ruby_version_is ""..."1.9" do
  describe "Hash#indices" do
    it_behaves_like(:hash_values_at, :indices)
  end
end

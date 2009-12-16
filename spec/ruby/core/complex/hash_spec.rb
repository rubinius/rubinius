require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/complex/hash'

ruby_version_is "1.9" do
  describe "Complex#hash" do
    it_behaves_like(:complex_hash, :hash)
  end
end

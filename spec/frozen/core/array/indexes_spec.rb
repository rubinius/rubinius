require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/indexes'

ruby_version_is '' ... '1.9' do
  describe "Array#indexes" do
    it_behaves_like(:array_indexes, :indexes)
  end
end

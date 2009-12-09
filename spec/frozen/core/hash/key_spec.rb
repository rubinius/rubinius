require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/key'
require File.dirname(__FILE__) + '/shared/index'

describe "Hash#key?" do
  it_behaves_like(:hash_key_p, :key?)
end

ruby_version_is "1.8.8" do
  describe "Hash#key" do
    it_behaves_like(:hash_index, :key)
  end
end  

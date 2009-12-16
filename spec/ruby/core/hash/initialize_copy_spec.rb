require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/replace'

describe "Hash#initialize_copy" do
  it "is private" do
    hash_class.should have_private_instance_method("initialize_copy")
  end

  it_behaves_like(:hash_replace, :initialize_copy)
end

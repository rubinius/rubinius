require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/replace'

describe "Array#initialize_copy" do
  it "is private" do
    Array.should have_private_instance_method("initialize_copy")
  end

  it_behaves_like(:array_replace, :initialize_copy)
end

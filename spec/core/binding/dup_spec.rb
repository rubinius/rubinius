require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Binding#dup" do
  it_behaves_like(@binding_clone, :dup)
end

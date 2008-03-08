require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/clone'

describe "LookupTable#dup" do
  it_behaves_like :lookuptable_clone, :dup
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/length'

describe "Hash#length" do
  it_behaves_like(:hash_length, :length)
end

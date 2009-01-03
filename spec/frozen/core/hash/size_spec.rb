require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/length'

describe "Hash#size" do
  it_behaves_like(:hash_length, :size)
end

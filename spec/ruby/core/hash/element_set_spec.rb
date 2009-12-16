require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/store'

describe "Hash#[]=" do
  it_behaves_like(:hash_store, :[]=)
end

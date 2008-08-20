require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/key'

describe "Hash#member?" do
  it_behaves_like(:hash_key_p, :member?)
end

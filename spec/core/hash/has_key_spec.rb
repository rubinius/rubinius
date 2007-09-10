require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/key_spec'

describe "Hash#has_key?" do
  it_behaves_like(@hash_key_p, :has_key?)
end


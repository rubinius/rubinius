require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/divide_spec'

describe "Bignum#div" do
  it_behaves_like(@bignum_divide, :div)
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/length_spec'

describe "Array#size" do
  it_behaves_like(@array_length, :size)
end

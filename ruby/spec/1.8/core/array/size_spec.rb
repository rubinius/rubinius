require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/length'

describe "Array#size" do
  it_behaves_like(:array_length, :size)
end

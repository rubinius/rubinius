require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/next'

describe "Integer#succ" do
  it_behaves_like(:integer_next, :succ)
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'
require File.dirname(__FILE__) + '/shared/succ.rb'

describe "String#next" do
  it_behaves_like(:string_succ, :next)
end

describe "String#next!" do
  it_behaves_like(:string_succ_bang, :"next!")
end
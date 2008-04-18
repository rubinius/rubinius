require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/conjugate.rb'

describe "Complex#conj" do
  it_behaves_like(:complex_conjugate, :conj)
end

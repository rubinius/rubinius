require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/conjugate.rb'

describe "Complex#conjugate" do
  it_behaves_like(:complex_conjugate, :conjugate)
end

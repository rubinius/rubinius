require File.expand_path('../../../spec_helper', __FILE__)
require 'matrix'

describe "Matrix#new" do
  
  it "is not accessible" do
    lambda { Matrix.new }.should raise_error(NoMethodError)
  end
  
end

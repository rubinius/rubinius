require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix#new" do
  
  it "is not accessible" do
    lambda { Matrix.new }.should raise_error(NoMethodError)
  end
  
end

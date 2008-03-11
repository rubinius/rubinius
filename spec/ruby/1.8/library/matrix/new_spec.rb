require 'matrix'
require File.dirname(__FILE__) + '/../../spec_helper'

describe "Matrix#new" do
  
  it "should a Matrix" do
    lambda { Matrix.new }.should raise_error
  end
  
end

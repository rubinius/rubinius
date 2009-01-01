require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix#initialize" do
  it "is private" do
    Matrix[].private_methods.should include("initialize")
  end

  it "returns an instance of Matrix" do
    Matrix[].should be_kind_of(Matrix)
  end
end

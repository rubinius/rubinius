require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/determinant'
require 'matrix'

describe "Matrix#determinant" do
  it_behaves_like(:determinant, :determinant)
end

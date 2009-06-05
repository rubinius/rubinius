require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/determinant'
require 'matrix'

describe "Matrix#det" do
  it_behaves_like(:determinant, :det)
end

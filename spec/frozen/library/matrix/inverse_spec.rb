require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/inverse'
require 'matrix'

describe "Matrix#inverse" do
  it_behaves_like(:inverse, :inverse)
end

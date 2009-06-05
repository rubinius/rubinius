require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/equal_value'
require 'matrix'

describe "Matrix#eql?" do
  it_behaves_like(:equal, :eql)
end

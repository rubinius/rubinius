require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/equal_value'
require 'matrix'

describe "Matrix#==" do
  it_behaves_like(:equal, :equal_value)
end

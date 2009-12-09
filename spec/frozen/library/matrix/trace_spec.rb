require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/trace'
require 'matrix'

describe "Matrix#trace" do
  it_behaves_like(:trace, :trace)
end

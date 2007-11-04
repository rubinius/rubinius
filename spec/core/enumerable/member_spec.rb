require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/include'

describe "Enumerable#member?" do
  it_behaves_like(:enumerable_include, :member?) 
end

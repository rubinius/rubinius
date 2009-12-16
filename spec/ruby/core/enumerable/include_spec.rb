require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/include'

describe "Enumerable#include?" do
  it_behaves_like(:enumerable_include, :include?) 
end

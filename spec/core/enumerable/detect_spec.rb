require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/find'

describe "Enumerable#detect" do
  it_behaves_like(:enumerable_find , :detect) 
end

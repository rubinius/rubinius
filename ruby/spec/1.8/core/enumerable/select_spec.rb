require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/find_all'

describe "Enumerable#select" do
  it_behaves_like(:enumerable_find_all , :select) 
end

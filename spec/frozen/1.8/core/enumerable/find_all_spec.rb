require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/find_all'

describe "Enumerable#find_all" do
  it_behaves_like(:enumerable_find_all , :find_all) 
end

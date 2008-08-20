require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/find'

describe "Enumerable#find" do
  it_behaves_like(:enumerable_find , :find) 
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/collect'

describe "Enumerable#map" do   
  it_behaves_like(:enumerable_collect , :map) 
end

describe "Enumerable#map with implicit to_proc [each(&obj) form]" do    
  it_behaves_like(:enumerable_collect_to_proc , :map) 
end

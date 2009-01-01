require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/entries'

describe "Enumerable#to_a" do   
  it_behaves_like(:enumerable_entries , :to_a) 
end

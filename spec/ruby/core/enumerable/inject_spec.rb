require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/inject'

describe "Enumerable#inject" do
  it_behaves_like :enumerable_inject, :inject
end
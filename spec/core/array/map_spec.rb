require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/collect_spec'

describe "Array#map" do
  it_behaves_like(@array_collect, :map)
end

describe "Array#map!" do
  it_behaves_like(@array_collect_b, :map!)
end

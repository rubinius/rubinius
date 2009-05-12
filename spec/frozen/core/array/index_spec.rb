require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/index'

describe "Array#index" do
  it_behaves_like :array_index, :index
end

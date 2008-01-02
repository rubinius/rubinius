require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/new'

describe "Exception.new" do
  it_behaves_like(:exception_new, :new)
end

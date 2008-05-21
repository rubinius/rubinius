require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/new'

describe "Range.compile" do
  it_behaves_like(:regexp_new, :compile)
end

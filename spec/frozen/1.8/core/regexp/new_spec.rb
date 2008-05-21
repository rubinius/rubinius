require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/new'

describe "Regexp.new" do
  it_behaves_like(:regexp_new, :new)
end

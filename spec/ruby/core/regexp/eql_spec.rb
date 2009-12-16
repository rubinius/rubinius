require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/equal_value'

describe "Regexp#eql?" do
  it_behaves_like(:regexp_eql, :eql?)
end

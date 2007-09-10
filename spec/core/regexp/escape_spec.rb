require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/quote_spec'

describe "Range.escape" do
  it_behaves_like(@regexp_quote, :escape)
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/quote'

describe "Regexp.quote" do
  it_behaves_like(:regexp_quote, :quote)
end

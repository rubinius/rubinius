require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/quote', __FILE__)

describe "Range.escape" do
  it_behaves_like(:regexp_quote, :escape)
end

describe "Regexp.escape" do
  it "needs to be reviewed for spec completeness"
end

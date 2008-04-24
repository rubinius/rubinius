require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/mspec'
require File.dirname(__FILE__) + '/../../../runner/filters/regexp'

describe RegexpFilter, "#to_regexp" do
  before :each do
    @filter = RegexpFilter.new nil
  end

  it "converts its arguments to Regexp instances" do
    @filter.to_regexp('a(b|c)', 'b[^ab]', 'cc?').should == [/a(b|c)/, /b[^ab]/, /cc?/]
  end
end

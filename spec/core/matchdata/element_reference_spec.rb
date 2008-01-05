require File.dirname(__FILE__) + '/../../spec_helper'

describe "MatchData#[]" do
  it "provides access to named groups" do
    r = Regexp.new("(?<section>...)(?<code>\d+)")
    m = r.match("THX1138")
    m[:section] == "THX"
    m[:code] == "1138"
  end
end

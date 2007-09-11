require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/length_spec'

describe "MatchData#size" do
  it_behaves_like(@matchdata_length, :size)
end

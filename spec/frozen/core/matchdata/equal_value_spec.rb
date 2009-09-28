require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/eql'

ruby_version_is "1.9" do
  describe "MatchData#==" do
    it_behaves_like(:matchdata_eql, :==)
  end
end

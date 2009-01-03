require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/matched_size.rb'
require 'strscan'

describe "StringScanner#matchedsize" do
  it_behaves_like(:strscan_matched_size, :matchedsize)

  it "warns in verbose mode that the method is obsolete" do
    s = StringScanner.new("abc")
    begin
      old = $VERBOSE
      lambda {
        $VERBOSE = true
        s.matchedsize
      }.should complain(/matchedsize.*obsolete.*matched_size/)

      lambda {
        $VERBOSE = false
        s.matchedsize
      }.should_not complain
    ensure
      $VERBOSE = old
    end
  end
end

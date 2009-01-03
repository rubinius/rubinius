require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/rest_size.rb'
require 'strscan'

describe "StringScanner#restsize" do
  it_behaves_like(:strscan_rest_size, :restsize)

  it "warns in verbose mode that the method is obsolete" do
    s = StringScanner.new("abc")
    begin
      old = $VERBOSE
      lambda {
        $VERBOSE = true
        s.restsize
      }.should complain(/restsize.*obsolete.*rest_size/)

      lambda {
        $VERBOSE = false
        s.restsize
      }.should_not complain
    ensure
      $VERBOSE = old
    end
  end
end

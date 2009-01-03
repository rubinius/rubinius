require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/get_byte'
require 'strscan'

describe "StringScanner#getbyte" do
  it_behaves_like :strscan_get_byte, :getbyte

  it "warns in verbose mode that the method is obsolete" do
    s = StringScanner.new("abc")
    begin
      old = $VERBOSE
      lambda {
        $VERBOSE = true
        s.getbyte
      }.should complain(/getbyte.*obsolete.*get_byte/)

      lambda {
        $VERBOSE = false
        s.getbyte
      }.should_not complain
    ensure
      $VERBOSE = old
    end
  end
end

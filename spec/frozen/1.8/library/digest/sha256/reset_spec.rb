require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/constants'

describe "Digest::SHA256#reset" do

  it 'can returns digest state to initial conditions' do
    cur_digest = Digest::SHA256.new
    cur_digest.update SHA256Constants::Contents
    cur_digest.digest().should_not == SHA256Constants::BlankDigest
    cur_digest.reset
    cur_digest.digest().should == SHA256Constants::BlankDigest
  end

end


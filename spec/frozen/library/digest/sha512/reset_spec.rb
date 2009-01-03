require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/constants'

describe "Digest::SHA512#reset" do

  it 'can returns digest state to initial conditions' do
    cur_digest = Digest::SHA512.new
    cur_digest.update SHA512Constants::Contents
    cur_digest.digest().should_not == SHA512Constants::BlankDigest
    cur_digest.reset
    cur_digest.digest().should == SHA512Constants::BlankDigest
  end

end


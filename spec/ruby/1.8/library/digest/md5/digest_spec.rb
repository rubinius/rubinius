require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/constants'

describe "Digest::MD5#digest" do

  it 'returns a digest' do
    cur_digest = Digest::MD5.new
    cur_digest.digest().should == MD5Constants::BlankDigest
    cur_digest.digest(MD5Constants::Contents).should == MD5Constants::Digest
  end

end

describe "Digest::MD5.digest" do

  it 'returns a digest' do
    Digest::MD5.digest(MD5Constants::Contents).should == MD5Constants::Digest
  end

end

require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/constants'

describe "Digest::MD5#digest_length" do

  it 'returns the length of computed digests' do
    cur_digest = Digest::MD5.new
    cur_digest.digest_length.should == MD5Constants::DigestLength
  end

end


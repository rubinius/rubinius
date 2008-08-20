require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/constants'

describe "Digest::SHA384#digest_length" do

  it 'returns the length of computed digests' do
    cur_digest = Digest::SHA384.new
    cur_digest.digest_length.should == SHA384Constants::DigestLength
  end

end


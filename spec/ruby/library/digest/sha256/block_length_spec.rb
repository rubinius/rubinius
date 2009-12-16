require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/constants'

describe "Digest::SHA256#block_length" do

  it 'returns the length of digest block' do
    cur_digest = Digest::SHA256.new
    cur_digest.block_length.should == SHA256Constants::BlockLength
  end

end


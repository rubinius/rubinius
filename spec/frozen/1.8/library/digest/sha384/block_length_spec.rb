require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/constants'

describe "Digest::SHA384#block_length" do

  it 'returns the length of digest block' do
    cur_digest = Digest::SHA384.new
    cur_digest.block_length.should == SHA384Constants::BlockLength
  end

end


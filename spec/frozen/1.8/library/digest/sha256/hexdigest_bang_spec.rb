require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/constants'

describe "Digest::SHA256#hexdigest!" do

  it 'returns a hexdigest and resets the state' do
    cur_digest = Digest::SHA256.new

    cur_digest << SHA256Constants::Contents
    cur_digest.hexdigest!.should == SHA256Constants::Hexdigest
    cur_digest.hexdigest.should == SHA256Constants::BlankHexdigest
  end

end

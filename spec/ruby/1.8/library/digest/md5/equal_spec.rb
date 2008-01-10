require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/constants'

describe "Digest::MD5#==" do

  it 'should be equal to itself' do
    cur_digest = Digest::MD5.new
    cur_digest.should == cur_digest
  end

  it 'should be equal to string representing its hexdigest' do
    cur_digest = Digest::MD5.new
    cur_digest.should == MD5Constants::BlankHexdigest
  end

  it 'should be equal for same digest different object' do
    cur_digest = Digest::MD5.new
    cur_digest2 = Digest::MD5.new
    cur_digest.should == cur_digest2
  end

end


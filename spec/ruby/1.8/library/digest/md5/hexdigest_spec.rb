require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/constants'

describe "Digest::MD5#hexdigest" do

  it 'returns a hexdigest' do
    cur_digest = Digest::MD5.new
    cur_digest.hexdigest.should == MD5Constants::BlankHexdigest
    cur_digest.hexdigest(MD5Constants::Contents).should == MD5Constants::Hexdigest
    cur_digest.hexdigest.should == MD5Constants::BlankHexdigest
  end

end

describe "Digest::MD5.hexdigest" do

  it 'returns a hexdigest' do
    Digest::MD5.hexdigest(MD5Constants::Contents).should == MD5Constants::Hexdigest
  end

end

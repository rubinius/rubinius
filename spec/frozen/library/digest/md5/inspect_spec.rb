require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/constants'

describe "Digest::MD5#inspect" do

  it 'returns a Ruby object representation' do
    cur_digest = Digest::MD5.new
    cur_digest.inspect.should == "#<#{MD5Constants::Klass}: #{cur_digest.hexdigest()}>"
  end

end


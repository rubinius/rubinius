require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/constants'

describe "Digest::SHA384#inspect" do

  it 'returns a Ruby object representation' do
    cur_digest = Digest::SHA384.new
    cur_digest.inspect.should == "#<#{SHA384Constants::Klass}: #{cur_digest.hexdigest()}>"
  end

end


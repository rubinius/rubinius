require File.dirname(__FILE__) + '/../../spec_helper'

require 'resolv'

describe "Resolv#getnames" do
	it 'resolves 74.201.255.84' do # currently something in engineyard.com
		names = nil
		lambda {
			names = Resolv.getnames("74.201.255.84")
		}.should_not raise_error(Resolv::ResolvError)
		names.should_not == nil
		names.size.should > 0
	end

end

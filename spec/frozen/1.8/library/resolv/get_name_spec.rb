require File.dirname(__FILE__) + '/../../spec_helper'

require 'resolv'

describe "Resolv#getname" do
	it 'resolves 74.201.255.84' do # currently something in engineyard.com
		lambda {
			Resolv.getname("74.201.255.84")
		}.should_not raise_error(Resolv::ResolvError)
		lambda {
			Resolv.getname("should.raise.error")
		}.should raise_error(Resolv::ResolvError)
	end

end

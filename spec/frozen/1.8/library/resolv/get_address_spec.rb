require File.dirname(__FILE__) + '/../../spec_helper'

require 'resolv'

describe "Resolv#getaddress" do
	it 'resolves www.rubyspec.org' do
		lambda {
			address = Resolv.getaddress("www.ruby-lang.org")
		}.should_not raise_error(Resolv::ResolvError)
		lambda {
			address = Resolv.getaddress("should.raise.error")
		}.should raise_error(Resolv::ResolvError)
	end

end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

describe 'Time.times' do
  ruby_version_is "" ... "1.9" do
    it "is an obsolete method" do
      lambda { Time.times }.should complain(/obsolete.*use.*Process::times/)
    end
  end

  ruby_version_is "1.9" do
    it "is no longer exist" do
      lambda { Time.times }.should raise_error(NoMethodError)
    end
  end
end

require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/constants'
require File.dirname(__FILE__) + '/shared/length'

describe "Digest::SHA384#size" do
  it_behaves_like :sha384_length, :size
end


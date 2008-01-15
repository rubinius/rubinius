require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/constants'
require File.dirname(__FILE__) + '/shared/length'

describe "Digest::SHA256#length" do
  it_behaves_like :sha256_length, :length
end


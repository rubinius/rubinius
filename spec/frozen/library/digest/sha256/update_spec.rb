require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/constants'
require File.dirname(__FILE__) + '/shared/update'

describe "Digest::SHA256#update" do
  it_behaves_like :sha256_update, :update
end

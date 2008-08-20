require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/constants'
require File.dirname(__FILE__) + '/shared/update'

describe "Digest::SHA384#update" do
  it_behaves_like :sha384_update, :update
end

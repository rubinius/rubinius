require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/constants'
require File.dirname(__FILE__) + '/shared/update'

describe "Digest::MD5#<<" do
 it_behaves_like(:md5_update, :<<)
end

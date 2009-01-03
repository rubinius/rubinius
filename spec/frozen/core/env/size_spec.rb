require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/length.rb'

describe "ENV.size" do
 it_behaves_like(:env_length, :size)
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/help'

describe "Options#usage" do
  it_behaves_like :lib_options_usage_help, :usage
end

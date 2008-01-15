require 'date' 
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/civil'

describe "Date#civil" do
  it_behaves_like(:date_civil, :civil)
end

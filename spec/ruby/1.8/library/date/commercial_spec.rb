require 'date' 
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/commercial'

describe "Date#commercial" do
  it_behaves_like(:date_commercial, :commercial)
end

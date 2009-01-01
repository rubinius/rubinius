require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/rfc2822'
require 'time'

describe "Time.rfc2822" do
  it_behaves_like :time_rfc2822, :rfc2822
end

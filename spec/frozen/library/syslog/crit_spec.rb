require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/log'
require 'syslog'

describe "Syslog.crit" do
  it_behaves_like :syslog_log, :crit
end

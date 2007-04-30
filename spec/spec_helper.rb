$:.unshift File.dirname(__FILE__)

begin
  if ENV['USE_RSPEC'] == '1'
    require 'rspec_helper'
  else
    require 'mini_rspec'
  end
rescue
  require 'mini_rspec'
end

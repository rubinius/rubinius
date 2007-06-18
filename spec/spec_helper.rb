$:.unshift File.dirname(__FILE__)

begin
  if ENV['USE_RSPEC'] == '1'
    require 'rspec_helper'
  else
    require 'mspec_helper'
  end
rescue
  require 'mspec_helper'
end

if !defined?(RUBY_NAME) then
  begin
    require 'rbconfig'
    RUBY_NAME = Config::CONFIG["RUBY_INSTALL_NAME"]
  rescue Exception
    RUBY_NAME = RUBY_ENGINE
  end
end

def only(*args)
  if Object.const_defined?(:RUBY_ENGINE) and args.include?(RUBY_ENGINE.to_sym)
    yield
  end
end

def except(*args)
  unless Object.const_defined?(:RUBY_ENGINE) and args.include?(RUBY_ENGINE.to_sym)
    yield
  end
end
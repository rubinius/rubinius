source 'https://rubygems.org'

gem "bundler", "~> 1.3"
gem "rake", "~> 10.0"
gem "gems", "~> 0.8"

gem "rubinius-toolset"
gem "rubinius-bridge"
gem "daedalus-core"

mode = ENV["RBX_MODE"]
unless mode
  require File.expand_path("../config.rb", __FILE__)
  mode = Rubinius::BUILD_CONFIG[:language_version]
end

case mode
when "18"
  gem_version = "1.0"
when "21"
  gem_version = "2.0"
else
  raise "Unknown Rubinius language mode: #{mode.inspect}"
end

gem "rubinius-melbourne", "~> #{gem_version}"
gem "rubinius-processor", "~> #{gem_version}"
gem "rubinius-compiler", "~> #{gem_version}"
gem "rubinius-ast", "~> #{gem_version}"

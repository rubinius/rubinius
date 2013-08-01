source 'https://rubygems.org'

gem "rake", "~> 10.0"

gem "rubinius-toolset"
gem "rubinius-bridge"
gem "daedalus-core"

case mode = ENV["RBX_MODE"]
when "18"
  gem_version = "1.0"
when "21"
  gem_version = "2.0"
else
  raise "Unknown Rubinius language mode: #{mode}"
end

gem "rubinius-melbourne", "~> #{gem_version}"
gem "rubinius-processor", "~> #{gem_version}"
gem "rubinius-compiler", "~> #{gem_version}"
gem "rubinius-ast", "~> #{gem_version}"

source 'https://rubygems.org'

gem "rake", "~> 10.0"

gem "rubinius-toolset"
gem "rubinius-bridge"
gem "daedalus-core"

case mode = ENV["RBX_MODE"]
when "18"
  gem "rubinius-melbourne", "~> 1.0"
  gem "rubinius-processor", "~> 1.0"
  gem "rubinius-compiler", "~> 1.0"
  gem "rubinius-ast", "~> 1.0"
when "21"
  gem "rubinius-melbourne", "~> 2.0"
  gem "rubinius-processor", "~> 2.0"
  gem "rubinius-compiler", "~> 2.0"
  gem "rubinius-ast", "~> 2.0"
else
  raise "Unknown Rubinius language mode: #{mode}"
end

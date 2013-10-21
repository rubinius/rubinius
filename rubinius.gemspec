# coding: utf-8

Gem::Specification.new do |spec|
  spec.name          = "rubinius"
  spec.version       = "2.0.1"
  spec.authors       = ["Brian Shirai"]
  spec.email         = ["brixen@gmail.com"]
  spec.description   = %q{A meta-gem for all the Rubinius components that are provided as gems.}
  spec.summary       = %q{A meta-gem for all the Rubinius components that are provided as gems.}
  spec.homepage      = "http://rubini.us"
  spec.license       = "BSD"

  # build tools
  spec.add_dependency "rubinius-build_tools", "~> 2.0"

  # developer tools
  spec.add_dependency "rubinius-developer_tools", "~> 2.0"
end

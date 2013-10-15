# coding: utf-8

Gem::Specification.new do |spec|
  spec.name          = "rubinius"
  spec.version       = "2.0.0"
  spec.authors       = ["Brian Shirai"]
  spec.email         = ["brixen@gmail.com"]
  spec.description   = %q{A meta-gem for the Rubinius components that compile Ruby source code to bytecode.}
  spec.summary       = %q{A meta-gem for the Rubinius build toolchain.}
  spec.homepage      = "http://rubini.us"
  spec.license       = "BSD"

  # build tools
  spec.add_dependency "rubinius-build_tools", "~> 2.0"

  # developer tools
  spec.add_dependency "rubinius-developer_tools", "~> 2.0"
end

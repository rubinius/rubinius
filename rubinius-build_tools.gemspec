# coding: utf-8

Gem::Specification.new do |spec|
  spec.name          = "rubinius-build_tools"
  spec.version       = "2.0.0"
  spec.authors       = ["Brian Shirai"]
  spec.email         = ["brixen@gmail.com"]
  spec.description   = %q{A meta-gem for the Rubinius components that compile Ruby source code to bytecode.}
  spec.summary       = %q{A meta-gem for the Rubinius build toolchain.}
  spec.homepage      = "http://rubini.us"
  spec.license       = "BSD"

  spec.add_dependency "rubinius-ast", "~> 2.0"
  spec.add_dependency "rubinius-compiler", "~> 2.0"
  spec.add_dependency "rubinius-melbourne", "~> 2.0"
  spec.add_dependency "rubinius-processor", "~> 2.0"
  spec.add_dependency "rubinius-toolset", "~> 2.0"
end

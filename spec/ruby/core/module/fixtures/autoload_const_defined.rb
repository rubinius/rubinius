ScratchPad << ModuleSpecs::Autoload.const_defined?(:ForConstDefined)

module ModuleSpecs
  module Autoload
    ScratchPad << const_defined?(:ForConstDefined)

    ForConstDefined = 1

    ScratchPad << const_defined?(:ForConstDefined)

    module DefinedByForConstDefined; end
  end
end

ScratchPad << ModuleSpecs::Autoload.const_defined?(:ForConstDefined)

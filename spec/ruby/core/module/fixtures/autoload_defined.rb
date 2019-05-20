ScratchPad << defined?(ModuleSpecs::AutoloadDefined::ForDefined)
ScratchPad << defined?(ModuleSpecs::AutoloadDefined::ForDefined.size)

module ModuleSpecs
  class AutoloadDefinedSub < AutoloadDefined
    ScratchPad << defined?(ForDefined)
    ScratchPad << defined?(AutoloadDefined::ForDefined)
  end

  class AutoloadDefined
    ScratchPad << defined?(ForDefined)
    ScratchPad << defined?(AutoloadDefined::ForDefined)

    ScratchPad << defined?(ForDefined.size)

    class AutoloadDefinedSub < AutoloadDefined
      ScratchPad << defined?(ForDefined)
    end

    ForDefined = 1

    module DefinedByForDefined; end
  end
end

ScratchPad << defined?(ModuleSpecs::AutoloadDefined::ForDefined)
ScratchPad << defined?(ModuleSpecs::AutoloadDefined::ForDefined.size)

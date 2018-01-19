ScratchPad << defined?(ModuleSpecs::AutoloadDefined::ForRDefined)
ScratchPad << defined?(ModuleSpecs::AutoloadDefined::ForRDefined.size)

module ModuleSpecs
  class AutoloadDefinedSub < AutoloadDefined
    ScratchPad << defined?(ForRDefined)
    ScratchPad << defined?(AutoloadDefined::ForRDefined)
  end

  class AutoloadDefined
    ScratchPad << defined?(ForRDefined)
    ScratchPad << defined?(AutoloadDefined::ForRDefined)

    ScratchPad << defined?(ForRDefined.size)

    class AutoloadDefinedSub < AutoloadDefined
      ScratchPad << defined?(ForRDefined)
    end

    ForRDefined = 1

    module DefinedByForRDefined; end
  end
end

ScratchPad << defined?(ModuleSpecs::AutoloadDefined::ForRDefined)
ScratchPad << defined?(ModuleSpecs::AutoloadDefined::ForRDefined.size)

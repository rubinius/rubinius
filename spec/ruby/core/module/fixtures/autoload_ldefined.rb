ScratchPad << defined?(ModuleSpecs::AutoloadDefined::ForLDefined)

module ModuleSpecs
  class AutoloadDefinedSub < AutoloadDefined
    ScratchPad << defined?(ForLDefined)
    ScratchPad << defined?(AutoloadDefined::ForLDefined)
  end

  class AutoloadDefined
    ScratchPad << defined?(ForLDefined)
    ScratchPad << defined?(AutoloadDefined::ForLDefined)

    ForLDefined = 1
  end
end

ScratchPad << defined?(ModuleSpecs::AutoloadDefined::ForLDefined)

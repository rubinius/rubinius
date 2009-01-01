module ModuleSpecs::AutoloadTestModule2
  module M
    C = 1
  end

  class A
    include M
  end
end
module ModuleSpecs::AutoloadRequire
  module ModuleSpecAutoloadRequire
    def hello
      "Hello, World!"
    end

    module_function :hello
  end
end

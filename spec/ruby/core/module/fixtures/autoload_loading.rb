ScratchPad << File.basename(__FILE__)

ModuleSpecs::Autoload.autoload(:AutoloadAgain,
                               fixture(__FILE__, "autoload_again.rb"))

class ModuleSpecs::Autoload::AutoloadAgain
  def m
    :autoload_again_and_again
  end
end

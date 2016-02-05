module Rubinius
  DEPRECATIONS = {
    # "Description" => "Alternative"
    "Ruby 2.1 features that are incompatible with Ruby 2.2 are deprecated." =>
      "Use Ruby 2.2 features if they are available.",
    "Rubinius::CompiledMethod is deprecated." => "Use Rubinius::CompiledCode instead.",
    "Rubinius::StaticScope is deprecated." => "Use Rubinius::ConstantScope instead.",
    "Rubinius::KERNEL_PATH is deprecated." => "Use Rubinius::CORE_PATH instead.",
  }
end

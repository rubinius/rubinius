module Rubinius
  DEPRECATIONS = {
    # "Description" => "Alternative"
    "Ruby 2.1 features that are incompatible with Ruby 2.2 are deprecated." =>
      "Use Ruby 2.2 features if they are available.",
    "Rubinius::KERNEL_PATH is deprecated." => "Use Rubinius::CORE_PATH instead.",
    "CallCustom is deprecated." =>
      "The replacement will be new executable resolution, cache, and invoke machine instructions.",
  }
end

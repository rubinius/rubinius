module Rubinius
  DEPRECATIONS = {
    # "Description" => "Alternative"
    "Ruby 2.1 features that are incompatible with Ruby 2.2 are deprecated." =>
      "Use Ruby 2.2 features if they are available.",
    "Rubinius::KERNEL_PATH is deprecated." => "Use Rubinius::CORE_PATH instead.",
    "Support for 32-bit platforms is deprecated." =>
      "Use 64-bit platforms or open an issue if 32-bit support is a critical feature.",
    "Support for the GCC compiler is deprecated." =>
      "Use clang or open an issue if GCC support is a critical feature.",
  }
end

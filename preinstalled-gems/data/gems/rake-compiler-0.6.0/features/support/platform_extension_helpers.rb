module PlatformExtensionHelpers
  def binary_extension(platform = RUBY_PLATFORM)
    case platform
      when /darwin/
        'bundle'
      when /mingw|mswin|linux/
        'so'
      else
        RbConfig::CONFIG['DLEXT']
    end
  end
end

World(PlatformExtensionHelpers)

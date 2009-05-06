module Rubinius
  TARGET_IS_186 = true
  TARGET_IS_18 = true
  TARGET_IS_19 = false
  
  def self.target_ruby(version)
    return if RUBY_VERSION >= version
    RUBY_VERSION.replace(version)
    TARGET_IS_186 = RUBY_VERSION <= "1.8.6"
    TARGET_IS_18 = RUBY_VERSION < "1.9"
    TARGET_IS_19 = !TARGET_IS_18
  end
end
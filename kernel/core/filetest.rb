# depends on: module.rb

module FileTest
  def exist?(path)
    File.exist?(path)
  end
  alias_method :exists?, :exist?

  def self.after_loaded
    module_function :exist?, :exists?
  end
end


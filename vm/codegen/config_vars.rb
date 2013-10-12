$:.unshift File.expand_path("../../../library", __FILE__)

require 'rubinius/configuration'

c = Rubinius::ConfigurationVariables.instance

File.open ARGV.shift, "wb" do |f|
  c.write_vm_variables(f)
end

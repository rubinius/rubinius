$:.unshift File.expand_path("../../../lib", __FILE__)

require 'rubinius/configuration'

c = Rubinius::ConfigurationVariables.instance

File.open ARGV.shift, "w" do |f|
  c.write_vm_variables(f)
end

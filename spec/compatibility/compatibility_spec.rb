require 'spec/compatibility/target_context_extension'

context "A Rubinius Interpreter" do
  extend TargetContextExtension

  files = Dir['spec/compatibility/expressions/*']
  expressions = files.collect do |file|
    open(file).read.split("\n")
  end.flatten
  generate_specifications(expressions)
end


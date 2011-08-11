# SpecDataRelation enables concise specs that involve several different forms
# of the same data. This is specifically useful for the parser and compiler
# specs where the output of each stage is essentially related to the input
# Ruby source. Together with the #relates spec method, it enables specs like:
#
#   describe "An If node" do
#     relates "a if b" do
#       parse do
#         # return the expected sexp
#       end
#
#       compile do |g|
#         # return the expected bytecode
#       end
#
#       jit do |as|
#         # return the expected asm/machine code
#       end
#     end
#
#     relates "if a; b; end" do
#       # ...
#     end
#   end

class SpecDataRelation
  # Provides a simple configurability so that any one or more of the possible
  # processes can be run. See the custom options in custom/utils/options.rb.
  def self.enable(process)
    @processors ||= []
    @processors << process
  end

  # Returns true if no process is specifically set or if +process+ is in the
  # list of enabled processes. In other words, all processes are enabled by
  # default, or any combination of them may be enabled.
  def self.enabled?(process)
    @processors.nil? or @processors.include?(process)
  end

  def initialize(ruby)
    @ruby = ruby
  end

  # Formats the Ruby source code for reabable output in the -fs formatter
  # option. If the source contains no newline characters, wraps the source in
  # single quotes to set if off from the rest of the description string. If
  # the source does contain newline characters, sets the indent level to four
  # characters.
  def format(ruby)
    if /\n/ =~ ruby
      lines = ruby.rstrip.each_line.to_a
      if /( *)/ =~ lines.first
        if $1.size > 4
          dedent = $1.size - 4
          ruby = lines.map { |l| l[dedent..-1] }.join
        else
          indent = " " * (4 - $1.size)
          ruby = lines.map { |l| "#{indent}#{l}" }.join
        end
      end
      "\n#{ruby}"
    else
      "'#{ruby}'"
    end
  end

  # Creates spec example blocks if the compile process is enabled.
  def compile(*plugins, &block)
    return unless self.class.enabled? :compiler

    ruby = @ruby
    it "is compiled from #{format ruby}" do
      generator = Rubinius::TestGenerator.new
      generator.instance_eval(&block)

      ruby.should compile_as(generator, *plugins)
    end
  end
end

class Object
  def relates(str, &block)
    SpecDataRelation.new(str).instance_eval(&block)
  end
end

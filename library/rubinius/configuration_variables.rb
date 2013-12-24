module Rubinius
class ConfigurationVariables
  def self.define
    obj = new
    yield obj.root
    @instance = obj
  end

  def self.instance
    @instance
  end

  def initialize
    @root = Section.new(self, nil)
    @variables = []
  end

  attr_reader :root, :variables

  def write_vm_variables(io)
    @variables.each do |v|
      if decl = v.declaration
        io.puts decl
      end
    end

    io.puts "Configuration() :"
    all = @variables.map { |v| v.initializer }.compact
    
    io.puts all.join(",\n")
    io.puts "{"

    @variables.each do |v|
      if des = v.setup
        io.puts des
      end
    end

    io.puts "}"
  end

  def show_help(io)
    max = @variables.map { |v| v.name.size }.max

    @variables.each do |v|
      width = v.name.size
      io.puts "#{' ' * (max - width)}#{v.name}: #{v.description}"

      if s = v.value_info
        io.puts "#{' ' * max}  #{s}"
      end

    end
  end

  class Variable
    def initialize(name, vm=true)
      @name = name
      @default = nil
      @type = nil
      @vm = vm
      @description = nil
      @options = nil

      if vm
        @vm_name = name.gsub(".","_")
      else
        @vm_name = nil
      end
    end

    attr_reader :name
    attr_accessor :default, :type, :description, :vm_name, :options

    def value_info
      if @type == "config::Radio"
        possible = @options[:possible]
        default = @options[:default]

        "default: #{default}, possible: #{possible.map { |x| x[0] }.join(", ")}"
      elsif @default
        "default: #{@default}"
      else
        nil
      end
    end

    def declaration
      return nil unless @vm
      raise "No type set for #{@name}" unless @type
      raise "No vm name set for #{@name}" unless @vm_name

      "#{@type} #{@vm_name};"
    end

    def initializer
      return nil unless @vm

      if @default
        "#{@vm_name}(this, \"#{@name}\", #{@default.inspect})"
      else
        "#{@vm_name}(this, \"#{@name}\")"
      end
    end

    def setup
      return nil unless @vm

      str = "#{@vm_name}.set_description(#{@description.inspect});"

      if @type == "config::Radio"
        possible = @options[:possible]
        unless possible
          raise "Radio type requires the :possible key"
        end

        default = @options[:default] || possible.first

        possible.each do |k,v|
          if k == default
            str << "\n#{@vm_name}.add(#{k.inspect}, #{v.inspect}, true);"
          else
            str << "\n#{@vm_name}.add(#{k.inspect}, #{v.inspect});"
          end
        end
      end

      str
    end
  end

  class Section
    def initialize(config, prefix)
      @config = config
      @prefix = prefix
    end

    def section(name)
      s = Section.new @config, full_name(name)
      yield s
    end

    def full_name(name)
      return name unless @prefix
      "#{@prefix}.#{name}"
    end

    def vm_variable(name, default, options=nil)
      var = Variable.new full_name(name)

      case default
      when Fixnum
        var.default = default
        var.type = "config::Integer"
      when :integer
        var.type = "config::Integer"

      when String
        var.default = default
        var.type = "config::String"
      when :string
        var.type = "config::String"

      when true, false
        var.default = default
        var.type = "config::Bool"
      when :bool
        var.type = "config::Bool"

      when Array
        var.default = default
        var.type = "config::Radio"
      when :radio
        var.type = "config::Radio"
      end

      case options
      when String
        var.description = options
      when Hash
        var.options = options

        var.description = options[:description]
        var.vm_name = options[:as] if options[:as]
      end

      @config.variables << var
    end

  end
end
end

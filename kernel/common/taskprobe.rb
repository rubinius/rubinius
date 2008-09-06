# TaskProbe prints information about various VM operations.
#
# The following are existing probes. More could be added.
#
#   :add_method          - a method is added to a method_table
#   :start_method        - a method will be run
#   :lookup_failed       - unable to locate a method
#   :execute_instruction - an instruction will be run
#
# There is only one TaskProbe instance active at any time,
# but multiple instances can be created. Use the TaskProbe
# class methods +enable+ and +disable+ to control the current
# instance. See also instance method +show+.

class Rubinius::TaskProbe
  # Enables the specified +probe+ in the current
  # TaskProbe instance. Installs a new TaskProbe
  # instance if there is no current instance.
  #
  #   TaskProbe.enable :execute_instruction
  #   # from this point on, show instructions
  #
  def self.enable(probe)
    if p = Rubinius::Task.current.probe
      p.enable probe
    else
      install Rubinius::TaskProbe.new(probe)
    end
  end

  # Disables the specified +probe+ in the current
  # TaskProbe instance. Does nothing if there is
  # no current instance.
  #
  #   TaskProbe.disable :execute_instruction
  #   # from this point on, don't show instructions
  #
  def self.disable(probe)
    if p = Rubinius::Task.current.probe
      p.disable probe
    end
  end

  # Sets the current TaskProbe instance to +task_probe+.
  def self.install(task_probe)
    tp = Rubinius::Task.current.probe
    Rubinius::Task.current.probe = task_probe
    tp
  end

  # Enables one or more probes in the new TaskProbe
  # instance.
  #
  #   probe = TaskProbe.new :start_method, :add_method
  #
  def initialize(*probes)
    probes.each { |probe| enable probe }
  end

  # If passed a block, installs self as the current
  # TaskProbe and yields to the block. Restores the
  # previous TaskProbe instance after yielding.
  #
  # If not passed a block, installs self as the
  # current TaskProbe, overwriting the exiting one
  # if any.
  #
  #   insn = TaskProbe.new :execute_instruction
  #   meth = TaskProbe.new :start_method
  #
  #   insn.show do
  #     ary = [1, 2, 3, 4]
  #     meth.show do
  #       ary.each { |a| p a }
  #     end
  #   end
  #
  def show
    tp = self.class.install self
    return unless block_given?
    yield
    self.class.install tp
  end
end

module MSpec
  def self.describe(mod, msg, &block)
    stack.push RunState.new
    
    current.describe(mod, msg, &block)
    current.process
    
    stack.pop
  end
  
  def self.process
    actions :start
    files
    actions :finish
  end
  
  def self.files
    return unless files = retrieve(:files)

    files.each do |file|
      actions :load
      protect("loading #{file}") { Kernel.load file }
      actions :unload
    end
  end
  
  def self.actions(action, state=nil)
    actions = retrieve(action)
    actions.each { |obj| obj.send action, state } if actions
  end
  
  def self.register_files(files)
    store :files, files
  end
  
  def self.register_mode(mode)
    store :mode, mode
  end
  
  def self.retrieve(symbol)
    instance_variable_get :"@#{symbol}"
  end
  
  def self.store(symbol, value)
    instance_variable_set :"@#{symbol}", value
  end
  
  # This method is used for registering actions that are
  # run at particular points in the spec cycle:
  #   :start        before any specs are run
  #   :load         before a spec file is loaded
  #   :enter        before a describe block is run
  #   :before       before a single spec is run
  #   :expectation  before a 'should', 'should_receive', etc.
  #   :after        after a single spec is run
  #   :leave        after a describe block is run
  #   :unload       after a spec file is run
  #   :finish       after all specs are run
  #
  # Objects registered as actions above should respond to
  # a method of the same name. For example, if an object
  # is registered as a :start action, it should respond to
  # a #start method call.
  #
  # Additionally, there are two "action" lists for 
  # filtering specs:
  #   :include  return true if the spec should be run
  #   :exclude  return true if the spec should NOT be run
  #
  def self.register(symbol, action)
    unless value = retrieve(symbol)
      value = store symbol, []
    end
    value << action
  end
  
  def self.unregister(symbol, action)
    if value = retrieve(symbol)
      value.delete action
    end
  end
end

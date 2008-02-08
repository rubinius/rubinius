class Selector
  ivar_as_index :name => 0, :send_sites => 1

  attach_function 'selector_lookup', :lookup, [:state, :object], :object
  attach_function 'selector_clear',  :clear_object,  [:state, :object], :void
  attach_function 'selector_clear_by_name', :clear, [:state, :object], :void

  def inspect
    "#<Selector name=#{@name} sites=#{@send_sites.size}>"
  end

  def clear
    Selector.clear_object self
  end
end

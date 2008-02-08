class SendSite

  attach_function "send_site_at", :at, [:state, :object, :int], :object
  attach_function 'send_site_create', :create, [:state, :object, :pointer], :object

  def self.new(name)
    SendSite.create name, nil
  end

  def name
    SendSite.at self, 0
  end

  def selector
    SendSite.at self, 1
  end

  def inspect
    "#<SendSite:0x#{object_id.to_s(16)} #{name}>"
  end
end

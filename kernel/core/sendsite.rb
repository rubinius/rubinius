class SendSite

  attach_function 'send_site_create', :create, [:state, :object], :object

  def self.new(name)
    SendSite.create name
  end
end

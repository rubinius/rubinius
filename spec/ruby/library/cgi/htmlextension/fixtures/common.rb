module CGISpecs
  class HtmlExtension
    def initialize
      self.extend(CGI::Html4)
      self.element_init
      self.extend(CGI::HtmlExtension)
    end
  end
  
  def self.split(string)
    string.split("<").reject { |x| x.empty? }.map { |x| "<#{x}" }
  end
end
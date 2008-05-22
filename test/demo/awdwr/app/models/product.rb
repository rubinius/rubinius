#---
# Excerpted from "Agile Web Development with Rails, 3rd Ed.",
# published by The Pragmatic Bookshelf.
# Copyrights apply to this code. It may not be used to create training material, 
# courses, books, articles, and the like. Contact us if you are in doubt.
# We make no guarantees that this code is fit for any purpose. 
# Visit http://www.pragmaticprogrammer.com/titles/rails3 for more book information.
#---


class Product < ActiveRecord::Base
  has_many :line_items
  # ...

  def self.find_products_for_sale
    find(:all, :order => "title")
  end

  # validation stuff...

  validates_presence_of :title, :description, :image_url
  validates_numericality_of :price
  validate :price_must_be_at_least_a_cent
  validates_uniqueness_of :title
  validates_format_of(:image_url,
                      :with    => %r{\.(gif|jpg|png)$}i,
                      :message => 'must be a graphic URL: image\.(gif|jpg|png)')

  protected
  def price_must_be_at_least_a_cent
    errors.add(:price, 'should be at least 0.01') if price.nil? ||
      price < 0.01
  end

end

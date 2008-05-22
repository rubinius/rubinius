#---
# Excerpted from "Agile Web Development with Rails, 3rd Ed.",
# published by The Pragmatic Bookshelf.
# Copyrights apply to this code. It may not be used to create training material, 
# courses, books, articles, and the like. Contact us if you are in doubt.
# We make no guarantees that this code is fit for any purpose. 
# Visit http://www.pragmaticprogrammer.com/titles/rails3 for more book information.
#---
require "#{File.dirname(__FILE__)}/../test_helper"

class DslUserStoriesTest < ActionController::IntegrationTest
  fixtures :products


  
  DAVES_DETAILS = {
      :name     => "Dave Thomas",
      :address  => "123 The Street",
      :email    => "dave@pragprog.com",
      :pay_type => "check"
  }
  

  MIKES_DETAILS = {
      :name     => "Mike Clark",
      :address  => "345 The Avenue",
      :email    => "mike@pragmaticstudio.com",
      :pay_type => "cc"
  }
  
  
    
  def setup
    LineItem.delete_all
    Order.delete_all
    @ruby_book = products(:ruby_book)
    @rails_book = products(:rails_book)
  end 
  
  # A user goes to the store index page. They select a product,
  # adding it to their cart. They then check out, filling in
  # their details on the checkout form. When they submit,
  # an order is created in the database containing
  # their information, along with a single line item
  # corresponding to the product they added to their cart.
  
  
  def test_buying_a_product
    dave = regular_user
    dave.get "/store/index"
    dave.is_viewing "index"
    dave.buys_a @ruby_book
    dave.has_a_cart_containing @ruby_book
    dave.checks_out DAVES_DETAILS
    dave.is_viewing "index"
    check_for_order DAVES_DETAILS, @ruby_book
  end
  

  
  def test_two_people_buying
    dave = regular_user
        mike = regular_user
    dave.buys_a @ruby_book
        mike.buys_a @rails_book
    dave.has_a_cart_containing @ruby_book
    dave.checks_out DAVES_DETAILS
        mike.has_a_cart_containing @rails_book
    check_for_order DAVES_DETAILS, @ruby_book
        mike.checks_out MIKES_DETAILS
        check_for_order MIKES_DETAILS, @rails_book
  end
  
  
  
  def regular_user
    open_session do |user|
      def user.is_viewing(page)
        assert_response :success
        assert_template page
      end
    
      def user.buys_a(product)
        xml_http_request :put, "/store/add_to_cart", :id => product.id
        assert_response :success 
      end
    
      def user.has_a_cart_containing(*products)
        cart = session[:cart]
        assert_equal products.size, cart.items.size
        for item in cart.items
          assert products.include?(item.product)
        end
      end
    
      def user.checks_out(details)
        post "/store/checkout"
        assert_response :success
        assert_template "checkout"

        post_via_redirect "/store/save_order",
                          :order => { :name     => details[:name],
                                     :address  => details[:address],
                                     :email    => details[:email],
                                     :pay_type => details[:pay_type]
                                    }
        assert_response :success
        assert_template "index"
        assert_equal 0, session[:cart].items.size
      end
    end  
  end
  
  
  def check_for_order(details, *products)
    order = Order.find_by_name(details[:name])
    assert_not_nil order
    
    assert_equal details[:name],     order.name
    assert_equal details[:address],  order.address
    assert_equal details[:email],    order.email
    assert_equal details[:pay_type], order.pay_type
    
    assert_equal products.size, order.line_items.size
    for line_item in order.line_items
      assert products.include?(line_item.product)
    end
  end
end
